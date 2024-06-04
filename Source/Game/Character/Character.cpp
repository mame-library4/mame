#include "Character.h"
#include "../Scene/GameScene.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
Character::Character(std::string filename)
    : Object(filename)
{
}

void Character::Update(const float& elapsedTime)
{
    // アニメーション更新
    Object::Update(elapsedTime);

    // 吹っ飛び更新
    UpdateForce(elapsedTime);
}

// ----- ImGui用 -----
void Character::DrawDebug()
{
    if (ImGui::TreeNode("Move"))
    {
        ImGui::DragFloat("Acceleration", &acceleration_);
        ImGui::DragFloat("Deceleration", &deceleration_);
        ImGui::DragFloat("MaxSpeed", &maxSpeed_);
        ImGui::DragFloat3("Velocity", &velocity_.x);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Rotate"))
    {
        ImGui::DragFloat("Speed", &rotateSpeed_);

        ImGui::TreePop();
    }


    ImGui::DragFloat("CollisionRadius", &collisionRadius_);

    if (ImGui::TreeNode("Collision"))
    {

        if (ImGui::TreeNode("DamageDetection"))
        {
            for (DamageDetectionData& data : damageDetectionData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("AttackDetection"))
        {
            for (AttackDetectionData& data : attackDetectionData_)
            {
                //if (data.GetIsActive() == false) continue;
                data.DrawDebug();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("CollisionDetection"))
        {
            for (CollisionDetectionData& data : collisionDetectionData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

// ----- Collision更新 -----
void Character::UpdateCollisions(const float& elapsedTime, const float& scaleFactor)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {        
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
}

// ----- ステージ外に出ないようにする -----
void Character::CollisionCharacterVsStage()
{
    const DirectX::XMFLOAT3 characterPos = GetTransform()->GetPosition();
    const DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    DirectX::XMFLOAT3 vec = characterPos - stagePos;
    const float length = XMFloat3Length(vec);
    const float radius = GameScene::stageRadius_ - collisionRadius_;
    if (length > radius)
    {
        vec = XMFloat3Normalize(vec);
        GetTransform()->SetPosition(GameScene::stageCenter_ + vec * radius);
    }
}

DirectX::XMFLOAT3 Character::SetTargetPosition()
{
    // ステージの中で移動先を決める
    DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    int length = rand() % static_cast<int>(GameScene::stageRadius_);
    DirectX::XMFLOAT3 direction = { static_cast<float>(rand() % 21 - 10), 0, static_cast<float>(rand() % 21 - 10) };
    DirectX::XMFLOAT3 vec = stagePos + XMFloat3Normalize(direction) * length;
    return vec;
}

void Character::UpdateForce(const float& elapsedTime)
{
    // パワーが無いときは処理しない
    if (blowPower_ <= 0) return;

    blowPower_ -= elapsedTime * 2.0f;
    blowPower_ = std::max(blowPower_, 0.0f); // 0.0f以下にならないように修正

    // 吹っ飛び方向にどれだけ、吹っ飛ばすかを計算する
    DirectX::XMFLOAT3 direction = {};
    direction = XMFloat3Normalize(blowDirection_) * blowPower_;

    // 吹っ飛ばす。
    GetTransform()->AddPosition(direction);
}

void Character::AddForce(const DirectX::XMFLOAT3& direction, const float& power)
{
    // Y方向には吹っ飛ばさない
    blowDirection_ = { direction.x, 0, direction.z };
    blowPower_ = power;
}

// ---------- くらい判定 ----------
#pragma region くらい判定
// ----- 登録 -----
void Character::RegisterDamageDetectionData(const DamageDetectionData& data)
{
    damageDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
DamageDetectionData& Character::GetDamageDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (DamageDetectionData& data : damageDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return DamageDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
DamageDetectionData& Character::GetDamageDetectionData(const int& index)
{
    return damageDetectionData_.at(index);
}


#pragma endregion くらい判定

// ---------- 攻撃判定 ----------
#pragma region 攻撃判定
// ----- 登録 -----
void Character::RegisterAttackDetectionData(const AttackDetectionData& data)
{
    attackDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
AttackDetectionData& Character::GetAttackDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (AttackDetectionData& data : attackDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return AttackDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
AttackDetectionData& Character::GetAttackDetectionData(const int& index)
{
    return attackDetectionData_.at(index);
}
#pragma endregion 攻撃判定

// ---------- 押し出し判定 ----------
#pragma region 押し出し判定
void Character::RegisterCollisionDetectionData(const CollisionDetectionData& data)
{
    collisionDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
CollisionDetectionData& Character::GetCollisionDetectionData(const std::string& name)
{
    // 名前でデータを探す
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return CollisionDetectionData();
}

// ----- データ取得 ( 登録番号 ) -----
CollisionDetectionData& Character::GetCollisionDetectionData(const int& index)
{
    return collisionDetectionData_.at(index);
}
#pragma endregion 押し出し判定