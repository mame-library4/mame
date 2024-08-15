#pragma once
#include "Object.h"

class Projectile : public Object
{
public:
    Projectile(const std::string filename, const float& scaleFactor);
    ~Projectile();

    virtual void Initialize()                                   = 0;
    virtual void Finalize()                                     = 0;
    virtual void Update(const float& elapsedTime)               = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr)  = 0;
    virtual void DrawDebug();

public:// [Get, Set] Function
    [[nodiscard]] const float GetRadius() const { return radius_; }
    void SetRadius(const float& radius) { radius_ = radius; }

    [[nodiscard]] const int GetId() const { return id_; }

private:

    float radius_ = 0.0f;

    int id_ = 0; // “o˜^”Ô†

private:
    // “o˜^”Ô†U‚è•ª‚¯—p
    static int idCounter_; 
};

