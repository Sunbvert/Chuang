//
//  Canvas.hpp
//  Chuang
//
//  Created by 孙川 on 2019/11/7.
//  Copyright © 2019 孙川. All rights reserved.
//

#ifndef Canvas_hpp
#define Canvas_hpp

#include <stdio.h>
#include <vector>

#include "Renderer.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "DebugDraw.hpp"

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

#include "Graphics.hpp"

struct SquarePaint {
    SquarePaint(glm::vec4 _color, glm::mat4 _model)
    {
        model = _model;
        color = _color;
    }
    glm::mat4 model;
    glm::vec4 color;
};

class Canvas : Graphics {
public:
    Canvas();
    ~Canvas();
    void DrawSquare(float x, float y, float width, float height, glm::vec4 color);
    void SetSquarePosition(int id, float x, float y);
    void removeSquare(int id);
    void SetColor(float r, float g, float b, float a);
    void OnRender() override;
    void Clear();
    
    b2Body *ParentBody;
private:
    std::vector<SquarePaint> m_SquarePaints;
    
    glm::mat4 m_PV;
    glm::vec4 m_Color;
    
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Shader> m_Shader;
};

#endif /* Canvas_hpp */
