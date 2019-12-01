//
//  Environment.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/17.
//  Copyright © 2019 孙川. All rights reserved.
//

#pragma once
#ifndef Environment_hpp
#define Environment_hpp

#include <stdio.h>
#include <map>
#include <string>

struct Result
{
    float *observation = nullptr;
    float reward;
    bool done;
    std::map<std::string, std::string> info;
    
    Result(float _observation[], float _reward, bool _done, std::map<std::string, std::string> _info)
    {
        observation = _observation;
        reward = _reward;
        done = _done;
        info = _info;
    }
};

struct Space
{
    int X;
    int Y;
    void Set(int x, int y)
    {
        X = x;
        Y = y;
    }
};

class Environment
{
public:
    Environment() : action(nullptr), result(nullptr), observation_space(nullptr), action_space(nullptr), steped(false) {}
    ~Environment();
    virtual void Step() = 0;
    virtual void Reset() = 0;
    virtual void Render() = 0;
    void SetAction(float action[]);
    float* GetAction();
    void SetResult(Result* _result);
    Result* GetResult();
    void SetActionSpace(int x, int y);
    Space* GetActionSpace();
    void SetObservationSpace(int x, int y);
    Space* GetObservationSpace();
    
    bool steped;
private:
    float *action;
    Result *result;
    Space *observation_space;
    Space *action_space;
};

#endif /* Environment_hpp */
