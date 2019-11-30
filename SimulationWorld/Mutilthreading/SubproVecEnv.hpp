//
//  SubproVecEnv.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/27.
//  Copyright © 2019 孙川. All rights reserved.
//

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

#include "Environment.hpp"
#include "ThreadWrapper.hpp"

class VecEnv
{
public:
    VecEnv(int num_envs)
    {
        m_num_envs = num_envs;
    };
    virtual ~VecEnv() {};
    
//    virtual Result Reset() = 0;
//    virtual Result Step(float action[]) = 0;
//    virtual void Close() = 0;
protected:
    int m_num_envs;
};

enum CMD
{
    STEP,
    RESET,
    CLOSE
};

static std::mutex g_m;
static std::condition_variable g_cv;

void push_cmd(std::queue<CMD>& tasks, CMD cmd)
{
    const std::lock_guard<std::mutex> lock(g_m);
    tasks.push(cmd);
    g_cv.notify_all();
}

CMD pop_cmd(std::queue<CMD>& tasks)
{
    std::unique_lock<std::mutex> lk(g_m);
    g_cv.wait(lk, [&tasks]{ return !tasks.empty(); });
    
    CMD cmd = tasks.front();
    tasks.pop();
    return cmd;
}

void worker(std::queue<CMD>& tasks, Environment &env, float data[])
{
    while (true)
    {
        CMD cmd = pop_cmd(tasks);
        if (cmd == CLOSE)
            break;
        switch (cmd) {
            case STEP:
                //env.Step(data);
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
    SubproVecEnv(std::vector<Environment> env_fns);
    ~SubproVecEnv();
    
//    Result Reset() override;
//    Result Step(float action[]) override;
//    void Close() override;
    
private:
    bool waiting;
    bool closed;
    float *data;
    
    std::vector<std::thread> vecOfThreads;
    std::vector<std::queue<CMD>> vecOfTasks;
    std::vector<Environment> vecOfEnvs;
};



#endif /* SubproVecEnv_hpp */
