//
//  AsyncVecEnv.hpp
//  Chuang
//
//  Created by 孙川 on 2019/12/4.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef AsyncVecEnv_hpp
#define AsyncVecEnv_hpp

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "VecEnv.hpp"
#include "RobotHopper.hpp"

enum CMD
{
    STEP,
    RESET
};

class CmdObserver
{
    public:
      CmdObserver(std::shared_ptr<RobotHopper> hopper, int id): hopper_(hopper), id_(id)
      {
        std::cout << "observer[" << id_ << "] CTor" << std::endl;
      }
      void update(CMD cmd)
      {
        if (work_.valid() && work_.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
        {
          std::cout << "observer[" << id_ << "] blocking until previous work is finished" << std::endl;
        }
        work_ = std::async(std::launch::async, [this, cmd]() { doTheJob(cmd); });
      }

    private:
      void doTheJob(CMD cmd)
      {
        std::cout << "observer[" << id_ << "] going to execute cmd " << cmd << std::endl;
        switch (cmd)
        {
          case STEP: hopper_->Step(); break;
          case RESET: hopper_->Reset(); break;
          default: std::cout << cmd << std::endl;
        }
      }
      std::shared_ptr<RobotHopper> hopper_;
      // id_ is just for demonstration purposes
      int id_;
      std::future<void> work_;
};

class AsyncVecEnv : public VecEnv
{
public:
    AsyncVecEnv(std::vector<RobotHopper> &env_fns);
    ~AsyncVecEnv();
    AsyncVecEnv(AsyncVecEnv&&) = default;
    
    void Reset(std::vector<Result*>* results) override;
    void Step(float actions[], std::vector<Result*>* results) override;
    void Close() override;
    
    void ImGuiRender();
private:
    std::vector<std::shared_ptr<RobotHopper>> hoppers;
    std::vector<CmdObserver> observers;
    
    bool waiting;
    bool closed;
    Space *action_space;
};

#endif /* AsyncVecEnv_hpp */
