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

    virtual void OnHit() = 0;

public:// [Get, Set] Function
    // ----- Damage -----
    [[nodiscard]] const float GetDamage() const { return damage_; }
    void SetDamage(const float& damage) { damage_ = damage; }
    
    [[nodiscard]] const float GetRadius() const { return radius_; }
    void SetRadius(const float& radius) { radius_ = radius; }

    [[nodiscard]] const float GetCounterRadius() const { return counterRadius_; }
    void SetCounterRadius(const float& radius) { counterRadius_ = radius; }

    [[nodiscard]] const int GetId() const { return id_; }

private:

    float damage_ = 0.0f;

    float radius_ = 0.0f;
    float counterRadius_ = 0.0f; // ƒJƒEƒ“ƒ^[”ÍˆÍ‚Ì”¼Œa

    int id_ = 0; // “o˜^”Ô†

private:
    // “o˜^”Ô†U‚è•ª‚¯—p
    static int idCounter_; 
};

