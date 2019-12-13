//
//  RobotHopper.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/26.
//  Copyright © 2019 孙川. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "RobotHopper.hpp"
#include "imgui/imgui.h"
#include "DebugDraw.hpp"
#include "glm/glm.hpp"

#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

class VisionQueryCallback : public b2QueryCallback
{
public:
    b2Fixture *m_BodyFixture;
    b2Fixture *m_GroundFixture;
    b2AABB *m_AABB;

    VisionQueryCallback(b2AABB *aabb)
    {
        m_BodyFixture = nullptr;
        m_GroundFixture = nullptr;
        m_AABB = aabb;
    }

    bool ReportFixture(b2Fixture *fixture) override
    {
        b2Body *body = fixture->GetBody();

        if (body->GetType() == b2_dynamicBody && m_BodyFixture == nullptr)
        {
            if (CollidWithFixture(fixture, body))
            {
                m_BodyFixture = fixture;
            }
        }
        else if (body->GetType() == b2_staticBody && m_GroundFixture == nullptr)
        {
            if (CollidWithFixture(fixture, body))
            {
                m_GroundFixture = fixture;
            }
        }

        if (m_BodyFixture != nullptr && m_GroundFixture != nullptr)
        {
            return false;
        }

        return true;
    }

    c2v b2vToc2v(b2Vec2 b2)
    {
        c2v c2;
        c2.x = b2.x;
        c2.y = b2.y;
        return c2;
    }

    c2r b2rToc2r(b2Rot b2)
    {
        c2r c2;
        c2.c = b2.c;
        c2.s = b2.s;
        return c2;
    }

    void copyArrayToc2v(c2v c2vArray[], b2Vec2 b2vArray[], int count)
    {
        for (int i = 0; i < count; i++)
        {
            c2vArray[i] = b2vToc2v(b2vArray[i]);
        }
    }

    bool CollidWithFixture(b2Fixture *fixture, b2Body *body)
    {
        if (fixture->GetShape()->GetType() == b2Shape::e_polygon)
        {
            b2Transform transform = body->GetTransform();
            b2PolygonShape *shape = (b2PolygonShape*)fixture->GetShape();
            c2AABB a;
            a.min = b2vToc2v(m_AABB->lowerBound);
            a.max = b2vToc2v(m_AABB->upperBound);
            c2Poly b;
            b.count = shape->m_count;
            copyArrayToc2v(b.verts, shape->m_vertices, b.count);
            copyArrayToc2v(b.norms, shape->m_normals, b.count);
            c2x x;
            x.p = b2vToc2v(transform.p);
            x.r = b2rToc2r(transform.q);
            if(c2AABBtoPoly(a, &b, &x))
            {
                return true;
            }
        }

        return false;
    }
};


RobotHopper::RobotHopper() : m_HeadInitialPosition(0.0f, -0.08f)
{
    enableRender = false;
    m_Canvas = nullptr;
    m_unmoveStepCount = 0;
    m_lastHeadX = m_HeadInitialPosition.x;
    
    b2Vec2 gravity(0.0f, -10.0f);
    m_World = new b2World(gravity);
    
    m_HeadContactListener = new HeadContactListener(&done);
    m_World->SetContactListener(m_HeadContactListener);
    
    b2BodyDef bodyDef;
    m_groundBody = m_World->CreateBody(&bodyDef);

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(90.0f, -1.25f);
    b2Body* groundBody = m_World->CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(100.0f, 0.25f);
    groundBody->CreateFixture(&groundBox, 0.0f);
    
    CreateHopperRobot();

    SetObservationSpace(VISION_SIZE * VISION_SIZE + FEATURE_SIZE, 1);
    SetActionSpace(FEATURE_SIZE, 1);
    
    done = false;
    WorldRunTime = 0;
    WorldBeginTime = glfwGetTime();
}

void RobotHopper::EnableRender()
{
    enableRender = true;
    m_World->SetDebugDraw(&g_debugDraw);
    m_Canvas = new Canvas();
    uint32 flags = b2Draw::e_shapeBit + b2Draw::e_jointBit;
    g_debugDraw.SetFlags(flags);
}

RobotHopper::~RobotHopper()
{
    delete m_World;
    delete m_HeadContactListener;
}

void RobotHopper::Step()
{
    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    // Update
    float *action = GetAction();
    TakeAction(action);
    
    m_World->Step(1.0f / FPS, velocityIterations * 30.0f, positionIterations * 30.0f);
    
    GetObservation(m_Observation);
    float reward = GetReward();

    WorldRunTime = glfwGetTime() - WorldBeginTime;
    if (WorldRunTime >= 4 && m_RobotHead.body->GetWorldCenter().x <= m_HeadInitialPosition.x)
    {
        done = true;
    }

    // Stop robot from staying still too long
    if (m_RobotHead.body->GetWorldCenter().x == m_lastHeadX)
    {
        m_unmoveStepCount++;
    }
    else 
    {
        m_unmoveStepCount = 0;
    }
    m_lastHeadX = m_RobotHead.body->GetWorldCenter().x;

    if (m_unmoveStepCount >= 20)
    {
        done = true;
    }

    if (done)
    {
        reward -= 200;
    }
    
    std::map<std::string, std::string> b;
    Result *result = new Result(m_Observation, reward, done, b);
    SetResult(result);
    steped = true;
}

void RobotHopper::SampleAction(float action[])
{
    
    for (int i = 0; i < FEATURE_SIZE; i++)
    {
        /* generate secret number between 0.0 and 1.0 : */
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        action[i] = r * 2.0f - 1.0f;
    }
}

void RobotHopper::TakeAction(float action[])
{
    m_WaistJoint->SetMotorSpeed(action[0] * 2.0f);
    m_KneeJoint->SetMotorSpeed(action[1] * 2.0f);
    m_AnkleJoint->SetMotorSpeed(action[2] * 2.0f);

    // std::cout << "0: " << action[0] << " 1: " << action[1] << " 2: " << action[2] << std::endl;
}

void RobotHopper::Render()
{
    if (enableRender)
        OnRender();
}

void RobotHopper::Reset()
{
    // Delete
    m_World->DestroyBody(m_RobotHead.body);
    m_World->DestroyBody(m_RobotThigh.body);
    m_World->DestroyBody(m_RobotCalf.body);
    m_World->DestroyBody(m_RobotFoot.body);
    
    delete m_HeadContactListener;
    m_HeadContactListener = nullptr;
    
    // Regernate
    done = false;
    WorldRunTime = 0;
    WorldBeginTime = glfwGetTime();  // Seconds
    
    CreateHopperRobot();
    
    m_HeadContactListener = new HeadContactListener(&done);
    m_World->SetContactListener(m_HeadContactListener);
    
    GetObservation(m_Observation);
    std::map<std::string, std::string> b;
    Result *result = new Result(m_Observation, 0.0f, done, b);
    SetResult(result);
    steped = true;
    m_unmoveStepCount = 0;
    m_lastHeadX = 0.0f;
}

void RobotHopper::OnImGuiRender()
{
    if (done)
    {
        ImGui::Text("Simulation Done!");
    }
    ImGui::Text("Current unmoved step count: %d", m_unmoveStepCount);
}

void RobotHopper::OnRender()
{
    // Render stuff

    m_Canvas->OnRender();
    OnImGuiRender();

    m_World->DrawDebugData();
    g_debugDraw.Flush();
}

b2RevoluteJoint* RobotHopper::CreateRevoluteJoint(b2Body *bodyA, b2Body *bodyB, b2Vec2 anchor, bool enableLimit, float32 lowerAngle, float32 upperAngle)
{
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(bodyA, bodyB, anchor);
    jointDef.lowerAngle = lowerAngle;
    jointDef.upperAngle = upperAngle;
    jointDef.enableLimit = enableLimit;
    jointDef.motorSpeed = 0.0f;
    jointDef.maxMotorTorque = 80.0f;
    jointDef.enableMotor = true;
    return (b2RevoluteJoint*)m_World->CreateJoint(&jointDef);
}

b2Body* RobotHopper::CreateDynamicBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight, float32 desity, float32 friction)
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

b2Body* RobotHopper::CreateStaticBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x, y);
    b2Body* body = m_World->CreateBody(&bodyDef);
    b2PolygonShape box;
    box.SetAsBox(halfWidth, halfHeight);
    body->CreateFixture(&box, 0.0f);

    return body;
}

void RobotHopper::GetObservation(float observation[])
{
    b2Vec2 visionCenter = m_RobotHead.body->GetWorldCenter();
    b2AABB aabb;

    float cellLength = VISION_LENGTH / VISION_SIZE;
    b2Vec2 visionLength(VISION_LENGTH, VISION_LENGTH);

    //glm::vec4 color(0.8f, 0.8f, 0.8f, 1.0f);

    //b2Vec2 center = aabb.GetCenter();
    if (enableRender)
        m_Canvas->Clear();
//    m_Canvas->DrawSquare(-1.0, 1.0, cellLength, cellLength, color);

    for (int i = 0; i < VISION_SIZE; i++)
    {
        for (int j = 0; j < VISION_SIZE; j++)
        {
            aabb.upperBound = b2Vec2(visionCenter.x - VISION_LENGTH / 2 + j * cellLength, visionCenter.y + VISION_LENGTH / 2 - i * cellLength);
            aabb.lowerBound = aabb.upperBound + b2Vec2(cellLength, -cellLength);

            VisionQueryCallback callback(&aabb);
            m_World->QueryAABB(&callback, aabb);

            float score = GetVisionScore(callback.m_BodyFixture, callback.m_GroundFixture);
        
            observation[j + i * VISION_SIZE] = score;
            
            b2Vec2 center = aabb.GetCenter();
            if (enableRender)
                m_Canvas->DrawSquare(center.x, center.y, cellLength, cellLength, glm::vec4(score, 0.0f, 0.0f, 1.0f));
        }
    }
    
    int featureStart = VISION_SIZE * VISION_SIZE;
    observation[featureStart] = m_WaistJoint->GetJointSpeed();
    observation[featureStart + 1] = m_KneeJoint->GetJointSpeed();
    observation[featureStart + 2] = m_AnkleJoint->GetJointSpeed();
}

float RobotHopper::GetReward()
{
    b2Vec2 position = m_RobotHead.body->GetPosition();
    // WorldRunTime = glfwGetTime() - WorldBeginTime;
    float reward = position.x * 10; //  - (float)WorldRunTime;
    
    //std::cout << reward << std::endl;
    return reward;
}

float RobotHopper::GetVisionScore(b2Fixture *body, b2Fixture *ground)
{
    float score = 0;
    if (body)
    {
        RobotBodyPart *part = (RobotBodyPart*)body->GetBody()->GetUserData();
        switch (part->worldBodyType) {
            case HEAD:
                score = 0.1f;
                break;
            case THIGH:
                score = 0.2f;
                break;
            case CALF:
                score = 0.3f;
                break;
            case FOOT:
                score = 0.4f;
                break;
            default:
                break;
        }
    }
    if (ground)
    {
        score += 0.5f;
    }
    return score;
}

void RobotHopper::CreateHopperRobot()
{
    b2Vec2 headSize(0.4f, 0.2f);
    b2Vec2 thighSize(0.1f, 0.4f);
    b2Vec2 calfSize(0.1f, 0.3f);
    b2Vec2 footSize(0.24f, 0.1f);

    b2Vec2 headCenter(m_HeadInitialPosition.x, m_HeadInitialPosition.y);
    b2Vec2 thighCenter(headCenter.x, headCenter.y - headSize.y / 2 - thighSize.y / 2);
    b2Vec2 calfCenter(headCenter.x, thighCenter.y - thighSize.y / 2 - calfSize.y / 2);
    b2Vec2 footCenter(headCenter.x, calfCenter.y - calfSize.y / 2 - footSize.y / 2);

    float density = 300.0f;

    b2Body* head = CreateDynamicBody(headCenter.x, headCenter.y, headSize.x / 2, headSize.y / 2, density, 0.3f);
    b2Body* thigh = CreateDynamicBody(thighCenter.x, thighCenter.y, thighSize.x / 2, thighSize.y / 2, density, 0.3f);
    b2Body* calf = CreateDynamicBody(calfCenter.x, calfCenter.y, calfSize.x / 2, calfSize.y / 2, density, 0.3f);
    b2Body* foot = CreateDynamicBody(footCenter.x, footCenter.y, footSize.x / 2, footSize.y / 2, density, 0.3f);

    m_RobotHead.Set(head, HEAD);
    m_RobotThigh.Set(thigh, THIGH);
    m_RobotCalf.Set(calf, CALF);
    m_RobotFoot.Set(foot, FOOT);

    head->SetUserData(&m_RobotHead);
    thigh->SetUserData(&m_RobotThigh);
    calf->SetUserData(&m_RobotCalf);
    foot->SetUserData(&m_RobotFoot);

    m_WaistAngleLimit.Set(-0.25f * b2_pi, 0.25f * b2_pi);
    m_KneeAngleLimit.Set(-1.0f * b2_pi, 0.1f * b2_pi);
    m_AnkleAngleLimit.Set(-0.25f * b2_pi, 0.25f * b2_pi);

    m_WaistJoint = CreateRevoluteJoint(head, thigh, b2Vec2(headCenter.x, headCenter.y - headSize.y / 2), true, m_WaistAngleLimit.x, m_WaistAngleLimit.y);
    m_KneeJoint = CreateRevoluteJoint(thigh, calf, b2Vec2(headCenter.x, thighCenter.y - thighSize.y / 2), true, m_KneeAngleLimit.x, m_KneeAngleLimit.y);
    m_AnkleJoint = CreateRevoluteJoint(calf, foot, b2Vec2(headCenter.x, calfCenter.y - calfSize.y / 2), true, m_AnkleAngleLimit.x, m_AnkleAngleLimit.y);

    // Set robot body mass
    
    float totalMass = head->GetMass() + thigh->GetMass() + calf->GetMass()  + foot->GetMass();
    std::cout << "total robot mass: " << totalMass << std::endl;
}

b2World* RobotHopper::Getb2WorldPtr()
{
    return m_World;
}

b2Body* RobotHopper::GetGroundBody()
{
    return m_groundBody;
}
