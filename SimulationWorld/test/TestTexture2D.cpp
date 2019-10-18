//
//  TestTesture.cpp
//  Chuang
//
//  Created by 孙川 on 2019/10/15.
//  Copyright © 2019 孙川. All rights reserved.
//

#include "TestTexture2D.hpp"

#include "Renderer.hpp"
#include "imgui/imgui.h"
#include "glm/glm.hpp"

namespace test {

    TestTexture2D::TestTexture2D()
        : m_Translation(0, 0, 0),
        m_Proj(glm::ortho(0.0f, 1024.0f, 0.0f, 640.0f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))) {
        
        float positions[] = {
           200.0f, 200.0f, 0.0f, 0.0f,
           600.0f, 200.0f, 1.0f, 0.0f,
           600.0f, 400.0f, 1.0f, 1.0f,
           200.0f, 400.0f, 0.0f, 1.0f
        };
        
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        
        // Need to reset Blend function later
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
            
        m_VAO = std::make_unique<VertexArray>();
        m_VertexBuffer =  std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
            
        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);
        
        m_Shader = std::make_unique<Shader>("../Res/Shader/Basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
        m_Texture = std::make_unique<Texture>("../Res/Texture/314.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestTexture2D::~TestTexture2D() {
        
    }

    void TestTexture2D::OnUpdate(float deltaTime) {
        
    }

    void TestTexture2D::OnRender() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        Renderer renderer;
        
        m_Texture->Bind();
        
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void TestTexture2D::OnImGuiRender() {
        ImGui::SliderFloat3("Translation", &m_Translation.x, 0.0f, 960.0f);
    }


}
