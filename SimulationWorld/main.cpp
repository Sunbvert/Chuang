//
//  main.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/1.
//  Copyright © 2019 孙川. All rights reserved.
//

#include <iostream>
#include <Box2D/Box2D.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(int argc, const char * argv[]) {
    GLFWwindow *window;
    
    // initialize the library
    if ( !glfwInit() ) {
        return -1;
    }
    
    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Simulator", NULL, NULL);
    
    if ( !window ) {
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current
    glfwMakeContextCurrent( window );
    
    // loop until the user closes the widow
    while( !glfwWindowShouldClose( window ) ) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // render the OpenGL, put all the OpenGL code right here
        
        // swap front and back buffers
        glfwSwapBuffers(window);
        
        // pool for and process events
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
