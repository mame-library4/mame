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
        if (ImGui::TreeNode("Cylinder"))
        {
            for (CollisionCylinderData& data : collisionCylinderData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Sphere"))
        {
            for (CollisionSphereData& data : collisionSphereData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

// ----- 円柱判定用データ更新 -----
void Character::UpdateCollisionCylinderData(const float& scaleFactor)
{
    // 各データの位置をjointPositionで更新。
    // jointPositionがない場合、
    // 位置には (0, 0, 0) が入るため、その子たちは別途更新必要
    for (CollisionCylinderData& data : collisionCylinderData_)
    {
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor));
    }
}

// ----- 球判定用データ更新 -----
void Character::UpdateCollisionSphereData(const float& scaleFactor)
{
    // 各データの位置をjointPositionで更新。
    // jointPositionがない場合、
    // 位置には (0, 0, 0) が入るため、その子たちは別途更新必要
    for (CollisionSphereData& data : collisionSphereData_)
    {
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor));
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

DirectX::XMFLOAT3 Character::SetTargetPosition(const DirectX::XMFLOAT3& pos)
{
    // kokoshuuseisinaakannkamo
    DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    DirectX::XMFLOAT3 vec = pos - stagePos;
    float length = XMFloat3Length(vec);
    if (length > GameScene::stageRadius_)
    {
        vec = XMFloat3Normalize(vec);
        // kokoshuuseisinaakannkamo
        return vec * GameScene::stageRadius_;
    }
    else
    {
        return pos;
    }
}

// ----- 円柱判定情報登録 -----
void Character::RegisterCollisionCylinderData(const CollisionCylinderData& data)
{
    collisionCylinderData_.emplace_back(data);
}

// ----- 円柱判定情報取得 -----
Character::CollisionCylinderData& Character::GetCollisionCylinderData(const std::string& name)
{
    for (CollisionCylinderData& data : collisionCylinderData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return CollisionCylinderData();
}

// ----- 球判定情報登録 -----
void Character::RegisterCollisionSphereData(const CollisionSphereData& data)
{
    collisionSphereData_.emplace_back(data);
}

// ----- 球判定情報取得 -----
Character::CollisionSphereData& Character::GetCollisionSphereData(const std::string& name)
{
    for (CollisionSphereData& data : collisionSphereData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // 見つからなかった
    return CollisionSphereData();
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
