//
//  VecEnv.hpp
//  Chuang
//
//  Created by 孙川 on 2019/12/4.
//  Copyright © 2019 孙川. All rights reserved.
//

#pragma once
#ifndef VecEnv_hpp
#define VecEnv_hpp

#include <vector>

#include "Environment.hpp"

class VecEnv
{
public:
    VecEnv(int _num_envs)
    {
        num_envs = _num_envs;
    };
    virtual ~VecEnv() {};
    
    virtual void Reset(std::vector<Result*>* results) = 0;
    virtual void Step(float actions[], std::vector<Result*>* results) = 0;
    virtual void Close() = 0;
    
    void ImGuiRender(const char* data[], int num_column, int num_row);
protected:
    int num_envs;
};

#endif /* VecEnv_hpp */
