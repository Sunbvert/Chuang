//
//  WindowEventCallback.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/11.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef WindowEventCallback_hpp
#define WindowEventCallback_hpp

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Box2D/Box2D.h>

#include "TestBox2D.hpp"
#include "World.hpp"

class WindowEventCallback
{
public:
    static void onResizeWindow(GLFWwindow*, int width, int height);
    static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onMouseButton(GLFWwindow* window, int32 button, int32 action, int32 mods);
    static void onMouseMotion(GLFWwindow*, double xd, double yd);
    static void onMouseScroll(GLFWwindow* window, double dx, double dy);

    static World *world;
    
private:
    static bool rightMouseDown;
    static b2Vec2 lastp;
};

#endif /* WindowEventCallback_hpp */
