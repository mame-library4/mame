#pragma once
#include "Object.h"

class Projectile : public Object
{
public:
    Projectile(const std::string filename, const float& scaleFactor);
    ~Projectile() {}

    virtual void Initialize()                                   = 0;
    virtual void Finalize()                                     = 0;
    virtual void Update(const float& elapsedTime)               = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr)  = 0;
    virtual void DrawDebug()                                    = 0;
};

