//
//  ThreadWrapper.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/29.
//  Copyright © 2019 孙川. All rights reserved.
//
//  The code bellow copy from
//  https://thispointer.com/c11-how-to-use-stdthread-as-a-member-variable-in-class/

#include "ThreadWrapper.hpp"

// Parameterized Constructor
ThreadWrapper::ThreadWrapper(std::function<void()> func) : threadHandler(func)
{}
 
// Move Constructor
ThreadWrapper::ThreadWrapper(ThreadWrapper && obj) : threadHandler(std::move(obj.threadHandler))
{
    std::cout << "Move Constructor is called" << std::endl;
}
 
//Move Assignment Operator
ThreadWrapper & ThreadWrapper::operator=(ThreadWrapper && obj)
{
    std::cout << "Move Assignment is called" << std::endl;
    if (threadHandler.joinable())
        threadHandler.join();
    threadHandler = std::move(obj.threadHandler);
    return *this;
}
 
// Destructor
ThreadWrapper::~ThreadWrapper()
{
    if (threadHandler.joinable())
        threadHandler.join();
}
