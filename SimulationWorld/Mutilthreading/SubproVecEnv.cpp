//
//  SubproVecEnv.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/27.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "SubproVecEnv.hpp"

SubproVecEnv::SubproVecEnv(std::vector<Environment>env_fns) : VecEnv((int)env_fns.size())
{
//    waiting = false;
//    closed = false;
//
//    vecOfEnvs = env_fns;
//
//    for (int i = 0; i < vecOfEnvs.size(); i++)
//    {
//        std::queue<CMD> task;
//        vecOfTasks.push_back(task);
//        vecOfThreads.push_back(std::thread(worker, std::ref(task), std::ref(vecOfEnvs[i])));
//    }
}

SubproVecEnv::~SubproVecEnv()
{
    
}

//Result SubproVecEnv::Step(float *action)
//{
//    
//}
//
//Result SubproVecEnv::Reset()
//{
//    
//}
//
//void SubproVecEnv::Close()
//{
//    
//}
