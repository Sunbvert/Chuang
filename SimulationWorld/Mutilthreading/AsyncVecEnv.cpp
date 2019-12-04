//
//  AsyncVecEnv.cpp
//  Chuang
//
//  Created by 孙川 on 2019/12/4.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "AsyncVecEnv.hpp"

AsyncVecEnv::AsyncVecEnv(std::vector<RobotHopper> &env_fns) : VecEnv((int)env_fns.size())
{
    waiting = false;
    closed = false;
    
    action_space = env_fns[0].GetActionSpace();
    for (int i = 0; i < num_envs; i++)
    {
        hoppers.push_back(std::make_shared<RobotHopper>());
    }
    int observerId = 0;
    for (auto hopper : hoppers)
    {
        observers.push_back(CmdObserver(hopper, observerId));
        observerId++;
    }
}

AsyncVecEnv::~AsyncVecEnv()
{
    
}

void AsyncVecEnv::Step(float *actions, std::vector<Result *> *result)
{

}

void AsyncVecEnv::Reset(std::vector<Result *> *results)
{
    
}

void AsyncVecEnv::Close()
{
    
}
