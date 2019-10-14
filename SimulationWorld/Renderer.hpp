//
//  Renderer.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/4.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include <GL/glew.h>

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

void GLClearError();

void GLCheckError();

class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};
