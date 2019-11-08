//
//  VertexBuffer.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/4.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

class VertexBuffer {
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();
    
    void Bind() const;
    void Unbind() const;
};
