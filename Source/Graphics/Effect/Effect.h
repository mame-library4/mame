#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>

// エフェクト
class Effect
{
public:
    Effect(const char* filename, const std::string& effectName);
    ~Effect() {};

    Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const float& scale = 1.0f, const float& speed = 1.0f);

    void DrawDebug();

public:
    [[nodiscard]] const std::string GetName() const { return name_; }

private:
    Effekseer::EffectRef effekseerEffect_;

    DirectX::XMFLOAT3   position_   = { 0.0f, 0.0f, 0.0f };
    float               scale_      = 0.0f;
    DirectX::XMFLOAT3   rotate_     = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4   color_      = { 1.0f, 1.0f, 1.0f, 1.0f };
    float               speed_      = 1.0f;
    float               angle_      = 0.0f;
    std::string         name_;
};

