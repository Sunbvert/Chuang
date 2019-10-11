//
//  Renderer.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/4.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "Renderer.hpp"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}
