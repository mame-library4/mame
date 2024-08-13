#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>

// エフェクト
class Effect
{
public:
    Effect(const char* filename, const std::string& effectName);
    ~Effect() {};

    // 再生
    Effekseer::Handle Play(const DirectX::XMFLOAT3& position, const float& speed = 1.0f,
        const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f }, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    // 停止
    void Stop(Effekseer::Handle handle);

    // 座標設定
    void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

    // スケール設定
    void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

    void DrawDebug();

public:
    [[nodiscard]] const std::string GetName() const { return name_; }

private:
    Effekseer::EffectRef effekseerEffect_;

    DirectX::XMFLOAT3   position_   = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3   scale_      = { 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3   rotate_     = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4   color_      = { 1.0f, 1.0f, 1.0f, 1.0f };
    float               speed_      = 1.0f;
    float               angle_      = 0.0f;
    std::string         name_;
};

