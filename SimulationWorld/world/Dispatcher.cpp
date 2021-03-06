//
//  Dispatcher.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.
//
//  Here we accept and handle instructions form python client.

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "Dispatcher.hpp"
#include "MqRpc.hpp"
#include "RobotHopper.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Dispatcher::Dispatcher(PlayGround *testHopper)
{
    m_num_envs = 0;
    m_num_actions = 0;
    m_num_observations = 0;
    m_VecEnv = nullptr;
    m_Results = new std::vector<Result*>();
    m_TestHopper = testHopper;

    BeginConnection();
}

Dispatcher::~Dispatcher()
{
    for (auto p : m_Envs)
    {
        delete p;
    }
    m_Envs.clear();
    for (auto p : *m_Results)
    {
        delete p;
    }
    m_Results->clear();
    delete m_Results;
    delete m_VecEnv;
}

json Dispatcher::OnDataRecieve(json &data)
{
    // std::string d = data.dump();
    // std::cout << "Data Recieved: "<< d << std::endl;
    
    std::string instruction = data["call"];

    json j;

    if (data["test"])
    {
        j = HandleTestRender(data);
    }
    else
    {
        m_TestHopper->EndTest();
        if (instruction == "make")
        {
            j = Make(data);
        }
        else if (instruction == "step")
        {
            j = Step(data);
        }
        else if (instruction == "reset")
        {
            j = Reset(data);
        }
    }
    
    // std::string s = j.dump();
    // std::cout << "Data Send: "<< s << std::endl;
    
    return j;
}

json Dispatcher::HandleTestRender(json &data)
{
    std::string instruction = data["call"];

    json response;

    if (instruction == "make")
    {
        m_TestHopper->PrepareTest();
        response["status"] = 1;
    }
    else if (instruction == "step")
    {
        response["response"] = "step";
        
        std::vector<float> actions;
        data["action"].get_to(actions);
        
        assert(actions.size() == m_num_actions);
        float *action = &actions[0];
        Result *result = m_TestHopper->TestStep(action);

        float *p = result->observation;
        std::vector<float> vec(p, p + m_num_observations);

        response["observation"] = vec;
        response["reward"] = result->reward;
        response["done"] = result->done;
        
        return response;
    }
    else if (instruction == "reset")
    {
        m_TestHopper->BeginTest();
        response["response"] = "reset";

        Result *result = m_TestHopper->TestReset();

        float *p = result->observation;
        std::vector<float> vec(p, p + m_num_observations);

        response["observation"] = vec;
        
        return response;
    }

    return response;
}

void Dispatcher::BeginConnection()
{
    MqRpc::EstablishConnection();
    MqRpc::ServerRun(*this);
}

json Dispatcher::Make(json &data)
{
    m_num_envs = data["num_envs"];
    json response;
    response["response"] = "make";
    if (m_VecEnv)
    {
        std::cout << "ERROR: SubVecEnv already exit!" << std::endl;
        response["status"] = 0;
        return response;
    }
    
    for (int i = 0; i < m_num_envs; i++)
    {
        RobotHopper *hopper  = new RobotHopper();
        m_Envs.push_back(hopper);
        Result *result = nullptr;
        m_Results->push_back(result);
    }
    m_VecEnv = new SubproVecEnv(m_Envs);
    
    response["status"] = 1;
    response["observation_space"] = { m_Envs[0]->GetObservationSpace()->X, m_Envs[0]->GetObservationSpace()->Y };
    response["action_space"] = { m_Envs[0]->GetActionSpace()->X, m_Envs[0]->GetActionSpace()->Y };
    
    m_num_actions = m_Envs[0]->GetActionSpace()->X;
    m_num_observations = m_Envs[0]->GetObservationSpace()->X;
    return response;
}

json Dispatcher::Step(json &data)
{
    json response;
    response["response"] = "step";
    
    std::vector<float> actions;
    data["action"].get_to(actions);
    
    assert(actions.size() == m_num_actions * m_num_envs);
    float *action = &actions[0];
    m_VecEnv->Step(action, m_Results);
    
    std::vector<std::vector<float>> observations;
    std::vector<float> rewards;
    std::vector<bool> dones;
    assert(m_num_envs == m_Results->size());
    
    for (int i = 0; i < m_num_envs; i++)
    {
        float *p = (*m_Results)[i]->observation;
        std::vector<float> vec(p, p + m_num_observations);
        observations.push_back(vec);
        rewards.push_back((*m_Results)[i]->reward);
        dones.push_back((*m_Results)[i]->done);
    }
    response["observation"] = observations;
    response["reward"] = rewards;
    response["done"] = dones;
    
    return response;
}

json Dispatcher::Reset(json &data)
{
    json response;
    response["response"] = "reset";
    
    m_VecEnv->Reset(m_Results);
    
    std::vector<std::vector<float>> observations;
    assert(m_num_envs == m_Results->size());
    
    for (int i = 0; i < m_num_envs; i++)
    {
        float *p = (*m_Results)[i]->observation;
        std::vector<float> vec(p, p + m_num_observations);
        observations.push_back(vec);
    }
    response["observation"] = observations;
    return response;
}

void Dispatcher::ImGuiRender()
{
    // if (ImGui::Button("Begin Connection"))
    // {
    //     BeginConnection();
    // }
    // if (ImGui::Button("Test"))
    // {
    //     json data;
    //     data["num_envs"] = 1;
    //     Make(data);
    //     data["action"] = std::array<float, 3>({0.0f, 0.0f, 0.0f});
    //     json j = Step(data);
    //     std::cout << j << std::endl;
    // }
}
