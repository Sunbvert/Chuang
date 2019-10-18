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
        void OnImGuiRender() override;
        void ShiftMouseDown(const b2Vec2& p);
        void MouseDown(const b2Vec2& p);
        void MouseUp(const b2Vec2& p);
        void MouseMove(const b2Vec2& p);
    private:
        b2World* m_World;
        b2Body* CreateDynamicBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight, float32 desity, float32 friction);
        b2Body* CreateStaticBody(float32 x, float32 y, float32 halfWidth, float32 halfHeight);
        b2Body* m_TestBody;
        
        b2MouseJoint* m_mouseJoint;
        
        glm::mat4 m_Proj, m_View;
        glm::vec3 m_Translation;
        
        glm::vec2 m_TestBodyPosition;
        glm::vec2 m_HeadInitialPosition;
    };

}
