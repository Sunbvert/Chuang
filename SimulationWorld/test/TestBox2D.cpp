//
//  TestBox2D.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/16.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "TestBox2D.hpp"
#include <GL/glew.h>
#include "imgui/imgui.h"
#include "DebugDraw.hpp"

namespace test {
    
    TestBox2D::TestBox2D()
    {
        b2Vec2 gravity(0.0f, -10.0f);
        m_World = new b2World(gravity);
        m_World->SetDebugDraw(&g_debugDraw);
        
        m_TextLine = 30;
        
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(100.0f, 100.0f);
        m_GroundBody = m_World->CreateBody(&groundBodyDef);
        b2PolygonShape groundBox;
        groundBox.SetAsBox(100.0f, 100.0f);
        m_GroundBody->CreateFixture(&groundBox, 0.0f);
    }

    TestBox2D::~TestBox2D()
    {
        delete m_World;
        m_World = nullptr;
    }

    void TestBox2D::OnUpdate(float deltaTime)
    {
        float32 timeStep = 1.0f / 60.0f;
        
        uint32 flags = b2Draw::e_shapeBit;
        g_debugDraw.SetFlags(flags);
        
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        
        m_World->Step(timeStep, velocityIterations, positionIterations);
        m_World->DrawDebugData();
        g_debugDraw.Flush();
        
        g_debugDraw.DrawString(5, 5, "Press 'a' to control the flippers");
        //m_TextLine += 16;
    }

    void TestBox2D::OnImGuiRender()
    {
        
    }

}

