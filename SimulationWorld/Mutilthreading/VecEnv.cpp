//
//  VecEnv.cpp
//  Chuang
//
//  Created by 孙川 on 2019/12/4.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "VecEnv.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void VecEnv::ImGuiRender(const char* data[], int num_column, int num_row)
{
    ImGui::Columns(num_column, "VecEnv Debug Info"); // 4-ways, with border
    ImGui::Separator();
    for (int i = 0; i < num_column; i++)
    {
        ImGui::Text(data[i]);
        ImGui::NextColumn();
    }
    ImGui::Separator();

    for (int i = 1; i < num_row; i++)
    {
        for (int j = 0; j < num_column; j++)
        {
            ImGui::Text(data[i * num_column + j]);
            ImGui::NextColumn();
        }
    }
    ImGui::Columns(1);
    ImGui::Separator();
}
