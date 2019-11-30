//
//  ThreadWrapper.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/29.
//  Copyright © 2019 孙川. All rights reserved.
//
//  The code bellow copy from
//  https://thispointer.com/c11-how-to-use-stdthread-as-a-member-variable-in-class/

#ifndef ThreadWrapper_hpp
#define ThreadWrapper_hpp

#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <assert.h>
#include <chrono>
 
/*
 * A class that has thread object as member variable
 */
class ThreadWrapper
{
    // std::thread object
    std::thread  threadHandler;
 
public:
    //Delete the copy constructor
    ThreadWrapper(const ThreadWrapper&) = delete;
 
    //Delete the Assignment opeartor
    ThreadWrapper& operator=(const ThreadWrapper&) = delete;
 
    // Parameterized Constructor
    ThreadWrapper(std::function<void()> func);
 
    // Move Constructor
    ThreadWrapper(ThreadWrapper && obj);
 
    //Move Assignment Operator
    ThreadWrapper & operator=(ThreadWrapper && obj);
 
    //Destructor
    ~ThreadWrapper();
};
#endif /* ThreadWrapper_hpp */
