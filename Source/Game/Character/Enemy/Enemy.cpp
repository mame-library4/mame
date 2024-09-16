#include "Enemy.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

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
    float dot = XMFloat2Dot(vec, ownerFront) - 1.0f;

    // 回転量が少なければここで終了
    if (dot > -0.005f) return;

    // 回転速度設定
    float rotateSpeed = GetRotateSpeed() * elapsedTime;
    float rotateY = dot * rotateSpeed;
    rotateY = std::min(rotateY, -0.7f * rotateSpeed);

    // 回転処理
    if (cross > 0)
    {
        GetTransform()->AddRotationY(rotateY);
    }
    else
    {
        GetTransform()->AddRotationY(-rotateY);
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

