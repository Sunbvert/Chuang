//
//  main.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/1.
//  Copyright © 2019 孙川. All rights reserved.
//

#include <Box2D/Box2D.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"

int main(void) {
    GLFWwindow *window;
    
    // initialize the library
    if ( !glfwInit() ) {
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Simulator", NULL, NULL);
    
    if ( !window ) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent( window );
    glfwSwapInterval(1);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        printf("Error!\n");
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    
    float positions[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    VertexArray va;
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);
    
    Shader shader("../Res/Shader/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
    
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();
    
    //GL check error stack
    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
    
    float r = 0.0f;
    float increment = 0.05f;
    // loop until the user closes the widow
    while( !glfwWindowShouldClose( window ) ) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // render the OpenGL, put all the OpenGL code right here
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        
        va.Bind();
        ib.Bind();
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;
        r += increment;
        
        // swap front and back buffers
        glfwSwapBuffers(window);
        
        // pool for and process events
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
