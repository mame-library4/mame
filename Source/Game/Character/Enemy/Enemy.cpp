#include "Enemy.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- コンストラクタ -----
Enemy::Enemy(std::string filename)
    : Character(filename)
{
}

// ----- 回転処理 -----
void Enemy::Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos)
{
    DirectX::XMFLOAT2 ownerFront = { GetTransform()->CalcForward().x , GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 target = { targetPos.x , targetPos.z };
    DirectX::XMFLOAT2 vec = XMFloat2Normalize(target - ownerPos);

    // 外積でどっちに回転するか判断
    float cross = XMFloat2Cross(vec, ownerFront);

    // 内積で回転幅を算出
    ownerFront = XMFloat2Normalize(ownerFront);
    float dot = XMFloat2Dot(vec, ownerFront) - 1.0f;

    // 回転速度設定
    float rotateSpeed = GetRotateSpeed() * elapsedTime;

    // 回転処理
    if (cross > 0)
    {
        GetTransform()->AddRotationY(dot * rotateSpeed);
    }
    else
    {
        GetTransform()->AddRotationY(-dot * rotateSpeed);
    }
}

// ----- ノード更新 -----
void Enemy::UpdateNode(const float& elapsedTime)
{
    // 現在実行するノードがあれば、ビヘイビアツリーからノードを実行
    if (activeNode_ != nullptr)
    {
        activeNode_ = behaviorTree_->Run(activeNode_, behaviorData_.get(), elapsedTime);
    }
    // 現在実行されているノードがなければ、次に実行するノードを推論する
    else
    {
        activeNode_ = behaviorTree_->ActiveNodeInference(behaviorData_.get());
    }
}

// ----- ブレンドアニメーション設定 -----
void Enemy::PlayBlendAnimation(const TamamoAnimation& index, const bool& loop, const float& speed)
{
    const int currentAnimationIndex = GetCurrentBlendAnimationIndex();

    // 現在のアニメーションと引数のアニメーションが同じ場合
    if (currentAnimationIndex == static_cast<int>(index))
    {
        // 仮でwalkを一つ目のブレンド引数に入れておく
        Object::PlayBlendAnimation(static_cast<int>(Enemy::TamamoAnimation::Walk), static_cast<int>(index), loop, speed);
        SetWeight(1.0f);
        return;
    }

    // 攻撃アニメーションの時はweight値を１にする
    if (currentAnimationIndex != static_cast<int>(TamamoAnimation::Idle) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::Walk) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::WalkLeft) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::WalkRight))
    {
        SetWeight(1.0f);
    }

    Object::PlayBlendAnimation(static_cast<int>(index), loop, speed);
}

// ----- プレイヤーまでの距離を算出 -----
const float Enemy::CalcDistanceToPlayer()
{
    DirectX::XMFLOAT3 ownerPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();

    return XMFloat3Length(playerPos - ownerPos);
}

// ----- 自分自身からプレイヤーへのベクトル -----
const DirectX::XMFLOAT3 Enemy::CalcDirectionToPlayer()
{
    DirectX::XMFLOAT3 ownerPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();

    return playerPos - ownerPos;
}
