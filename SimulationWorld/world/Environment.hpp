//
//  Environment.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/17.
//  Copyright © 2019 孙川. All rights reserved.
//

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

struct Action
{
    float *action = nullptr;
};

class Environment
{
public:
    Environment() {}
    virtual ~Environment() {}
    virtual Result Step(Action &action) = 0;
    virtual Result Reset() = 0;
    virtual void Render() = 0;
};

#endif /* Environment_hpp */
