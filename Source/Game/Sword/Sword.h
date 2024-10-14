#pragma once
#include "PrimitiveRenderer.h"

class Sword
{
public:
    Sword() {}
    ~Sword() {}

    void Update(const DirectX::XMMATRIX& start, const DirectX::XMMATRIX& end);
    void Render();
    void DrawDebug();

private:
    void AddVertex(const int& index, const DirectX::XMMATRIX& matrix);

private:
    PrimitiveRenderer primitiveRenderer_;

    DirectX::XMFLOAT3 vertexOffset_[8] =
    {
#if 0
        { -30.0f, 0.0f, 15.0f },
        { 10.0f, 0.0f, 15.0f },
        { -30.0f, -7.0f, 15.0f },
        { 10.0f,  -7.0f, 15.0f },
        { -5.0f, -18.0f, 120.0f },
        { 4.0f, -18.0f, 120.0f },
        { -5.0f, -12.0f, 120.0f },
        { 4.0f, -12.0f, 120.0f },
#else
        { -20.0f, 0.0f, 15.0f },
        { -5.0f, 0.0f, 15.0f },
        { -20.0f, -7.0f, 15.0f },
        { -5.0f,  -7.0f, 15.0f },
        { -8.0f, -18.0f, 120.0f },
        { -2.0f, -18.0f, 120.0f },
        { -8.0f, -12.0f, 120.0f },
        { -2.0f, -12.0f, 120.0f },
#endif
    };

};

