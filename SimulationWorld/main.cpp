//
//  main.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/1.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.hpp"

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "TestClearColor.hpp"
#include "TestTexture2D.hpp"
#include "TestBox2D.hpp"

#include "DebugDraw.hpp"

namespace
{
    bool rightMouseDown = false;
    b2Vec2 lastp;
}

void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static void onResizeWindow(GLFWwindow*, int width, int height)
{
    g_camera.width = width;
    g_camera.height = height;
}

static void onMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mods)
{
    //ImGui_ImplGlfwGL_MouseButtonCallback(window, button, action, mods);

    double xd, yd;
    glfwGetCursorPos(window, &xd, &yd);
    b2Vec2 ps((float32)xd, (float32)yd);

//    // Use the mouse to move things around.
//    if (button == GLFW_MOUSE_BUTTON_1)
//    {
//        //<##>
//        //ps.Set(0, 0);
//        b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
//        if (action == GLFW_PRESS)
//        {
//            if (mods == GLFW_MOD_SHIFT)
//            {
//                test->ShiftMouseDown(pw);
//            }
//            else
//            {
//                test->MouseDown(pw);
//            }
//        }
//
//        if (action == GLFW_RELEASE)
//        {
//            test->MouseUp(pw);
//        }
//    }
//    else
    if (button == GLFW_MOUSE_BUTTON_2)
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

static void onMouseMotion(GLFWwindow*, double xd, double yd)
{
    b2Vec2 ps((float)xd, (float)yd);

    b2Vec2 pw = g_camera.ConvertScreenToWorld(ps);
    //test->MouseMove(pw);
    
    if (rightMouseDown)
    {
        b2Vec2 diff = pw - lastp;
        g_camera.center.x -= diff.x;
        g_camera.center.y -= diff.y;
        lastp = g_camera.ConvertScreenToWorld(ps);
    }
}

static void onMouseScroll(GLFWwindow* window, double dx, double dy)
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

int main(void)
{
    glfwSetErrorCallback(glfwErrorCallback);
    
    g_camera.width = 1024;
    g_camera.height = 640;
    g_camera.zoom = 1.0f;
    g_camera.center.Set(0.0f, 0.0f);
    
    GLFWwindow *window;
    
    // initialize the library
    if ( !glfwInit() )
        return -1;
    
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(g_camera.width, g_camera.height, "Simulator", NULL, NULL);
    
    if ( !window )
    {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);
    
    glfwSetWindowSizeCallback(window, onResizeWindow);
    glfwSetScrollCallback(window, onMouseScroll);
    glfwSetCursorPosCallback(window, onMouseMotion);
    glfwSetMouseButtonCallback(window, onMouseButton);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        printf("Error! glew initialize failed \n");
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    // create box2d global debug draw callback
    g_debugDraw.Create();
    
    {
        // Blending functions
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        
        Renderer renderer;
        
        // ImGui initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        bool show_demo_window = false;
        
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;
        
        testMenu->RegisterTest<test::TestClearColor>("Clear Color");
        testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
        testMenu->RegisterTest<test::TestBox2D>("Box2D");
        
        double time1 = glfwGetTime();
        double time2;
        double frameTime = 0.0;
        double alpha = 0.9f; // alpha for frame time smooth
        
        while( !glfwWindowShouldClose( window ) )
        {
            glClearColor(0.3f, 0.3f, 0.3f, 1.f);
            // todo: clear depth buffer and enable depth test
            renderer.Clear();
            
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            /* begin writing code here */
            
            if (currentTest)
            {
                currentTest->OnUpdate(0.0f);
                currentTest->OnRender();
                ImGui::Begin("Test");
                if (currentTest != testMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->OnImGuiRender();
                ImGui::End();
            }
            
            /* end writing code here */
            
            // Show frame time
            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(ImVec2((float)1024, (float)640));
            ImGui::Begin("Overlay", NULL, ImVec2(0,0), 0.0f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoScrollbar);
            ImGui::SetCursorPos(ImVec2(5, (float)640 - 20));
            ImGui::Text("%.1f ms", 1000.0 * frameTime);
            ImGui::End();

            // Measure speed
            time2 = glfwGetTime();
            frameTime = alpha * frameTime + (1.0 - alpha) * (time2 - time1);
            time1 = time2;
            
            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);
            
            ImGui::Render();
            
            // clear errors send out by imgui
            GLCheckError();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            GLClearError();
            
            // swap front and back buffers
            glfwSwapBuffers(window);
            
            // pool for and process events
            glfwPollEvents();
        }
        delete currentTest;
        if (currentTest != testMenu)
            delete testMenu;
    }
    
    // Cleanup
    g_debugDraw.Destroy();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
