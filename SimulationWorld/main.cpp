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

int main(void) {
    GLFWwindow *window;
    
    // initialize the library
    if ( !glfwInit() ) {
        return -1;
    }
    
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(960, 540, "Simulator", NULL, NULL);
    
    if ( !window ) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        printf("Error! glew initialize failed \n");
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    std::cout << glGetString(GL_VERSION) << std::endl;
    
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
        bool show_demo_window = true;
        
        test::Test* currentTest = nullptr;
        test::TestMenu* testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;
        
    testMenu->RegisterTest<test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
        
        while( !glfwWindowShouldClose( window ) ) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            renderer.Clear();
            
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            /* begin writing code here */
            
            if (currentTest) {
                currentTest->OnUpdate(0.0f);
                currentTest->OnRender();
                ImGui::Begin("Test");
                if (currentTest != testMenu && ImGui::Button("<-")) {
                    delete currentTest;
                    currentTest = testMenu;
                }
                currentTest->OnImGuiRender();
                ImGui::End();
            }
            
            
            
            
            /* end writing code here */
            
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
