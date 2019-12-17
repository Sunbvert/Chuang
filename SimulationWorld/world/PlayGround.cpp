//
//  PlayGround.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/26.
//  Copyright © 2019 孙川. All rights reserved.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "PlayGround.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class QueryCallback : public b2QueryCallback
{
public:
    QueryCallback(const b2Vec2& point)
    {
        m_point = point;
        m_fixture = NULL;
    }

    bool ReportFixture(b2Fixture* fixture) override
    {
        b2Body* body = fixture->GetBody();
        if (body->GetType() == b2_dynamicBody)
        {
            bool inside = fixture->TestPoint(m_point);
            if (inside)
            {
                m_fixture = fixture;

                // We are done, terminate the query.
                return false;
            }
        }

        // Continue the query.
        return true;
    }

    b2Vec2 m_point;
    b2Fixture* m_fixture;
};

PlayGround::PlayGround() : m_mouseJoint(nullptr)
{
    m_RobotHopper = new RobotHopper();
    m_RobotHopper->EnableRender();
    m_World = m_RobotHopper->Getb2WorldPtr();
    m_groundBody = m_RobotHopper->GetGroundBody();

    enableControl = true;
}

PlayGround::~PlayGround()
{
    delete m_RobotHopper;
}

void PlayGround::Update()
{
    if (enableControl)
    {
        float action[3] = {0.0f, 0.0f, 0.0f};
        //m_RobotHopper->SampleAction(action);  //use this when need random actions
        m_RobotHopper->SetAction(action);
        m_RobotHopper->Step();
    }
}

void PlayGround::Render()
{
    m_RobotHopper->Render();
}

void PlayGround::ImGuiRender()
{
    if (enableControl)
    {
        ImGui::Text("Control enabled");
    }
    if (ImGui::Button("Reset View"))
    {
        g_camera.center.Set(0.0f, 0.0f);
        g_camera.zoom = 1.0f;
    }

    if (ImGui::Button("Reset World"))
    {
        m_RobotHopper->Reset();
    }
}

void PlayGround::PrepareTest()
{
    enableControl = false;
    m_RobotHopper->Reset();
}

void PlayGround::BeginTest()
{
    enableControl = false;
}

void PlayGround::EndTest()
{
    enableControl = true;
}

Result* PlayGround::TestStep(float *action)
{
    m_RobotHopper->SetAction(action);
    m_RobotHopper->Step();
    return m_RobotHopper->GetResult();
}

void PlayGround::SampleAction(float *action)
{
    m_RobotHopper->SampleAction(action);
}

Result* PlayGround::TestReset()
{
    m_RobotHopper->Reset();
    return m_RobotHopper->GetResult();
}

void PlayGround::Keyboard(int key)
{
    if (enableControl)
    {
        switch (key) {
            case GLFW_KEY_A:
                //m_KneeJoint->SetMotorSpeed(-0.2f);
                break;
            default:
                break;
        }
    }
}

void PlayGround::MouseDown(const b2Vec2& p)
{
    if (enableControl)
    {
        if (m_mouseJoint != NULL)
        {
            return;
        }

        // Make a small box.
        b2AABB aabb;
        b2Vec2 d;
        d.Set(0.001f, 0.001f);
        aabb.lowerBound = p - d;
        aabb.upperBound = p + d;

        // Query the world for overlapping shapes.
        QueryCallback callback(p);
        m_World->QueryAABB(&callback, aabb);

        if (callback.m_fixture)
        {
            b2Body* body = callback.m_fixture->GetBody();
            b2MouseJointDef md;
            md.bodyA = m_groundBody;
            md.bodyB = body;
            md.target = p;
            md.maxForce = 1000.0f * body->GetMass();
            m_mouseJoint = (b2MouseJoint*)m_World->CreateJoint(&md);
            body->SetAwake(true);
        }
    }
}

void PlayGround::ShiftMouseDown(const b2Vec2& p)
{
    if (enableControl)
    {
        if (m_mouseJoint != NULL)
        {
            return;
        }
    }
}

void PlayGround::MouseUp(const b2Vec2& p)
{
    if (enableControl)
    {
        if (m_mouseJoint)
        {
            m_World->DestroyJoint(m_mouseJoint);
            m_mouseJoint = NULL;
        }
    }
}

void PlayGround::MouseMove(const b2Vec2& p)
{
    if (enableControl)
    {
        if (m_mouseJoint)
        {
            m_mouseJoint->SetTarget(p);
        }
    }
}
