import json
import numpy as np
from lib.rabbitmq import RpcClient
from gym import spaces


class Dispatcher(object):
    def __init__(self):
        self.rpc_client = RpcClient()

    def make(self, num_envs, test=False):
        data = {
            'call': 'make',
            'num_envs': num_envs,
            'test': test
        }
        json_str = json.dumps(data)

        response = self.rpc_client.call(json_str)
        if response['status'] and not test:
            self.observation_space = response['observation_space']
            self.action_space = response['action_space']
            return True
        return False

    def step(self, actions, test=False):
        data = {
            'call': 'step',
            'action': actions,
            'test': test
        }
        json_str = json.dumps(data)

        response = self.rpc_client.call(json_str)
        return response['observation'], response['reward'], response['done'], ""

    def reset(self, test=False):
        data = {
            'call': 'reset',
            'test': test
        }
        json_str = json.dumps(data)

        response = self.rpc_client.call(json_str)
        return response['observation']


class RemoteVecEnv(object):
    def __init__(self, num_envs):
        self.dispatcher = Dispatcher()
        self.num_envs = num_envs
        if self.dispatcher.make(num_envs):
            print("Successfully make ", num_envs, " remote environment")
        else:
            print("Error creating ", num_envs, " remote environment, please check remote server.")
        self.action_space = spaces.Box(np.array([-1, -1, -1]), np.array([1, 1, 1]), dtype=np.float32)
        # high = np.array([0.9] * self.dispatcher.observation_space[0])
        high = np.array([np.inf] * self.dispatcher.observation_space[0])
        low = np.array([0] * self.dispatcher.observation_space[0])
        self.observation_space = spaces.Box(-high, high, dtype=np.float32)

    def reset(self, test=False):
        observation = self.dispatcher.reset(test)
        return np.asarray(observation, dtype=np.float32)

    def step(self, action, test=False):
        action_ = np.reshape(action, action.size)
        action_list = action_.tolist()
        # TODO: reshape action & change done data type

        next_state, reward, done, _ = self.dispatcher.step(action_list, test)
        next_state = np.asarray(next_state, dtype=np.float32)
        reward = np.asarray(reward, dtype=np.float32)
        done = np.asarray(done, dtype=bool)
        return next_state, reward, done, _