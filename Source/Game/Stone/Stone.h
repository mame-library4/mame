#pragma once
#include "Object.h"

class Stone : public Object
{
public:
    Stone();
    ~Stone();

    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader = nullptr);
    void DrawDebug();
};

