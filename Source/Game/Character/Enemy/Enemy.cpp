#include "Enemy.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"
#include "UI/UIPartDestruction.h"

// ----- コンストラクタ -----
Enemy::Enemy(const std::string& filename, const float& scaleFactor)
    : Character(filename, scaleFactor)
{
}

// ----- 回転処理 -----
void Enemy::Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos)
{
    DirectX::XMFLOAT2 ownerFront = { GetTransform()->CalcForward().x , GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 target = { targetPos.x , targetPos.z };
    DirectX::XMFLOAT2 vec = XMFloat2Normalize(target - ownerPos);

    ownerFront = XMFloat2Normalize(ownerFront);

    // 外積でどっちに回転するか判断
    float cross = XMFloat2Cross(vec, ownerFront);

    // 内積で回転幅を算出
    float dot = XMFloat2Dot(vec, ownerFront);
    float angle = acosf(dot);

    if (angle < DirectX::XMConvertToRadians(1)) return;

    const float speed = GetRotateSpeed() * elapsedTime;
    angle *= speed;

    // 回転処理
    if (cross > 0)
    {
        GetTransform()->AddRotationY(-angle);
    }
    else
    {
        GetTransform()->AddRotationY(angle);
    }
}

// ----- プレイヤーを探す ( 戦闘状態に移行できるか ) -----
const bool Enemy::SearchPlayer()
{
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };

    DirectX::XMFLOAT2 vec = ownerPos - playerPos;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);

    // 近距離攻撃範囲以内にいるので強制的に戦闘状態にする
    if (dist < nearAttackRadius_) return true;

    // 戦闘範囲にいる
    if (dist < battleRadius_)
    {
        // 単位ベクトル化
        vec = vec / dist;
        
        // 方向ベクトル化
        DirectX::XMFLOAT2 frontVec = { GetTransform()->CalcForward().x, GetTransform()->CalcForward().z };
        frontVec = XMFloat2Normalize(frontVec);

        // 前後判定
        float dot = XMFloat2Dot(frontVec, vec);
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;
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

// ----- ダメージ処理 -----
void Enemy::AddDamage(const float& damage, const int& dataIndex)
{
    // ダメージ処理 ( 体力をダメージ分を引く )
    Character::AddDamage(damage);

    // 部位の体力をダメージ分引く
    AddDamagePart(damage, dataIndex);

    // 部位破壊判定
    CheckPartDestruction();
}

void Enemy::SetHitStop()
{
    hitStopFrame_ = normalHitStopFrame_;
    currentHitStopFrame_ = 0;
    isHitStopActive_ = true;
}

// ----- 部位破壊判定 -----
void Enemy::CheckPartDestruction()
{
    for (int partIndex = 0; partIndex < static_cast<int>(PartName::Max); ++partIndex)
    {
        // 部位破壊がされていない
        if (isPartDestruction_[partIndex] == false)
        {
            // 部位の体力がなくなったらUIを生成してフラグを立てる
            if (partHealth_[partIndex] <= 0.0f)
            {
                // 部位破壊UI生成
                UIPartDestruction* ui = new UIPartDestruction();

                isPartDestruction_[partIndex] = true;
            }
        }
    }
}
