#pragma once
#include "Object.h"
#include <string>

class Projectile : public Object
{
public:
    Projectile(const std::string filename, const float& scaleFactor,
        const std::string& name = "", const int& drawType = 0, const int& attackType = 0);
    virtual ~Projectile() {}

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update(const float& elapsedTime) = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug();

    virtual const bool OnHit(const DirectX::XMFLOAT3& hitPosition) = 0;

public:// [Get, Set] Function
    // ----- Damage -----
    [[nodiscard]] const float GetDamage() const { return damage_; }
    void SetDamage(const float& damage) { damage_ = damage; }

    [[nodiscard]] const float GetRadius() const { return radius_; }
    void SetRadius(const float& radius) { radius_ = radius; }

    [[nodiscard]] const float GetCounterRadius() const { return counterRadius_; }
    void SetCounterRadius(const float& radius) { counterRadius_ = radius; }

    [[nodiscard]] const int GetId() const { return id_; }

    [[nodiscard]] const std::string GetName() const { return name_; }

    // ----- Hitƒtƒ‰ƒO -----
    [[nodiscard]] const bool GetIsHit() const { return isHit_; }
    void SetIsHit(const bool& flag) { isHit_ = flag; }

    // ----- •`‰æí—Ş -----
    [[nodiscard]] const int GetDrawType() const { return drawType_; }
    // ----- UŒ‚í—Ş -----
    [[nodiscard]] const int GetAttackType() const { return attackType_; }

private:
    std::string name_   = ""; // “o˜^–¼
    int         id_     = 0;  // “o˜^”Ô†

    float damage_ = 0.0f;

    float radius_ = 0.0f;
    float counterRadius_ = 0.0f; // ƒJƒEƒ“ƒ^[”ÍˆÍ‚Ì”¼Œa

    bool isHit_ = false;

    const int drawType_;    // •`‰æ‚Ìí—Ş
    const int attackType_;  // UŒ‚‚Ì”»’èí—Ş
};

