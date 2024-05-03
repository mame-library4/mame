#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

// エフェクト
class Effect
{
public:
    Effect(const char* filename);
    ~Effect() {};

    // 再生
    Effekseer::Handle Play(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale = { 1.0f,1.0f,1.0f }, DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });

    // 停止
    void Stop(Effekseer::Handle handle);

    // 座標設定
    void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

    // スケール設定
    void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

    // debug
    void DrawDebug();

    // fadeout
    void FadeOutEffect(Effekseer::Handle handle, const float time);
    Effekseer::Handle FadeOutEffect(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 color, const float time);
    Effekseer::Handle FadeOutEffect(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT4 color, const float time);

public:
    void SetTimer(float t) { timer = t; }
    void AddTimer(float t) { timer += t; }
    float GetTimer() { return timer; }

    void SetPosition(DirectX::XMFLOAT3 p) { pos = p; }
    void SetScale(DirectX::XMFLOAT3 s) { scale = s; }
    void SetRotate(DirectX::XMFLOAT3 r) { rotate = r; }
    void SetColor(DirectX::XMFLOAT4 c) { color = c; }

    DirectX::XMFLOAT3 GetPosition() { return pos; }
    DirectX::XMFLOAT3 GetScale() { return scale; }
    DirectX::XMFLOAT3 GetRotate() { return rotate; }
    DirectX::XMFLOAT4 GetColor() { return color; }

    Effekseer::Handle handle;

    float angle = 0;
    void SetAngle(float a) { angle = a; }
    float GetAngle() { return angle; }
    void SubtractAngle(float a) { angle -= a; }

private:
    Effekseer::EffectRef effekseerEffect;

    DirectX::XMFLOAT3 pos{ -2.5,2,10 };             // 位置
    DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };      // 大きさ
    DirectX::XMFLOAT3 rotate{ 0,0,0 };
    DirectX::XMFLOAT4 color{ 0.0f,1.0f,0.0f,1.0f }; // 色

    int drawTime = 0;

    float timer = 4.5f; // これ条件のやつ

public:
    // Imgui名前被り防止のためのやつ //
    static int nameNum;
    const char* GetName() const { return name.c_str(); }
    void SetName(const char* n) { name = n; }
    // Imgui名前被り防止のためのやつ //    

private:
    std::string name;   // Imgui用
};

