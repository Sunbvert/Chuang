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

void Renderer::Clear() const {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const {
    shader.Bind();
    va.Bind();
    ib.Bind();
    
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}
