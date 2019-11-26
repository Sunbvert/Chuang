//
//  RobotHopper.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/26.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef RobotHopper_hpp
#define RobotHopper_hpp

#include <stdio.h>

#include <Box2D/Box2D.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Canvas.hpp"
#include "Environment.hpp"

#include "MqRpc.hpp"
#include "Environment.hpp"

// 机器人身体和模拟环境用户数据
enum WorldBodyType {
    OBSTACLE,
    HEAD,
    THIGH,
    CALF,
    FOOT,
    NONE
};

struct RobotBodyPart
{
    RobotBodyPart () {}

    RobotBodyPart (b2Body *_body, WorldBodyType _type) : body(_body), worldBodyType(_type) {}

    void Set(b2Body *_body, WorldBodyType _type)
    {
        body = _body;
        worldBodyType = _type;
    }

    b2Body *body;
    WorldBodyType worldBodyType;
};

class HeadContactListener : public b2ContactListener
{
public:
    HeadContactListener(bool *_done)
    {
        done = _done;
    }
    void BeginContact(b2Contact* contact)
    {
        b2Fixture *a = contact->GetFixtureA();
        b2Fixture *b = contact->GetFixtureB();
        
        if (a->GetBody()->GetType() == b2_staticBody)
        {
            *done = CheckDone(b);
        }
        if (b->GetBody()->GetType() == b2_staticBody)
        {
            *done = CheckDone(a);
        }
    }
private:
    bool *done;
    bool CheckDone(b2Fixture *fixture)
    {
        if (fixture->GetBody()->GetType() == b2_dynamicBody)
        {
            RobotBodyPart *part = (RobotBodyPart*)fixture->GetBody()->GetUserData();
            if (part->worldBodyType == HEAD)
            {
                return true;
            }
        }
        
        return false;
    }
};

class RobotHopper : Environment
{
public:
    RobotHopper();
    ~RobotHopper();
    
    Result Step(float action[]) override;
    Result Reset() override;
    void Render() override;
    
    void SampleAction(float action[]);
    
    b2World* Getb2WorldPtr();
    b2Body* GetGroundBody();
private:
    void GetObservation(float observation[]);
    void TakeAction(float action[]);
    
    void OnRender();
    void OnImGuiRender();
    void CreateHopperRobot();
    
    float GetVisionScore(b2Fixture *body, b2Fixture* ground);
    float GetReward();
    
    b2World* m_World;
    b2Body* CreateDynamicBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight, float32 desity, float32 friction);
    b2Body* CreateStaticBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight);
    b2Body* m_groundBody;

    bool debugDraw = false;
    bool done = false;

    b2RevoluteJoint* CreateRevoluteJoint(b2Body* bodyA, b2Body* bodyB, b2Vec2 anchor, bool enableLimit, float32 lowerAngle, float32 upperAngle);

    glm::vec2 m_HeadInitialPosition;

    b2Vec2 m_WaistAngleLimit;
    b2Vec2 m_KneeAngleLimit;
    b2Vec2 m_AnkleAngleLimit;

    b2RevoluteJoint* m_WaistJoint;
    b2RevoluteJoint* m_KneeJoint;
    b2RevoluteJoint* m_AnkleJoint;
    
    double WorldBeginTime;
    double WorldRunTime;
    
    HeadContactListener *m_HeadContactListener;

    // 以机器人头部中心为原点，上下左右各一米的范围作为机器人的视觉范围
    // 将该范围内看到的内容储存到 VISION_SIZE 的数组中
    static const int VISION_SIZE = 20;
    static const int FEATURE_SIZE = 3;
    constexpr static const float VISION_LENGTH = 4.0f;
    float32 m_Observation [VISION_SIZE * VISION_SIZE + FEATURE_SIZE];

    RobotBodyPart m_RobotHead;
    RobotBodyPart m_RobotThigh;
    RobotBodyPart m_RobotCalf;
    RobotBodyPart m_RobotFoot;

    Space observation_space;
    Space action_space;
    Canvas *m_Canvas;
};

#endif /* RobotHopper_hpp */
