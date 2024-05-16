#include "Character.h"
#include "../Scene/GameScene.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
Character::Character(std::string filename)
    : Object(filename)
{
}

// ----- ImGui用 -----
void Character::DrawDebug()
{
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
void Character::UpdateCollisions(const float& scaleFactor)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
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
    DirectX::XMFLOAT3 characterPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    DirectX::XMFLOAT3 vec = characterPos - stagePos;
    float length = XMFloat3Length(vec);
    if (length > GameScene::stageRadius_)
    {
        vec = XMFloat3Normalize(vec);
        GetTransform()->SetPosition(GameScene::stageCenter_ + vec * GameScene::stageRadius_);
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

// ----- ImGui用 -----
void Character::CollisionCylinderData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::DragFloat("height", &height_, 0.01f);
        ImGui::TreePop();
    }
}

// ----- ImGui用 -----
void Character::CollisionSphereData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::TreePop();
    }
}

// ---------- くらい判定 ----------
#pragma region くらい判定
// ----- 登録 -----
void Character::RegisterDamageDetectionData(const DamageDetectionData& data)
{
    damageDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
Character::DamageDetectionData& Character::GetDamageDetectionData(const std::string& name)
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
Character::DamageDetectionData& Character::GetDamageDetectionData(const int& index)
{
    return damageDetectionData_.at(index);
}

// ----- ImGui用 -----
void Character::DamageDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
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
Character::AttackDetectionData& Character::GetAttackDetectionData(const std::string& name)
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
Character::AttackDetectionData& Character::GetAttackDetectionData(const int& index)
{
    return attackDetectionData_.at(index);
}

// ----- ImGui用 -----
void Character::AttackDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

#pragma endregion 攻撃判定

// ---------- 押し出し判定 ----------
#pragma region 押し出し判定
void Character::RegisterCollisionDetectionData(const CollisionDetectionData& data)
{
    collisionDetectionData_.emplace_back(data);
}

// ----- データ取得 ( 名前検索 ) -----
Character::CollisionDetectionData& Character::GetCollisionDetectionData(const std::string& name)
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
Character::CollisionDetectionData& Character::GetCollisionDetectionData(const int& index)
{
    return collisionDetectionData_.at(index);
}

// ----- ImGui用 -----
void Character::CollisionDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

#pragma endregion 押し出し判定

