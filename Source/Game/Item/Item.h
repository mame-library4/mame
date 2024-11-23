#pragma once
#include "Object.h"
#include <string>

class Item : public Object
{
public:
    Item(const std::string filename, const float& scaleFactor, const std::string& name = "");
    virtual ~Item() {}

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update(const float& elapsedTime) = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug();

    virtual void OnHit() = 0;

public:
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
    [[nodiscard]] const float GetDamageRadius() const { return damageRadius_; }
    [[nodiscard]] const bool GetIsDrawModel() const { return isDrawModel_; }

    [[nodiscard]] const int GetId() const { return id_; }
    [[nodiscard]] const std::string GetName() const { return name_; }

protected:
    DirectX::XMFLOAT3   offsetPosition_ = {};
    float               damageRadius_   = 1.0f;

    float effectScele_ = 0.3f;
    float effectSpeed_ = 3.0f;

    bool isDrawModel_ = true;

    int         id_     = 0;  // ìoò^î‘çÜ
    std::string name_   = ""; // ñºëO
};

