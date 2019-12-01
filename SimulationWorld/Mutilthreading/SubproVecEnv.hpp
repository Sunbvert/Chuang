//
//  SubproVecEnv.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/27.
//  Copyright © 2019 孙川. All rights reserved.
//

#pragma once
#ifndef SubproVecEnv_hpp
#define SubproVecEnv_hpp

#include <stdio.h>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

#include "RobotHopper.hpp"
#include "ThreadWrapper.hpp"

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
protected:
    int num_envs;
};

enum CMD
{
    STEP,
    RESET,
    CLOSE
};

static std::mutex g_m;
static std::condition_variable g_cv;

static void push_cmd(std::queue<CMD>& tasks, CMD cmd)
{
    const std::lock_guard<std::mutex> lock(g_m);
    tasks.push(cmd);
    g_cv.notify_all();
}

static CMD pop_cmd(std::queue<CMD>& tasks)
{
    std::unique_lock<std::mutex> lk(g_m);
    g_cv.wait(lk, [&tasks]{ return !tasks.empty(); });
    
    CMD cmd = tasks.front();
    tasks.pop();
    return cmd;
}

static void worker(std::queue<CMD> tasks, RobotHopper &env)
{
    while (true)
    {
        CMD cmd = pop_cmd(tasks);
        if (cmd == CLOSE)
            break;
        switch (cmd) {
            case STEP:
                env.Step();
                break;
            case RESET:
                env.Reset();
            default:
                break;
        }
    }
}

class SubproVecEnv : public VecEnv
{
public:
    SubproVecEnv(std::vector<RobotHopper> &env_fns); // TODO: use vector of Environment
    ~SubproVecEnv();
    SubproVecEnv(SubproVecEnv&&) = default;
    
    void Reset(std::vector<Result*>* results) override;
    void Step(float actions[], std::vector<Result*>*) override;
    void Close() override;
    
private:
    bool waiting;
    bool closed;
    Space *action_space;
    
    std::vector<std::thread> vecOfThreads;
    std::vector<std::queue<CMD>> vecOfTasks;
    std::vector<RobotHopper> vecOfEnvs;
};



#endif /* SubproVecEnv_hpp */
