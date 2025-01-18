#pragma once
#include "Item.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class MagicCircle : public Item
{
public:
    enum class STATE
    {
        Initialize,
        Launch,
        Scaling,
        Reception,
        Hit,
        Preparation,
        Attack,
        Close,
    };

public:
    MagicCircle(const DirectX::XMFLOAT3& staffPosition, const DirectX::XMFLOAT3& createPosition);
    ~MagicCircle() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit()                                        override;

private:
    DirectX::XMFLOAT3 staffPosition_    = {};
    DirectX::XMFLOAT3 createPosition_   = {}; // 生成位置
    
    // ---------- Effect ----------
    ComputeParticleEmitter  effectEmitter_          = {};
    ComputeParticleEmitter  attackEffectEmitter_    = {};
    float effectTimer_ = 0.0f;
    float effectSpeed_ = 4.0f;

    STATE state_ = STATE::Initialize;

    float scaleTimer_ = 0.0f;
    float scaleSpeed_ = 3.5f;

    float rotationSpeed_            = 0.0f;
    float scalingRotationSpeed_     = DirectX::XMConvertToRadians(360.0f);
    float receptionRotationSpeed_   = DirectX::XMConvertToRadians(10.0f);
    float attackEndRotationSpeed_   = DirectX::XMConvertToRadians(180.0f);
    float hitRotationSpeed_         = DirectX::XMConvertToRadians(540.0f);
    float closeRotationSpeed_       = DirectX::XMConvertToRadians(-540.0f);

    DirectX::XMFLOAT3 baseColor_ = { 0.0f, 1.0f, 1.0f };

    // ---------- 攻撃受付用 ----------
    float receptionTimer_ = 0.0f;
    float receptionTime_ = 3.0f;
    // ---------- Hit時用 ----------
    DirectX::XMFLOAT3   hitColor_                   = { 1.0f, 0.0f, 0.0f };
    float               hitScale_                   = 3.0f;
    float               hitOutlineSize_             = 2.0f;
    float               hitTimer_                   = 0.0f;
    float               hitTimerSpeed_              = 3.0f;
    float               hitTimerScaleSpeed_         = 5.0f;
    float               hitTimerOutlineScaleSpeed_  = 3.0f;
    // ---------- 攻撃準備用 ----------
    float               preparationScale_           = 5.0f;
    float               preparationOutlineSize_     = 0.1f;
    float               preparationTimer_           = 0.0f;
    float               preparationTimerSpeed_      = 10.0f;
    // ---------- 攻撃時用 ----------
    float               attackTimer_                = 0.0f;
    float               attackTime_                 = 3.0f;
    float               attackEndDelay_             = 1.0f;
    bool                isAbleAttack_               = false;
    
    bool  isUpdateScale_        = false;
    bool  isUpdateOutlineSize_  = false;
    // ---------- Close用 ----------
    DirectX::XMFLOAT3   oldColor_       = {};   
    DirectX::XMFLOAT3   closeColor_     = {};   // 目標色
    float               oldScale_       = 0.0f;
    float               closeScale_     = 0.0f; // 目標サイズ
    float               closeTimer_     = 0.0f; 
    float               closeSpeed_     = 5.0f;
    bool                isUpdateColor_  = false;
};

