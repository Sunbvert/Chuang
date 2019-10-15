//
//  TestMvp.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/14.
//  Copyright © 2019 孙川. All rights reserved.
//

#pragma once

#include "Test.hpp"

namespace test {
    
    class TestMvp : Test {
    public:
        TestMvp();
        ~TestMvp();
        
        void OnRender() override;
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
    private:
        float positions[16];
        unsigned int indices[6];
    };
}
