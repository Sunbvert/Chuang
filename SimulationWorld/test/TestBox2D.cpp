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
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

namespace test {

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

    class VisionQueryCallback : public b2QueryCallback
    {
    public:
        b2Fixture *m_BodyFixture;
        b2Fixture *m_GroundFixture;
        
        VisionQueryCallback()
        {
            m_BodyFixture = nullptr;
            m_GroundFixture = nullptr;
        }
        
        bool ReportFixture(b2Fixture *fixture) override
        {
            b2Body *body = fixture->GetBody();
            if (body->GetType() == b2_dynamicBody && m_BodyFixture == nullptr)
            {
                m_BodyFixture = fixture;
            }
            else if (body->GetType() == b2_staticBody && m_GroundFixture == nullptr)
            {
                m_GroundFixture = fixture;
            }
            
            if (m_BodyFixture != nullptr && m_GroundFixture != nullptr)
            {
                return false;
            }
            
            return true;
        }
    };
    
    TestBox2D::TestBox2D() : m_TestBodyPosition(0, 0), m_HeadInitialPosition(0.0f, 0.0f), m_mouseJoint(nullptr)
    {
        b2Vec2 gravity(0.0f, -10.0f);
        m_World = new b2World(gravity);
        m_World->SetDebugDraw(&g_debugDraw);
        
        b2BodyDef bodyDef;
        m_groundBody = m_World->CreateBody(&bodyDef);
        
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(0.0f, -1.25f);
        b2Body* groundBody = m_World->CreateBody(&groundBodyDef);
        b2PolygonShape groundBox;
        groundBox.SetAsBox(2.5f, 0.25f);
        groundBody->CreateFixture(&groundBox, 0.0f);
        
        b2Vec2 headSize(0.4f, 0.2f);
        b2Vec2 thighSize(0.1f, 0.4f);
        b2Vec2 calfSize(0.1f, 0.3f);
        b2Vec2 footSize(0.24f, 0.1f);
        
        b2Vec2 headCenter(m_HeadInitialPosition.x, m_HeadInitialPosition.y);
        b2Vec2 thighCenter(headCenter.x, headCenter.y - headSize.y / 2 - thighSize.y / 2);
        b2Vec2 calfCenter(headCenter.x, thighCenter.y - thighSize.y / 2 - calfSize.y / 2);
        b2Vec2 footCenter(headCenter.x, calfCenter.y - calfSize.y / 2 - footSize.y / 2);
        
        b2Body* head = CreateDynamicBody(headCenter.x, headCenter.y, headSize.x / 2, headSize.y / 2, 1.0f, 0.3f);
        b2Body* thigh = CreateDynamicBody(thighCenter.x, thighCenter.y, thighSize.x / 2, thighSize.y / 2, 1.0f, 0.3f);
        b2Body* calf = CreateDynamicBody(calfCenter.x, calfCenter.y, calfSize.x / 2, calfSize.y / 2, 1.0f, 0.3f);
        b2Body* foot = CreateDynamicBody(footCenter.x, footCenter.y, footSize.x / 2, footSize.y / 2, 1.0f, 0.3f);
        
        m_WaistAngleLimit.Set(-0.25f * b2_pi, 0.5f * b2_pi);
        m_KneeAngleLimit.Set(-1.0f * b2_pi, 0.1f * b2_pi);
        m_AnkleAngleLimit.Set(-0.25f * b2_pi, 0.25f * b2_pi);
        
        m_WaistJoint = CreateRevoluteJoint(head, thigh, b2Vec2(headCenter.x, headCenter.y - headSize.y / 2), true, m_WaistAngleLimit.x, m_WaistAngleLimit.y);
        m_KneeJoint = CreateRevoluteJoint(thigh, calf, b2Vec2(headCenter.x, thighCenter.y - thighSize.y / 2), true, m_KneeAngleLimit.x, m_KneeAngleLimit.y);
        m_AnkleJoint = CreateRevoluteJoint(calf, foot, b2Vec2(headCenter.x, calfCenter.y - calfSize.y / 2), true, m_AnkleAngleLimit.x, m_AnkleAngleLimit.y);
        
        //m_TestBody = CreateStaticBody(0.0f, 0.0f, 0.5f, 1.5f);
    }

    void TestBox2D::Keyboard(int key)
    {
        switch (key) {
            case GLFW_KEY_A:
                m_KneeJoint->SetMotorSpeed(-0.2f);
                break;
            default:
                break;
        }
    }

    b2RevoluteJoint* TestBox2D::CreateRevoluteJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 anchor, bool enableLimit, float32 lowerAngle, float32 upperAngle)
    {
        b2RevoluteJointDef jointDef;
        jointDef.Initialize(bodyA, bodyB, anchor);
        jointDef.lowerAngle = lowerAngle;
        jointDef.upperAngle = upperAngle;
        jointDef.enableLimit = enableLimit;
        jointDef.motorSpeed = 0.0f;
        jointDef.maxMotorTorque = 1.0f;
        jointDef.enableMotor = true;
        return (b2RevoluteJoint*)m_World->CreateJoint(&jointDef);
    }

    b2Body* TestBox2D::CreateDynamicBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight, float32 desity, float32 friction)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(x, y);
        b2Body* body = m_World->CreateBody(&bodyDef);
        b2PolygonShape bodyBox;
        bodyBox.SetAsBox(halfWidth, halfHeight);
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &bodyBox;
        fixtureDef.density = desity;
        fixtureDef.friction = friction;
        body->CreateFixture(&fixtureDef);
        
        return body;
    }

    b2Body* TestBox2D::CreateStaticBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight)
    {
        b2BodyDef bodyDef;
        bodyDef.position.Set(x, y);
        b2Body* body = m_World->CreateBody(&bodyDef);
        b2PolygonShape box;
        box.SetAsBox(halfWidth, halfHeight);
        body->CreateFixture(&box, 0.0f);
        
        return body;
    }

    void TestBox2D::GetVision()
    {
        float32 vision[VISION_LENGTH][VISION_LENGTH];
        b2AABB aabb;
        
        for (int i = 0; i < VISION_LENGTH; i++)
        {
            for (int j = 0; j < VISION_LENGTH; j++)
            {
                
            }
        }
    }

    void TestBox2D::MouseDown(const b2Vec2& p)
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

    void TestBox2D::ShiftMouseDown(const b2Vec2& p)
    {
        if (m_mouseJoint != NULL)
        {
            return;
        }
    }

    void TestBox2D::MouseUp(const b2Vec2& p)
    {
        if (m_mouseJoint)
        {
            m_World->DestroyJoint(m_mouseJoint);
            m_mouseJoint = NULL;
        }
    }

    void TestBox2D::MouseMove(const b2Vec2& p)
    {
        if (m_mouseJoint)
        {
            m_mouseJoint->SetTarget(p);
        }
    }

    TestBox2D::~TestBox2D()
    {
        delete m_World;
        m_World = nullptr;
    }

    void TestBox2D::OnUpdate(float deltaTime)
    {
        float32 timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 6;
        int32 positionIterations = 2;
        
        // Update
        
        
        m_World->Step(timeStep, velocityIterations, positionIterations);
    }

    void TestBox2D::OnRender()
    {
        uint32 flags = b2Draw::e_shapeBit + b2Draw::e_jointBit;
        g_debugDraw.SetFlags(flags);
        
        // Render stuff
        
        
        
        //g_debugDraw.DrawString(5, 5, "Press 'a' to control the flippers");
        //m_TextLine += 16;
        
        m_World->DrawDebugData();
        g_debugDraw.Flush();
    }

    void TestBox2D::OnImGuiRender()
    {
        if (ImGui::Button("Reset View"))
        {
            g_camera.center.Set(0.0f, 0.0f);
            g_camera.zoom = 1.0f;
        }
        ImGui::SliderFloat2("Test Body Transform", &m_TestBodyPosition.x, -3.0f, 3.0f);
        //m_TestBody->SetTransform(b2Vec2(m_TestBodyPosition.x, m_TestBodyPosition.y), 0.0f);
    }

}

