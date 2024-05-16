#include "Character.h"
#include "../Scene/GameScene.h"
#include "../Other/MathHelper.h"

// ----- �R���X�g���N�^ -----
Character::Character(std::string filename)
    : Object(filename)
{
}

void Character::Update(const float& elapsedTime)
{
    // �A�j���[�V�����X�V
    Object::Update(elapsedTime);

    // ������эX�V
    UpdateForce(elapsedTime);
}

// ----- ImGui�p -----
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

// ----- Collision�X�V -----
void Character::UpdateCollisions(const float& scaleFactor)
{
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // �U������X�V
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // �����o������X�V
    for (CollisionDetectionData& data : collisionDetectionData_)
    {        
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
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

DirectX::XMFLOAT3 Character::SetTargetPosition()
{
    // �X�e�[�W�̒��ňړ�������߂�
    DirectX::XMFLOAT3 stagePos = GameScene::stageCenter_;
    int length = rand() % static_cast<int>(GameScene::stageRadius_);
    DirectX::XMFLOAT3 direction = { static_cast<float>(rand() % 21 - 10), 0, static_cast<float>(rand() % 21 - 10) };
    DirectX::XMFLOAT3 vec = stagePos + XMFloat3Normalize(direction) * length;
    return vec;
}

void Character::UpdateForce(const float& elapsedTime)
{
    // �p���[�������Ƃ��͏������Ȃ�
    if (blowPower_ <= 0) return;

    blowPower_ -= elapsedTime * 2.0f;
    blowPower_ = std::max(blowPower_, 0.0f); // 0.0f�ȉ��ɂȂ�Ȃ��悤�ɏC��

    // ������ѕ����ɂǂꂾ���A������΂������v�Z����
    DirectX::XMFLOAT3 direction = {};
    direction = XMFloat3Normalize(blowDirection_) * blowPower_;

    // ������΂��B
    GetTransform()->AddPosition(direction);
}

void Character::AddForce(const DirectX::XMFLOAT3& direction, const float& power)
{
    // Y�����ɂ͐�����΂��Ȃ�
    blowDirection_ = { direction.x, 0, direction.z };
    blowPower_ = power;
}

// ----- ImGui�p -----
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

// ----- ImGui�p -----
void Character::CollisionSphereData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::TreePop();
    }
}

// ---------- ���炢���� ----------
#pragma region ���炢����
// ----- �o�^ -----
void Character::RegisterDamageDetectionData(const DamageDetectionData& data)
{
    damageDetectionData_.emplace_back(data);
}

// ----- �f�[�^�擾 ( ���O���� ) -----
Character::DamageDetectionData& Character::GetDamageDetectionData(const std::string& name)
{
    // ���O�Ńf�[�^��T��
    for (DamageDetectionData& data : damageDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // ������Ȃ�����
    return DamageDetectionData();
}

// ----- �f�[�^�擾 ( �o�^�ԍ� ) -----
Character::DamageDetectionData& Character::GetDamageDetectionData(const int& index)
{
    return damageDetectionData_.at(index);
}

// ----- ImGui�p -----
void Character::DamageDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

#pragma endregion ���炢����

// ---------- �U������ ----------
#pragma region �U������
// ----- �o�^ -----
void Character::RegisterAttackDetectionData(const AttackDetectionData& data)
{
    attackDetectionData_.emplace_back(data);
}

// ----- �f�[�^�擾 ( ���O���� ) -----
Character::AttackDetectionData& Character::GetAttackDetectionData(const std::string& name)
{
    // ���O�Ńf�[�^��T��
    for (AttackDetectionData& data : attackDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // ������Ȃ�����
    return AttackDetectionData();
}

// ----- �f�[�^�擾 ( �o�^�ԍ� ) -----
Character::AttackDetectionData& Character::GetAttackDetectionData(const int& index)
{
    return attackDetectionData_.at(index);
}

// ----- ImGui�p -----
void Character::AttackDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

#pragma endregion �U������

// ---------- �����o������ ----------
#pragma region �����o������
void Character::RegisterCollisionDetectionData(const CollisionDetectionData& data)
{
    collisionDetectionData_.emplace_back(data);
}

// ----- �f�[�^�擾 ( ���O���� ) -----
Character::CollisionDetectionData& Character::GetCollisionDetectionData(const std::string& name)
{
    // ���O�Ńf�[�^��T��
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        if (data.GetName() != name) continue;

        return data;
    }

    // ������Ȃ�����
    return CollisionDetectionData();
}

// ----- �f�[�^�擾 ( �o�^�ԍ� ) -----
Character::CollisionDetectionData& Character::GetCollisionDetectionData(const int& index)
{
    return collisionDetectionData_.at(index);
}

// ----- ImGui�p -----
void Character::CollisionDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

#pragma endregion �����o������

