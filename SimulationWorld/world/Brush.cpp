//
//  Brush.cpp
//  Chuang
//
//  Created by 孙川 on 2019/11/7.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "Brush.hpp"

Brush::Brush() : m_Translation(0, 0, 0),
    m_Proj(glm::ortho(0.0f, 1024.0f, 0.0f, 640.0f, -1.0f, 1.0f)),
    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
{
    
    float positions[] = {
       200.0f, 200.0f,
       600.0f, 200.0f,
       600.0f, 400.0f,
       200.0f, 400.0f
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
    m_Shader->SetUniform4f("u_Color", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
}

Brush::~Brush()
{
    
}

void Brush::DrawSquare(float x, float y, float width, float height)
{
    
}

void Brush::SetColor(float r, float g, float b, float a)
{
    m_Color = glm::vec4(r, g, b, a);
}

void Brush::OnRender()
{
    Renderer renderer;
    
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
    
    for (int i = 0; i < m_Models.size(); i++)
    {
        glm::mat4 mvp = m_Proj * m_View * m_Models[i];
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }
}
