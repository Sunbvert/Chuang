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

Dispatcher::Dispatcher()
{
    m_num_envs = 0;
    m_num_actions = 0;
    m_num_observations = 0;
    m_VecEnv = nullptr;
    m_Results = new std::vector<Result*>();
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
    std::string d = data.dump();
    std::cout << "Data Recieved: "<< d << std::endl;
    
    std::string instruction = data["instruction"];
    json j;
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
    
    std::string s = j.dump();
    std::cout << "Data Send: "<< s << std::endl;
    
    return j;
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
        std::vector<float> vec(*p, m_num_observations);
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
        std::vector<float> vec(*p, m_num_observations);
        observations.push_back(vec);
    }
    response["observation"] = observations;
    return response;
}

void Dispatcher::ImGuiRender()
{
    if (ImGui::Button("Begin Connection"))
    {
        BeginConnection();
    }
}
