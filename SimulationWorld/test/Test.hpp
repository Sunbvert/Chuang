//
//  test.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/14.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

namespace test {

    class Test {
    public:
        Test() {}
        virtual ~Test() {}
        
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}
    };
}
