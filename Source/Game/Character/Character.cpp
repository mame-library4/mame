#include "Character.h"
#include "../Scene/GameScene.h"
#include "../Other/MathHelper.h"

// ----- �R���X�g���N�^ -----
Character::Character(std::string filename)
    : Object(filename)
{
}

// ----- ImGui�p -----
void Character::DrawDebug()
{
    if (ImGui::TreeNode("Collision"))
    {
        for (CollisionCylinderData& data : collisionCylinderData_)
        {
            data.DrawDebug();
        }

        ImGui::TreePop();
    }
}

// ----- �~������p�f�[�^�X�V -----
void Character::UpdateCollisionCylinderData(const float& scaleFactor)
{
    // �e�f�[�^�̈ʒu��jointPosition�ōX�V�B
    // jointPosition���Ȃ��ꍇ�A
    // �ʒu�ɂ� (0, 0, 0) �����邽�߁A���̎q�����͕ʓr�X�V�K�v
    for (CollisionCylinderData& data : collisionCylinderData_)
    {
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor));
    }
}

// ----- �X�e�[�W�O�ɏo�Ȃ��悤�ɂ��� -----
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

// ----- �~��������o�^ -----
void Character::RegisterCollisionCylinderData(const CollisionCylinderData& data)
{
    collisionCylinderData_.emplace_back(data);
}

// ----- �~��������擾 -----
Character::CollisionCylinderData& Character::GetCollisionCylinderData(const std::string& name)
{
    for (CollisionCylinderData& data : collisionCylinderData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // ������Ȃ�����
    return CollisionCylinderData();
}

// ----- ImGui�p -----
void Character::CollisionCylinderData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_);
        ImGui::DragFloat("height", &height_);
        ImGui::TreePop();
    }
}
