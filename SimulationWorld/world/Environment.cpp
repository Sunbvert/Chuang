//
//  Environment.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/17.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "Environment.hpp"

#include <iostream>
#include <assert.h>
#include <thread>

void Environment::SetAction(float *_action)
{
    action = _action;
    // std::cout << std::this_thread::get_id() << " set " << action[1] << std::endl;
}

float* Environment::GetAction()
{
    if (!action)
    {
        std::cout << "ERROR: trying to get null action" << std::endl;
        assert(action);
    }
    // std::cout << std::this_thread::get_id() << " get " << action[1] << std::endl;
    return action;
}

void Environment::SetResult(Result *_result)
{
    result = _result;
}

Result* Environment::GetResult()
{
    if (!result)
    {
        std::cout << "ERROR: trying to get null result" << std::endl;
        assert(result);
    }
    return result;
}

void Environment::SetActionSpace(int x, int y)
{
    if (action_space)
    {
        action_space->Set(x, y);
    }
    else
    {
        action_space = new Space();
        action_space->Set(x, y);
    }
}

void Environment::SetObservationSpace(int x, int y)
{
    if (observation_space)
    {
        observation_space->Set(x, y);
    }
    else
    {
        observation_space = new Space();
        observation_space->Set(x, y);
    }
}

Space* Environment::GetActionSpace()
{
    if (!action_space)
    {
        std::cout << "ERROR: trying to get null action space" << std::endl;
        assert(action_space);
    }
    return action_space;
}

Space* Environment::GetObservationSpace()
{
    if (!observation_space)
    {
        std::cout << "ERROR: trying to get null observation space" << std::endl;
        assert(observation_space);
    }
    return observation_space;
}

Environment::~Environment()
{
    //delete[] action;
    delete result;
    delete action_space;
    delete observation_space;
}
