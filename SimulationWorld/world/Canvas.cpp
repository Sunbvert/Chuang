//
//  Canvas.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/7.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "Canvas.hpp"
#include "DebugDraw.hpp"

Canvas::Canvas()
{
    m_PV = g_camera.BuildProjectionMatrix(0.0);
    ParentBody = nullptr;
    
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
    
    // Need to reset Blend function later
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
        
    m_VAO = std::make_unique<VertexArray>();
    m_VertexBuffer =  std::make_unique<VertexBuffer>(positions, 2 * 4 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
        
    m_VAO->AddBuffer(*m_VertexBuffer, layout);
    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
    
    m_Shader = std::make_unique<Shader>("../Res/Shader/BasicColor.shader");
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", glm::vec4(0.8f, 0.3f, 0.8f, 0.5f));
}

Canvas::~Canvas()
{
    
}

void Canvas::DrawSquare(float x, float y, float width, float height, glm::vec4 color)
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    //因为原向量的宽高已经是单位向量，所以这里不用计算比例
    glm::mat4 scaling = glm::scale(glm::vec3(width, height, 1.0f));
    glm::mat4 model = translation * scaling;
    
    SquarePaint paint(color, model);
    m_SquarePaints.push_back(paint);
}

void Canvas::Clear()
{
    m_SquarePaints.clear();
}

void Canvas::OnRender()
{
    Renderer renderer;
    m_Shader->Bind();
    
    for (int i = 0; i < m_SquarePaints.size(); i++)
    {
        glm::mat4 mvp = m_PV * m_SquarePaints[i].model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        m_Shader->SetUniform4f("u_Color", m_SquarePaints[i].color);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
}
