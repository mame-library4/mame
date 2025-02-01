#include "AquaMeteor.h"
#include "MathHelper.h"
#include "ProjectileManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Camera.h"
#include "GamePad.h"

// ----- コンストラクタ -----
AquaMeteor::AquaMeteor(const float& cameraShakePower, const float& cameraShakeTime,
    const DirectX::XMFLOAT2& gamePadVibrationPower, const float& gamePadVibrationTime)
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "AquaMeteor",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::Enemy)),
    cameraShakePower_(cameraShakePower), cameraShakeTime_(cameraShakeTime),
    gamePadVibrationPower_(gamePadVibrationPower), gamePadVibrationTime_(gamePadVibrationTime)
{
    aquaConstants_ = std::make_unique<ConstantBuffer<AquaConstants>>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/AquaBulletPS.cso", aquaPS_.GetAddressOf());
    D3D11_TEXTURE2D_DESC desc = {};
    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Noise1.png", shaderResourceView_.GetAddressOf(), &desc);
}

// ----- 初期化 -----
void AquaMeteor::Initialize()
{
    Object::SetScrollDirection({ 1.0f, 1.0f });

    GetTransform()->SetScaleFactor(0.0f);

    // 攻撃判定設定
    SetRadius(1.0f);

    // 攻撃力設定
    SetDamage(90.0f);

    // エフェクト読み込み
    computeParticleEmitter_.SetEmitParameter("AquaMeteorCharge");
    hitEffectEmitter0_.SetEmitParameter("AquaMeteorHitEffect");
    hitEffectEmitter1_.SetEmitParameter("AquaMeteorHitEffect2");
}

// ----- 終了化 -----
void AquaMeteor::Finalize()
{
}

// ----- 更新 -----
void AquaMeteor::Update(const float& elapsedTime)
{
    // Hit後の処理
    if (GetIsHit())
    {
        hitTimer_ += elapsedTime;
        hitEffectTimer_ += elapsedTime;

        if (hitEffectTimer_ >= hitEffectCreateSpan_)
        {
            hitEffectEmitter1_.SetEmitPosition(GetTransform()->GetPosition());
            hitEffectEmitter1_.EmitParticle();

            hitEffectTimer_ = 0.0f;
        }

        if (hitTimer_ >= 1.0f)
        {
            // 自分自身を削除する
            ProjectileManager::Instance().Remove(this);
        }

        return;
    }

    lifeTimer_ -= elapsedTime;
    // 寿命が尽きたら自分自身を削除する
    if (lifeTimer_ <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }

    // チャージ時
    if (isLaunched_ == false)
    {
        DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
        playerPosition.y = positionY_;
        GetTransform()->SetPosition(playerPosition);
    }
    // 発射後
    else
    {
        GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);
    }

    Object::AddScrollTimer(elapsedTime);

    // サイズ設定
    scaleTimer_ += scaleSpeed_ * elapsedTime;
    scaleTimer_ = std::min(1.0f, scaleTimer_);
    const float scaleFactor = XMFloatLerp(0.0f, 1.0f, scaleTimer_);
    GetTransform()->SetScaleFactor(scaleFactor);

    // エフェクト
    if (isLaunched_ == false)
    {
        computeParticleEmitter_.SetEmitPosition(GetTransform()->GetPosition());
        computeParticleEmitter_.EmitParticle();
    }
}

// ----- 描画 -----
void AquaMeteor::Render(ID3D11PixelShader* psShader)
{
    // ヒットしているため、描画しない
    if (GetIsHit()) return;

    aquaConstants_->Activate(9);
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(9, 1, shaderResourceView_.GetAddressOf());
    Object::Render(aquaPS_.Get());
}

// ----- ImGui用 -----
void AquaMeteor::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        //ImGui::DragFloat("LifeTimer", &lifeTimer_);
        //ImGui::Checkbox("IsTrailEffectGeneratable", &isTrailEffectGeneratable_);
        //ImGui::DragFloat3("MoveDirection", &moveDirection_.x);
        //ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);

        Projectile::DrawDebug();

        ImGui::ColorEdit3("BaseColor", &aquaConstants_->GetData()->baseColor_.x);
        ImGui::ColorEdit3("RimColor", &aquaConstants_->GetData()->rimColor_.x);
        ImGui::DragFloat("RimThreshold", &aquaConstants_->GetData()->rimThreshold_, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("RimAmount", &aquaConstants_->GetData()->rimAmount_, 0.001f, 0.0f, 1.0f);

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼ばれる -----
const bool AquaMeteor::OnHit(const DirectX::XMFLOAT3& hitPosition)
{
    // まだ発射していない
    if (isLaunched_ == false) return false;
    // 既に当たっている
    if (GetIsHit()) return false;

    // エフェクト再生
    hitEffectEmitter0_.SetEmitPosition(GetTransform()->GetPosition());
    hitEffectEmitter0_.EmitParticle();

    // カメラシェイクを入れる
    Camera::Instance().ScreenVibrate(cameraShakePower_, cameraShakeTime_);

    // コントローラー振動入れる
    Input::Instance().GetGamePad().Vibration(gamePadVibrationTime_, gamePadVibrationPower_.x, gamePadVibrationPower_.y);

    return true;
}

// ----- 発射 -----
void AquaMeteor::Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& direction, const float& speed)
{
    const DirectX::XMFLOAT3 enemyPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition("Dragon15_spine2");
    const DirectX::XMFLOAT3 moveDirection = XMFloat3Normalize(enemyPosition - GetTransform()->GetPosition());

    moveDirection_ = direction;
    moveDirection_.y = moveDirection.y;
    moveDirection_ = XMFloat3Normalize(moveDirection_);

    moveSpeed_ = speed;

    isLaunched_ = true;
}