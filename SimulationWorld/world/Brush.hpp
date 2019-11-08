//
//  Brush.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/7.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef Brush_hpp
#define Brush_hpp

#include <stdio.h>
#include <vector>

#include "Renderer.hpp"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

#include "Graphics.hpp"

class Brush : Graphics {
public:
    Brush();
    ~Brush();
    void DrawSquare(float x, float y, float width, float height);
    void SetColor(float r, float g, float b, float a);
    void OnRender() override;
    void Clear();
private:
    std::vector<glm::mat4> m_Models;
    glm::vec4 m_Color;
    
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Shader> m_Shader;
    
    glm::mat4 m_Proj, m_View;
    glm::vec3 m_Translation;
};

#endif /* Brush_hpp */
