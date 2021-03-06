//
//  VertexArray.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/9.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include "VertexBuffer.hpp"

class VertexBufferLayout;

class VertexArray {
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    ~VertexArray();
    
    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    
    void Bind() const;
    void Unbind() const;
};
