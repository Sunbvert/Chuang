//
//  Dispatcher.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef Dispatcher_hpp
#define Dispatcher_hpp

#include "MqConnCallback.hpp"
#include "json.hpp"
#include "SubproVecEnv.hpp"
using json = nlohmann::json;


class Dispatcher : MqConnCallback
{
public:
    Dispatcher();
    ~Dispatcher();
    
    void ImGuiRender();
    
protected:
    json OnDataRecieve(json &data) override;
private:
    void BeginConnection();
    json Make(json &data);
    json Step(json &data);
    json Reset(json &data);
    
    int m_num_envs;
    int m_num_actions;
    int m_num_observations;
    SubproVecEnv *m_VecEnv;
    std::vector<RobotHopper*> m_Envs;
    std::vector<Result*> *m_Results;
};

#endif /* Dispatcher_hpp */
