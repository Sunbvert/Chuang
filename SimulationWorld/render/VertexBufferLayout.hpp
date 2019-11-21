//
//  VertexBufferLayout.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/9.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include <vector>
#include "Renderer.hpp"

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;
    
    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:         return 4;
            case GL_UNSIGNED_INT:  return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

template<typename T>
struct identity { typedef T type; };

class VertexBufferLayout {
public:
    VertexBufferLayout()
        : m_Stride(0) {};
    
    // template<typename T>
    // struct foobar : std::false_type
    // { };
    
    template<typename T>
    void Push(unsigned int count)
    {
//        static_assert(foobar<T>::value, "false value!");
//        assert(false);
        Push(count, identity<T>());
    }

    inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
    inline unsigned int GetStride() const { return m_Stride; }
    
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
    
//    template<typename T>
//    struct foobar : std::false_type
//    { };

    template<typename T>
    void Push(unsigned int, identity<T>)
    {
//        static_assert(foobar<T>::value, "false value!");
        assert(false);
    }

    void Push(unsigned int count, identity<float>) {
        m_Elements.push_back({ GL_FLOAT, count , GL_FALSE });
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
    }
    
    void Push(unsigned int count, identity<unsigned int>) {
        m_Elements.push_back({ GL_UNSIGNED_INT, count , GL_FALSE });
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
    }
    
    void Push(unsigned int count, identity<unsigned char>) {
        m_Elements.push_back({ GL_UNSIGNED_BYTE, count , GL_TRUE });
        m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
    }
};
