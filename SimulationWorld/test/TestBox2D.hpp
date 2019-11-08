//
//  TestBox2D.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/16.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include "Test.hpp"
#include <Box2D/Box2D.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

    class TestBox2D : public Test {
    public:
        TestBox2D();
        ~TestBox2D();
        
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        void MouseDown(const b2Vec2& p);
        void MouseUp(const b2Vec2& p);
        void ShiftMouseDown(const b2Vec2& p);
        void MouseMove(const b2Vec2& p);
        void Keyboard(int key);
        void GetVision();
    private:
        b2World* m_World;
        b2Body* CreateDynamicBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight, float32 desity, float32 friction);
        b2Body* CreateStaticBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight);
        b2Body* m_TestBody;
        b2Body* m_groundBody;
        
        bool debugDraw = false;
        
        b2RevoluteJoint* CreateRevoluteJoint(b2Body* bodyA, b2Body* bodyB, b2Vec2 anchor, bool enableLimit, float32 lowerAngle, float32 upperAngle);
        
        b2MouseJoint* m_mouseJoint;
        
        glm::mat4 m_Proj, m_View;
        glm::vec3 m_Translation;
        
        glm::vec2 m_TestBodyPosition;
        glm::vec2 m_HeadInitialPosition;
        
        b2Vec2 m_WaistAngleLimit;
        b2Vec2 m_KneeAngleLimit;
        b2Vec2 m_AnkleAngleLimit;
        
        b2RevoluteJoint* m_WaistJoint;
        b2RevoluteJoint* m_KneeJoint;
        b2RevoluteJoint* m_AnkleJoint;
        
        //以机器人头部中心为原点，上下左右各一米的范围作为机器人的视觉范围
        //将该范围内看到的内容储存到 10 * 10 的数组中
        static const int VISION_LENGTH = 10;
        float32 m_Vision [VISION_LENGTH][VISION_LENGTH];
        
        //机器人身体和模拟环境用户数据
        // TODO: 减少内存支出
        static const int USER_DATA_OBSTACLE = 1;
        static const int USER_DATA_HEAD = 0;
        static const int USER_DATA_THIGH = 3;
        static const int USER_DATA_CALF = 5;
        static const int USER_DATA_FOOT = 7;
        
    };

}
