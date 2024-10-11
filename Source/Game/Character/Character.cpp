#include "Character.h"
#include "../Scene/GameScene.h"
#include "MathHelper.h"

// ----- �R���X�g���N�^ -----
Character::Character(const std::string& filename, const float& scaleFactor)
    : Object(filename, scaleFactor)
{
}

// ----- �X�V -----
void Character::Update(const float& elapsedTime)
{
    // �A�j���[�V�����X�V
    Object::Update(elapsedTime);

    // ������эX�V
    UpdateForce(elapsedTime);

    // LookAt�X�V
    LookAtUpdate();
}

// ----- ImGui�p -----
void Character::DrawDebug()
{
    if (ImGui::TreeNode("HP"))
    {
        ImGui::DragFloat("Health", &health_);
        if (ImGui::Button("Add")) health_ += 10.0f;
        if (ImGui::Button("Reduce")) health_ -= 10.0f;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("LookAt"))
    {
        ImGui::Checkbox("USE", &isLookAt_);
        ImGui::DragFloat3("GlobalForward", &headGlobalForward_.x);
        ImGui::DragFloat3("TargetPosition", &lookAtTargetPosition_.x);
        ImGui::DragInt("JointIndex", &headJointIndex_);
        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Move"))
    {
        ImGui::DragFloat("Acceleration", &acceleration_);
        ImGui::DragFloat("Deceleration", &deceleration_);
        ImGui::DragFloat("MaxSpeed", &maxSpeed_);
        ImGui::DragFloat3("Velocity", &velocity_.x);

        float speed = XMFloat3Length(velocity_);
        ImGui::DragFloat("Speed", &speed);

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

// ----- Collision�X�V -----
void Character::UpdateCollisions(const float& elapsedTime)
{
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // �U������X�V
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
    // �����o������X�V
    for (CollisionDetectionData& data : collisionDetectionData_)
    {        
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
}

// ----- �X�e�[�W�O�ɏo�Ȃ��悤�ɂ��� -----
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

    blowPower_ -= decelerationForce_ * elapsedTime;
    blowPower_ = std::max(blowPower_, 0.0f); // 0.0f�ȉ��ɂȂ�Ȃ��悤�ɏC��

    // ������ѕ����ɂǂꂾ���A������΂������v�Z����
    DirectX::XMFLOAT3 direction = {};
    direction = XMFloat3Normalize(blowDirection_) * blowPower_ * elapsedTime;

    // ������΂��B
    GetTransform()->AddPosition(direction);
}

void Character::AddForce(const DirectX::XMFLOAT3& direction, const float& power, const float& decelerationForce)
{
    // Y�����ɂ͐�����΂��Ȃ�
    //blowDirection_ = { direction.x, 0, direction.z };
    blowDirection_ = direction;
    blowPower_ = power;
    decelerationForce_ = decelerationForce;
}

// ----- LookAt������ -----
void Character::LookAtInitilaize(const std::string& nodeName)
{
    // ���̃W���C���g�ԍ���ۑ�
    headJointIndex_ = GetNodeIndex(nodeName);

    GltfModel::Node node = GetNodes()->at(headJointIndex_);

    // ���̃O���[�o���̑O������ۑ�
    headGlobalForward_ = { node.globalTransform_._31, node.globalTransform_._32, node.globalTransform_._33 };
}

// ----- LookAt�X�V -----
void Character::LookAtUpdate()
{
    // LookAt���������邩
    if (isLookAt_ == false) return;

    // ���[���h�s��
    DirectX::XMMATRIX World = GetTransform()->CalcWorldMatrix(GetScaleFactor());

    // ���[���h�̋t�s��
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(NULL, World);

    // ���̃m�[�h
    GltfModel::Node& node = GetNodes()->at(headJointIndex_);

    // ���̃O���[�o���ʒu
    DirectX::XMFLOAT3 headGlobalPosition = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };

    // �^�[�Q�b�g�̃��[���h�ʒu
    DirectX::XMFLOAT3 targetWorldPosition = lookAtTargetPosition_;
    // �^�[�Q�b�g�̃O���[�o���ʒu
    DirectX::XMFLOAT3 targetGlobalPosition = {};
    DirectX::XMStoreFloat3(&targetGlobalPosition, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&targetWorldPosition), InverseWorld));

    // ������^�[�Q�b�g�ւ́@�O���[�o���̃x�N�g�������
    DirectX::XMFLOAT3 headToTargetGlobal = targetGlobalPosition - headGlobalPosition;

    // ���̃O���[�o���t�s��
    DirectX::XMMATRIX InverseGlobalTransform = DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_));

    // �O���[�o���̂��̂����[�J����Ԃɗ��Ƃ�����
    DirectX::XMVECTOR headToTargetLocal = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&headToTargetGlobal), InverseGlobalTransform);
    DirectX::XMVECTOR headLocalForward = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&headGlobalForward_), InverseGlobalTransform);

    // ��]�������߂�
    DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(headLocalForward, headToTargetLocal);

    // ��]�p�����߂�
    float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(headLocalForward, headToTargetLocal));

    // ��]�s������߂�
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(Axis), angle);

    // ���̃m�[�h�̃O���[�o���g�����X�t�H�[�����X�V����
    DirectX::XMStoreFloat4x4(&node.globalTransform_, Rotation * DirectX::XMLoadFloat4x4(&node.globalTransform_));

    // ���̎q�m�[�h���ċA�I�ɍX�V����
    std::function<void(int, int)> UpdateGlobalTransforms = [&](int parentIndex, int nodeIndex)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        if (parentIndex > -1)
        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&GetNodes()->at(parentIndex).globalTransform_));
        }
        for (int childIndex : node.children_)
        {
            UpdateGlobalTransforms(nodeIndex, childIndex);
        }
    };
    UpdateGlobalTransforms(-1, headJointIndex_);
}

// ---------- ���炢���� ----------
#pragma region ���炢����
// ----- �o�^ -----
void Character::RegisterDamageDetectionData(const DamageDetectionData& data)
{
    damageDetectionData_.emplace_back(data);
}

// ----- �f�[�^�擾 ( ���O���� ) -----
DamageDetectionData& Character::GetDamageDetectionData(const std::string& name)
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
DamageDetectionData& Character::GetDamageDetectionData(const int& index)
{
    return damageDetectionData_.at(index);
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
AttackDetectionData& Character::GetAttackDetectionData(const std::string& name)
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
AttackDetectionData& Character::GetAttackDetectionData(const int& index)
{
    return attackDetectionData_.at(index);
}
#pragma endregion �U������

// ---------- �����o������ ----------
#pragma region �����o������
void Character::RegisterCollisionDetectionData(const CollisionDetectionData& data)
{
    collisionDetectionData_.emplace_back(data);
}

// ----- �f�[�^�擾 ( ���O���� ) -----
CollisionDetectionData& Character::GetCollisionDetectionData(const std::string& name)
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
CollisionDetectionData& Character::GetCollisionDetectionData(const int& index)
{
    return collisionDetectionData_.at(index);
}
#pragma endregion �����o������