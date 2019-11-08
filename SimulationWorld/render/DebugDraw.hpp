//
//  DebugDraw.hpp
//  Chuang
//
//  Created by 孙川 on 2019/10/16.
//  Copyright © 2019 孙川. All rights reserved.
//
#pragma once

#include <Box2D/Box2D.h>

struct b2AABB;
struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;

//
struct Camera
{
    Camera()
    {
        center.Set(0.0f, 20.0f);
        zoom = 1.0f;
        width = 1280;
        height = 800;
    }

    b2Vec2 ConvertScreenToWorld(const b2Vec2& screenPoint);
    b2Vec2 ConvertWorldToScreen(const b2Vec2& worldPoint);
    void BuildProjectionMatrix(float32* m, float32 zBias);

    b2Vec2 center;
    float32 zoom;
    int32 width;
    int32 height;
};

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw
{
public:
    DebugDraw();
    ~DebugDraw();

    void Create();
    void Destroy();

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    void DrawTransform(const b2Transform& xf) override;

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

    void DrawString(int x, int y, const char* string, ...);

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

    void Flush();

private:
    GLRenderPoints* m_points;
    GLRenderLines* m_lines;
    GLRenderTriangles* m_triangles;
};

extern DebugDraw g_debugDraw;
extern Camera g_camera;

