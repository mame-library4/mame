#pragma once
#include "ParticleSystem.h"
#include <vector>
#include "Shader.h"

class TailParticle : public ParticleSystem
{
public:
    TailParticle();
    ~TailParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayTailParticle();
    void PlayTailTrailParticle();

    // ---------- 尻尾の位置更新 ----------
    void UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition);

    void Remove();

private:
    // ---------- 尻尾のパーティクル半径更新 ----------
    void UpdateTailRadius(const float& elapsedTime);

private:
#pragma region ---------- ParticleData ----------
    struct Particle
    {
        DirectX::XMFLOAT4   color_          = {};
        DirectX::XMFLOAT3   position_       = {}; 
        DirectX::XMFLOAT3   randomOffset_   = {}; 
        DirectX::XMFLOAT3   velocity_       = {}; 
        float               size_           = 0.0f;
        int                 tailIndex_      = 0;  // 尻尾の番号
        int                 state_          = 0;
    };
    struct TrailParticle
    {
        DirectX::XMFLOAT4   color_       = {};
        DirectX::XMFLOAT3   position_    = {};
        DirectX::XMFLOAT3   velocity_    = {};
        float               speed_       = 0.0f;
        float               size_        = 0.0f;
        float               createTimer_ = 0.0f;
        float               life_        = 0.0f;
        int                 state_       = 0;
        int                 jointIndex_  = 0;
    };

    static const int maxJointNum_ = 6;
    struct Constants
    {
        DirectX::XMFLOAT4   jointPosition_[maxJointNum_] = {};
        DirectX::XMFLOAT4   direction_[4]                = {};
        DirectX::XMFLOAT4   height_                      = {};
        DirectX::XMFLOAT4   radius_                      = {};
        float               deltaTime_                   = 0.0f;
        float               tailTrailTimer_              = 0.0f;        
        int                 tailParticleState_           = 0;
        int dummy_ = 0;
    };

#pragma endregion ---------- ParticleData ----------

    Constants           constants_;

    const int cbSlot_ = 2;
    const int csUAVSlot_ = 0;
    const int gsSRVSlot_ = 0;

    // ----- tailTrail -----
    ParticleData tailTrailParticle_;

    float   lifeTimer_  = 30.0f;  // 生存時間
    float   lerpTimer_  = 0.0f;
    int     state_      = 0;
    bool    isRemove_   = false;  // 消去準備フラグ

    // ----- ポイントライト -----
    float fadeOutSpeed_ = 30.0f;
    float maxIntensity_ = 10.0f;
    static const int maxPointLights_ = 4;
    Shader::PointLights pointLights_[maxPointLights_] = {};
};

