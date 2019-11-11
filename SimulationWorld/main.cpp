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
#include "WindowEventCallback.hpp"

// test
#include "Canvas.hpp"

void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
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
    
    glfwSetWindowSizeCallback(window, WindowEventCallback::onResizeWindow);
    glfwSetScrollCallback(window, WindowEventCallback::onMouseScroll);
    glfwSetCursorPosCallback(window, WindowEventCallback::onMouseMotion);
    glfwSetMouseButtonCallback(window, WindowEventCallback::onMouseButton);
    glfwSetKeyCallback(window, WindowEventCallback::onKey);
    
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
        bool enable_test = true;
        
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
        
        // test
//        Canvas *canvasTest = new Canvas;
//        canvasTest->AddSquare(1, 1.0f, 1.0f, 0.1f, 0.1f);
        
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
                if (typeid(*currentTest) == typeid(test::TestBox2D) && !WindowEventCallback::box2DSelected) {
                    WindowEventCallback::testBox2D = (test::TestBox2D*)currentTest;
                    WindowEventCallback::box2DSelected = true;
                } else if (typeid(*currentTest) != typeid(test::TestBox2D)) {
                    WindowEventCallback::box2DSelected = false;
                }
                
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
            
            // test code
//            canvasTest->OnRender();
            
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
        
       // delete canvasTest;
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
