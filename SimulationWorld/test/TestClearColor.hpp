//
//  TestClearColor.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/14.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include "Test.hpp"

namespace test {

    class TestClearColor : public Test {
    public:
        TestClearColor();
        ~TestClearColor();
        
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
    private:
        float m_ClearColor[4];
    };

}
