import argparse
import math
import time
import os
import pprint
import random
import gym
import numpy as np

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.distributions import Normal
from tensorboardX import SummaryWriter

from lib.common import mkdir
from lib.model import ActorCritic
from lib.multiprocessing_env import SubprocVecEnv
from lib.fake_gym import RemoteVecEnv
from ui.panels import TrainingVisualizer, ActionLog, ParameterControl, Logs


# Default parameters
NUM_ENVS            = 8
#ENV_ID              = "Pendulum-v0"
HIDDEN_SIZE         = 256
LEARNING_RATE       = 1e-4
GAMMA               = 0.99
GAE_LAMBDA          = 0.95
PPO_EPSILON         = 0.2
CRITIC_DISCOUNT     = 0.5
ENTROPY_BETA        = 0.001
PPO_STEPS           = 256
MINI_BATCH_SIZE     = 64
PPO_EPOCHS          = 10
TEST_EPOCHS         = 10
NUM_TESTS           = 10
TARGET_REWARD       = 8000


class PPO_Train:
    def __init__(self, logQueue):
        self.settings = {
            'NUM_ENVS': 8,
            'ENV_NAME': 'HopperRobot',
            'HIDDEN_SIZE': 256,
            'LEARNING_RATE': 1e-4,
            'GAMMA': 0.99,
            'GAE_LAMBDA': 0.95,
            'PPO_EPSILON': 0.2,
            'CRITIC_DISCOUNT': 0.5,
            'ENTROPY_BETA': 0.001,
            'PPO_STEPS': 256,
            'MINI_BATCH_SIZE': 64,
            'PPO_EPOCHS': 10,
            'TEST_EPOCHS': 10,
            'NUM_TESTS': 10,
            'TARGET_REWARD': 8000
        }
        self.logQueue = logQueue

    def test_env(self, env, model, device, deterministic=True):
        state = env.reset(True)
        done = False
        total_reward = 0
        while not done:
            # env.render()
            state = torch.FloatTensor(state).unsqueeze(0).to(device)
            dist, _ = model(state)
            action = dist.mean.detach().cpu().numpy()[0] if deterministic \
                else dist.sample().cpu().numpy()[0]
            next_state, reward, done, _ = env.step(action, True)
            state = next_state
            total_reward += reward
        return total_reward

    def normalize(self, x):
        x -= x.mean()
        x /= (x.std() + 1e-8)
        return x

    def compute_gae(self, next_value, rewards, masks, values, gamma=GAMMA, lam=GAE_LAMBDA):
        values = values + [next_value]
        gae = 0
        returns = []
        for step in reversed(range(len(rewards))):
            delta = rewards[step] + gamma * values[step + 1] * masks[step] - values[step]
            gae = delta + gamma * lam * masks[step] * gae
            # prepend to get correct order back
            returns.insert(0, gae + values[step])
        return returns

    def ppo_iter(self, states, actions, log_probs, returns, advantage):
        batch_size = states.size(0)
        # generates random mini-batches until we have covered the full batch
        for _ in range(batch_size // MINI_BATCH_SIZE):
            rand_ids = np.random.randint(0, batch_size, MINI_BATCH_SIZE)
            yield states[rand_ids, :], actions[rand_ids, :], log_probs[rand_ids, :], returns[rand_ids, :], advantage[
                                                                                                           rand_ids, :]

    def ppo_update(self, frame_idx, states, actions, log_probs, returns, advantages, clip_param=PPO_EPSILON):
        count_steps = 0
        sum_returns = 0.0
        sum_advantage = 0.0
        sum_loss_actor = 0.0
        sum_loss_critic = 0.0
        sum_entropy = 0.0
        sum_loss_total = 0.0

        # PPO EPOCHS is the number of times we will go through ALL the training data to make updates
        for _ in range(PPO_EPOCHS):
            # grabs random mini-batches several times until we have covered all data
            for state, action, old_log_probs, return_, advantage in self.ppo_iter(states, actions, log_probs, returns,
                                                                             advantages):
                dist, value = self.model(state)
                entropy = dist.entropy().mean()
                new_log_probs = dist.log_prob(action)

                ratio = (new_log_probs - old_log_probs).exp()
                surr1 = ratio * advantage
                surr2 = torch.clamp(ratio, 1.0 - clip_param, 1.0 + clip_param) * advantage

                actor_loss = - torch.min(surr1, surr2).mean()
                critic_loss = (return_ - value).pow(2).mean()

                loss = CRITIC_DISCOUNT * critic_loss + actor_loss - ENTROPY_BETA * entropy

                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()

                # track statistics
                sum_returns += return_.mean()
                sum_advantage += advantage.mean()
                sum_loss_actor += actor_loss
                sum_loss_critic += critic_loss
                sum_loss_total += loss
                sum_entropy += entropy

                count_steps += 1

        self.writer.add_scalar("returns", sum_returns / count_steps, frame_idx)
        self.writer.add_scalar("advantage", sum_advantage / count_steps, frame_idx)
        self.writer.add_scalar("loss_actor", sum_loss_actor / count_steps, frame_idx)
        self.writer.add_scalar("loss_critic", sum_loss_critic / count_steps, frame_idx)
        self.writer.add_scalar("entropy", sum_entropy / count_steps, frame_idx)
        self.writer.add_scalar("loss_total", sum_loss_total / count_steps, frame_idx)

    def Start(self, debugOutputQueue, pauseQueue):
        mkdir('.', 'checkpoints')
        parser = argparse.ArgumentParser()
        parser.add_argument("-n", "--name", default=self.settings['ENV_NAME'], help="Name of the run")
        args = parser.parse_args()
        self.writer = SummaryWriter(comment="ppo_" + args.name)

        # Autodetect CUDA
        use_cuda = torch.cuda.is_available()
        device = torch.device("cuda" if use_cuda else "cpu")
        self.logQueue.put(pprint.pformat('Device:' + device.type))

        # Prepare environments
        envs = RemoteVecEnv(NUM_ENVS)
        num_inputs = envs.observation_space.shape[0]
        num_outputs = envs.action_space.shape[0]

        self.logQueue.put('Successfully make 8 remake environment')

        self.model = ActorCritic(num_inputs, num_outputs, HIDDEN_SIZE).to(device)
        self.logQueue.put(pprint.pformat(self.model))
        self.optimizer = optim.Adam(self.model.parameters(), lr=LEARNING_RATE)

        frame_idx = 0
        train_epoch = 0
        best_reward = None

        state = envs.reset()
        early_stop = False

        while not early_stop:

            log_probs = []
            values = []
            states = []
            actions = []
            rewards = []
            masks = []

            for _ in range(PPO_STEPS):
                state = torch.FloatTensor(state).to(device)
                dist, value = self.model(state)

                action = dist.sample()
                # each state, reward, done is a list of results from each parallel environment
                action_exp = action.cpu().numpy()
                next_state, reward, done, _ = envs.step(action_exp)
                log_prob = dist.log_prob(action)

                log_probs.append(log_prob)
                values.append(value)
                rewards.append(torch.FloatTensor(reward).unsqueeze(1).to(device))
                masks.append(torch.FloatTensor(1 - done).unsqueeze(1).to(device))

                states.append(state)
                actions.append(action)

                state = next_state
                frame_idx += 1

                debugData = (next_state, reward, done, action_exp)
                debugOutputQueue.put(debugData)

                while pauseQueue.qsize() > 0:
                    if pauseQueue.qsize() == 1:
                        time.sleep(1)
                    else:
                        while not pauseQueue.empty():
                            pauseQueue.get()


            next_state = torch.FloatTensor(next_state).to(device)
            _, next_value = self.model(next_state)
            returns = self.compute_gae(next_value, rewards, masks, values)

            returns = torch.cat(returns).detach()
            log_probs = torch.cat(log_probs).detach()
            values = torch.cat(values).detach()
            states = torch.cat(states)
            actions = torch.cat(actions)
            advantage = returns - values
            advantage = self.normalize(advantage)

            self.ppo_update(frame_idx, states, actions, log_probs, returns, advantage)
            train_epoch += 1

            if train_epoch % TEST_EPOCHS == 0:
                test_reward = np.mean([self.test_env(envs, self.model, device) for _ in range(NUM_TESTS)])
                self.writer.add_scalar("test_rewards", test_reward, frame_idx)
                self.logQueue.put(pprint.pformat('Frame %s. reward: %s' % (frame_idx, test_reward)))
                # Save a checkpoint every time we achieve a best reward
                if best_reward is None or best_reward < test_reward:
                    if best_reward is not None:
                        self.logQueue.put(pprint.pformat("Best reward updated: %.3f -> %.3f" % (best_reward, test_reward)))
                        name = "%s_best_%+.3f_%d.dat" % (args.name, test_reward, frame_idx)
                        fname = os.path.join('.', 'checkpoints', name)
                        torch.save(self.model.state_dict(), fname)
                    best_reward = test_reward
                if test_reward > TARGET_REWARD: early_stop = True