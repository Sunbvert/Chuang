//
//  Test.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/14.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "Test.hpp"
#include "imgui/imgui.h"

namespace test {

    TestMenu::TestMenu(Test*& currentTestPointer)
        : m_CurrentTest(currentTestPointer) {
        
    }

    void TestMenu::OnImGuiRender() {
        for (auto& test : m_Tests) {
            if (ImGui::Button(test.first.c_str()))
                m_CurrentTest = test.second();
        }
    }

}
