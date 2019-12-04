//
//  SubproVecEnv.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/27.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "SubproVecEnv.hpp"

SubproVecEnv::SubproVecEnv(std::vector<RobotHopper*> &env_fns) : VecEnv((int)env_fns.size())
{
    waiting = false;
    closed = false;

    vecOfEnvs = env_fns;
    action_space = vecOfEnvs[0]->GetActionSpace();

    std::vector<std::thread> thds;
    for (int i = 0; i < num_envs; i++)
    {
        std::queue<CMD> *task = new std::queue<CMD>();
        vecOfTasks.push_back(task);
        vecOfThreads.push_back(std::thread(worker, task, std::ref(vecOfEnvs[i])));
    }
    
    std::cout << "action space X: " << vecOfEnvs[0]->GetActionSpace()->X << " observation space X: " << vecOfEnvs[1]->GetObservationSpace()->X << std::endl;
}

SubproVecEnv::~SubproVecEnv()
{
    for (int i = 0; i < num_envs; i++) {
        (vecOfThreads)[i].join();
    }
    for (auto p : vecOfTasks)
    {
        delete p;
    }
    vecOfTasks.clear();
    for (auto p : vecOfEnvs)
    {
        delete p;
    }
    vecOfEnvs.clear();
}


void SubproVecEnv::Step(float *actions, std::vector<Result*>* results)
{
    for (int i = 0; i < num_envs; i++)
    {
        vecOfEnvs[i]->SetAction(&actions[i * action_space->X]);
        push_cmd(vecOfTasks[i], STEP);
    }
    waiting = true;
    while (waiting)
    {
        // TODO: sleep cpu
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int count = 0;
        for (int i = 0; i < num_envs; i++)
        {
            if (vecOfEnvs[i]->steped)
            {
                count++;
            }
        }
        if (count == num_envs)
        {
            waiting = false;
        }
    }
    for (int i = 0; i < num_envs; i++)
    {
        vecOfEnvs[i]->steped = false;
        (*results)[i] = vecOfEnvs[i]->GetResult();
    }
}

void SubproVecEnv::Reset(std::vector<Result*>* results)
{
    for (int i = 0; i < num_envs; i++)
    {
        push_cmd(vecOfTasks[i], RESET);
    }
    waiting = true;
    while (waiting)
    {
        // TODO: sleep cpu
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int count = 0;
        for (int i = 0; i < num_envs; i++)
        {
            if (vecOfEnvs[i]->steped)
            {
                count++;
            }
        }
        if (count == num_envs)
        {
            waiting = false;
        }
    }
    for (int i = 0; i < num_envs; i++)
    {
        vecOfEnvs[i]->steped = false;
        (*results)[i] = vecOfEnvs[i]->GetResult();
    }
}

void SubproVecEnv::Close()
{
    for (int i = 0; i < num_envs; i++)
    {
        push_cmd(vecOfTasks[i], CLOSE);
    }
}
