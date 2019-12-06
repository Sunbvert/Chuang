//
//  PlayGround.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/26.
//  Copyright © 2019 孙川. All rights reserved.
//

#pragma once
#ifndef PlayGround_hpp
#define PlayGround_hpp

#include <stdio.h>
#include <Box2D/Box2D.h>

#include "RobotHopper.hpp"

class PlayGround
{
public:
    PlayGround();
    ~PlayGround();
    
    void Render();
    void ImGuiRender();
    void Update();

    void PrepareTest();
    Result* TestStep(float *action);
    Result* TestReset();
    
    void MouseDown(const b2Vec2& p);
    void MouseUp(const b2Vec2& p);
    void ShiftMouseDown(const b2Vec2& p);
    void MouseMove(const b2Vec2& p);
    void Keyboard(int key);
private:
    RobotHopper* m_RobotHopper;
    b2MouseJoint* m_mouseJoint;
    b2World* m_World;
    b2Body* m_groundBody;

    bool enableControl;
};

#endif /* PlayGround_hpp */
