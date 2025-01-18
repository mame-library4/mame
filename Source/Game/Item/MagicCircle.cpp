#include "MagicCircle.h"
#include "ItemManager.h"
#include "MathHelper.h"
#include "Easing.h"

// ----- コンストラクタ -----
MagicCircle::MagicCircle(const DirectX::XMFLOAT3& staffPosition, const DirectX::XMFLOAT3& createPosition)
    : Item("./Resources/Model/MagicCircle/MagicCircle.gltf", 1.0f, "MagicCircle", static_cast<int>(ItemManager::ItemType::MagicCircle)),
    staffPosition_(staffPosition), createPosition_(createPosition)
{
}

// ----- 初期化 -----
void MagicCircle::Initialize()
{
    // 位置設定
    GetTransform()->SetPosition(createPosition_);
    GetTransform()->SetPositionY(0.1f);

    // サイズ設定
    GetTransform()->SetScaleFactor(0.0f);

    // エミッシブ設定する
    SetEmissive({ 0.0f, 0.0f, 1.0f }, 1.0f);

    // アウトラインを使用
    SetIsOutlineActive(true);
    SetOutlineSize(0.1f);
    SetOutlineColor({ 0.0f, 1.0f, 1.0f, 1.0f });

    // エフェクト読み込み
    effectEmitter_.SetEmitParameter("HailBoltTrail");
    attackEffectEmitter_.SetEmitParameter("MagicCircleAttack");

    // 受付範囲を設定
    damageRadius_ = 2.0f;

    attackRadius_ = 0.0f;
}

// ----- 終了化 -----
void MagicCircle::Finalize()
{
}

// ----- 更新 -----
void MagicCircle::Update(const float& elapsedTime)
{
    switch (state_)
    {
    case STATE::Initialize:// 初期化

        state_ = STATE::Launch;
        rotationSpeed_ = scalingRotationSpeed_;

        break;
    case STATE::Launch:// 発射
        effectTimer_ += effectSpeed_ * elapsedTime;
        effectTimer_ = std::min(1.0f, effectTimer_);
        DirectX::XMFLOAT3 effectEmitPosition = {};
        effectEmitPosition = XMFloat3Lerp(staffPosition_, createPosition_, effectTimer_);
        effectEmitPosition.y = Easing::InCubic(effectTimer_, 1.0f, createPosition_.y, staffPosition_.y);
        effectEmitter_.SetEmitPosition(effectEmitPosition);
        effectEmitter_.EmitParticle();

        if(effectTimer_ == 1.0f) state_ = STATE::Scaling;

        break;
    case STATE::Scaling:// 魔法陣展開
    {
        scaleTimer_ += scaleSpeed_ * elapsedTime;
        scaleTimer_ = std::min(1.0f, scaleTimer_);
        const float scale = XMFloatLerp(0.0f, 5.0f, scaleTimer_);
        GetTransform()->SetScaleFactor(scale);

        if (scaleTimer_ == 1.0f)
        {
            scaleTimer_     = 0.0f;
            rotationSpeed_  = receptionRotationSpeed_;
            receptionTimer_ = receptionTime_;
            state_          = STATE::Reception;
        }
    }
        break;
    case STATE::Reception:// カウンター受付中
        receptionTimer_ -= elapsedTime;

        if (receptionTimer_ <= 0.0f)
        {
            oldColor_ = baseColor_;
            oldScale_ = GetTransform()->GetScaleFactor();
            isUpdateColor_ = true;

            state_ = STATE::Close;
        }

        break;
    case STATE::Hit:// 当たった時
    {
        hitTimer_ += hitTimerSpeed_ * elapsedTime;
        hitTimer_ = std::min(1.0f, hitTimer_);

        // BaseColor, OutlineColor, Scale更新
        if (isUpdateScale_)
        {
            const DirectX::XMFLOAT3 color = XMFloat3Lerp(baseColor_, hitColor_, hitTimer_);
            const float scale = XMFloatLerp(5.0f, hitScale_, hitTimer_);

            SetEmissiveColor(color);
            SetOutlineColor(color);
            GetTransform()->SetScaleFactor(scale);

            if (hitTimer_ == 1.0f)
            {
                hitTimerSpeed_ = hitTimerOutlineScaleSpeed_;

                isUpdateScale_ = false;
                hitTimer_ = 0.0f;
            }
        }
        // OutlineSize の更新
        else
        {           
            const float outlineSize = Easing::OutCubic(hitTimer_, 1.0f, hitOutlineSize_, preparationOutlineSize_);
            SetOutlineSize(outlineSize);
        }


        if (hitTimer_ == 1.0f)
        {
            isUpdateOutlineSize_ = true;

            // 速度設定
            rotationSpeed_ = hitRotationSpeed_;
            // ステート変更
            state_ = STATE::Preparation;
        }
    }
        break;
    case STATE::Preparation:// 攻撃
    {
        preparationTimer_ += preparationTimerSpeed_ * elapsedTime;
        preparationTimer_ = std::min(1.0f, preparationTimer_);

        // アウトラインのサイズを更新
        if (isUpdateOutlineSize_)
        {
            const float outlineSize = Easing::InCubic(preparationTimer_, 1.0f, preparationOutlineSize_, hitOutlineSize_);
            SetOutlineSize(outlineSize);

            if (preparationTimer_ == 1.0f)
            {
                isUpdateOutlineSize_ = false;
                preparationTimer_ = 0.0f;
            }
        }
        // サイズを更新
        else
        {
            const float scale = XMFloatLerp(hitScale_, preparationScale_, preparationTimer_);
            GetTransform()->SetScaleFactor(scale);

            if (preparationTimer_ == 1.0f)
            {
                isAbleAttack_ = true;

                // ステート変更
                state_ = STATE::Attack;
            }
        }
    }
        break;
    case STATE::Attack:// 攻撃

        attackTimer_ += elapsedTime;

        // 攻撃
        if (isAbleAttack_)
        {
            attackTimer_ = std::min(attackTimer_, attackTime_);

            // エフェクト生成
            attackEffectEmitter_.SetEmitPosition(GetTransform()->GetPosition());
            attackEffectEmitter_.EmitParticle();

            if (attackTimer_ >= attackTime_)
            {
                attackTimer_    = 0.0f;
                rotationSpeed_  = attackEndRotationSpeed_;
                isAbleAttack_   = false;
            }
        }
        // 攻撃終了
        else
        {
            attackTimer_ = std::min(attackTimer_, attackEndDelay_);
            
            if (attackTimer_ >= attackEndDelay_)
            {
                oldColor_ = baseColor_;
                oldScale_ = GetTransform()->GetScaleFactor();
                isUpdateColor_ = true;

                state_ = STATE::Close;
            }
        }

        break;
    case STATE::Close:// 終了
    {
        closeTimer_ += closeSpeed_ * elapsedTime;
        closeTimer_ = std::min(1.0f, closeTimer_);

        // 色を黒色にする
        if (isUpdateColor_)
        {
            const DirectX::XMFLOAT3 color = XMFloat3Lerp(oldColor_, closeColor_, closeTimer_);
            SetEmissiveColor(color);
            SetOutlineColor(color);

            if (closeTimer_ == 1.0f)
            {
                closeTimer_ = 0.0f;

                rotationSpeed_ = closeRotationSpeed_;

                isUpdateColor_ = false;
            }
        }
        else
        {
            const float scale = XMFloatLerp(oldScale_, closeScale_, closeTimer_);
            GetTransform()->SetScaleFactor(scale);

            if (closeTimer_ == 1.0f)
            {
                ItemManager::Instance().Remove(this);
            }
        }
    }
        break;
    }

    GetTransform()->AddRotationY(rotationSpeed_ * elapsedTime);
}

// ----- 描画 -----
void MagicCircle::Render(ID3D11PixelShader* psShader)
{
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Object::Render(psShader);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);
}

// ----- ImGui用 -----
void MagicCircle::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Remove")) ItemManager::Instance().Remove(this);

        //Item::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼ばれる -----
void MagicCircle::OnHit()
{
    // 現在当たり判定を受け付けていない
    if (state_ != STATE::Reception) return;

    isHit_ = true;

    // スケール更新
    hitTimerSpeed_ = hitTimerScaleSpeed_;
    isUpdateScale_ = true;

    state_ = STATE::Hit;
}
