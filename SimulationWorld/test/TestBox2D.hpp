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

namespace test {

    class TestBox2D : public Test {
    public:
        TestBox2D();
        ~TestBox2D();
        
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
    private:
        b2World* m_World;
        b2Body* m_GroundBody;
        int32 m_TextLine;
    };

}
