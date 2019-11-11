//
//  WindowEventCallback.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "WindowEventCallback.hpp"
#include "DebugDraw.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

test::TestBox2D*  WindowEventCallback::testBox2D = nullptr;
bool WindowEventCallback::rightMouseDown = false;
b2Vec2 WindowEventCallback::lastp(0, 0);
bool WindowEventCallback::box2DSelected = false;


void WindowEventCallback::onResizeWindow(GLFWwindow*, int width, int height)
{
    g_camera.width = width;
    g_camera.height = height;
}

void WindowEventCallback::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool keys_for_ui = ImGui::GetIO().WantCaptureKeyboard;
    if (keys_for_ui)
        return;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            // Quit
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_R:
            // Reset test
//            delete test;
//            test = entry->createFcn();
            break;
        default:
            if (box2DSelected)
            {
                testBox2D->Keyboard(key);
            }
        }
    }
    else if (action == GLFW_RELEASE)
    {
        //test->KeyboardUp(key);
    }
    // else GLFW_REPEAT
}


void WindowEventCallback::onMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mods)
{
    double xd, yd;
    glfwGetCursorPos(window, &xd, &yd);
    b2Vec2 ps((float32)xd, (float32)yd);

    // Use the mouse to move things around.
    if (button == GLFW_MOUSE_BUTTON_1 && box2DSelected)
    {
        b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
        if (action == GLFW_PRESS)
        {
            if (mods == GLFW_MOD_SHIFT)
            {
                testBox2D->ShiftMouseDown(pw);
            }
            else
            {
                testBox2D->MouseDown(pw);
            }
        }

        if (action == GLFW_RELEASE)
        {
            testBox2D->MouseUp(pw);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_2)
    {
        if (action == GLFW_PRESS)
        {
            lastp = g_camera.ConvertScreenToWorld(ps);
            rightMouseDown = true;
        }

        if (action == GLFW_RELEASE)
        {
            rightMouseDown = false;
        }
    }
}

void WindowEventCallback::onMouseMotion(GLFWwindow*, double xd, double yd)
{
    b2Vec2 ps((float)xd, (float)yd);

    b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
    
    if (box2DSelected)
        testBox2D->MouseMove(pw);
    
    if (rightMouseDown)
    {
        b2Vec2 diff = pw - lastp;
        g_camera.center.x -= diff.x;
        g_camera.center.y -= diff.y;
        lastp = g_camera.ConvertScreenToWorld(ps);
    }
}

void WindowEventCallback::onMouseScroll(GLFWwindow* window, double dx, double dy)
{
    bool mouse_for_ui = ImGui::GetIO().WantCaptureMouse;

    if (!mouse_for_ui)
    {
        if (dy > 0)
        {
            g_camera.zoom /= 1.1f;
        }
        else
        {
            g_camera.zoom *= 1.1f;
        }
    }
}
