#pragma once
#include "../Object.h"

class Stage : public Object
{
public:
    Stage(std::string filename);
    virtual ~Stage() {}

    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug()                        = 0;
};

