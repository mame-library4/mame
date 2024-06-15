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
    ImGui::DragFloat3("LookAtTarget", &lookAtTargetPosition_.x);

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

// ----- Collision�X�V -----
void Character::UpdateCollisions(const float& elapsedTime, const float& scaleFactor)
{
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
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

void Character::LookAtInitilaize(const std::string& nodeName)
{
    headNodeName_ = nodeName;
    DirectX::XMMATRIX World = GetJointWorldTransform(headNodeName_, 0.01f);
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, World);
    DirectX::XMFLOAT4X4 inverseWorld = {};
    DirectX::XMStoreFloat4x4(&inverseWorld, InverseWorld);
    const DirectX::XMVECTOR HeadLocalForward = DirectX::XMVectorSet(
        inverseWorld._31, inverseWorld._32, inverseWorld._33, 0.0f);
    DirectX::XMStoreFloat3(&headLocalForward_, DirectX::XMVector3Normalize(HeadLocalForward));

}

void Character::LookAtUpdate()
{
#if 1
    std::function<void(int, DirectX::XMVECTOR)> UpdateWorldTransforms = [&](int nodeIndex, DirectX::XMVECTOR HeadPosition)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation_));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
        //DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(HeadPosition);
        DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&node.parentGlobalTransform_));
        //DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T);// *DirectX::XMLoadFloat4x4(&parentGlobalTransforms));
        
    
        //for (int childIndex : node.children_)
        //{
        //    DirectX::XMMATRIX Matrix = GetJointGlobalTransform(childIndex, 0.01f);
        //    DirectX::XMVECTOR Position = Matrix.r[3];

        //    UpdateWorldTransforms(childIndex, Position);
        //}

    };
#else
    std::function<void(int)> UpdateWorldTransforms = [&](int nodeIndex)
    {
        GltfModel::Node& node = GetNodes()->at(nodeIndex);
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation_));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
        DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&node.parentGlobalTransform_));

        for (int childIndex : node.children_)
        {
            UpdateWorldTransforms(childIndex);
        }

    };
#endif

#if 0
    // ���̃��[�J���O�����x�N�g��
    const DirectX::XMVECTOR HeadLocalForward = DirectX::XMLoadFloat3(&headLocalForward_);
    // ���̃��[���h���W�ʒu�擾
    const DirectX::XMVECTOR HeadWorldPosition = DirectX::XMLoadFloat3(&GetJointPosition(headNodeName_, 0.01f));
    // �^�[�Q�b�g�̃��[���h�ʒu
    const DirectX::XMVECTOR TargetWorldPosition = DirectX::XMLoadFloat3(&lookAtTargetPosition_);
    // ���̃��[���h�t�s��
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, GetJointWorldTransform(headNodeName_, 0.01f));
    // ���[���h��Ԃ̃^�[�Q�b�g�����x�N�g�����v�Z���A���[�J����Ԃɕϊ�
    DirectX::XMVECTOR TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetWorldPosition, HeadWorldPosition));
    TargetLocalDirection = DirectX::XMVector3TransformNormal(TargetLocalDirection, InverseWorld);
    // ��]���v�Z
    const DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(HeadLocalForward, TargetLocalDirection));
    // ��]�p�v�Z
    const float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(HeadLocalForward, TargetLocalDirection)));
    // ��]�p�������ȏꍇ�͉�]���Ȃ�
    if (fabsf(angle) > 1e-8f)
    {
        // ��]���Ɖ�]�p�����]�N�H�[�^�j�I�������߂�
        const DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(Axis, angle);

        // ���̃��[�J����]�l�ɉ�]�N�H�[�^�j�I������������
        const int nodeIndex = GetNodeIndex(headNodeName_);
        DirectX::XMFLOAT4 headRotate = GetNodes()->at(nodeIndex).rotation_;
        DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&headRotate);
        Rotation = DirectX::XMQuaternionMultiply(Rotation, Q);
        DirectX::XMStoreFloat4(&headRotate, Rotation);

        // ���m�[�h�Ƃ��̎q�m�[�h�̃��[���h�s��X�V
        GetNodes()->at(nodeIndex).rotation_ = headRotate;
        DirectX::XMVECTOR HeadLocalPosition = GetJointGlobalTransform(headNodeName_, 0.01f).r[3];
        UpdateWorldTransforms(nodeIndex, HeadLocalPosition);
    }

#else

    // ���̃��[�J���O�����x�N�g��
    DirectX::XMVECTOR HeadLocalForward = DirectX::XMLoadFloat3(&headLocalForward_);

    // ���̃��[���h���W�ʒu�擾
    DirectX::XMMATRIX HeadGlobalMatrix = GetJointGlobalTransform(headNodeName_);

    DirectX::XMVECTOR HeadPosition = HeadGlobalMatrix.r[3];
    
    DirectX::XMFLOAT3 characterFront = GetTransform()->CalcForward();

    // �^�[�Q�b�g�̃��[���h�ʒu
    DirectX::XMVECTOR TargetPosition = DirectX::XMLoadFloat3(&lookAtTargetPosition_);

    // ���̃��[���h�t�s��
    DirectX::XMMATRIX World = GetJointWorldTransform(headNodeName_, 0.01f);
    DirectX::XMMATRIX InverseWorld = DirectX::XMMatrixInverse(nullptr, World);
    DirectX::XMFLOAT4X4 inverseWorld = {};
    DirectX::XMStoreFloat4x4(&inverseWorld, InverseWorld);
    TargetPosition = DirectX::XMVector3TransformCoord(TargetPosition, InverseWorld);

    DirectX::XMVECTOR HeadPositionWorld = DirectX::XMVector3TransformCoord(HeadPosition, World);

    DirectX::XMVECTOR characterFrontInverse = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&characterFront), InverseWorld);


    // ���[���h��Ԃ̃^�[�Q�b�g�����x�N�g�����v�Z���A���[�J����Ԃɕϊ�
    //DirectX::XMVECTOR TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(TargetPosition, HeadPosition));
    DirectX::XMVECTOR TargetLocalDirection = TargetPosition;

    DirectX::XMVECTOR TargetLocalDirectionWorld = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract({}, HeadPositionWorld));
    DirectX::XMVECTOR HeadLocalForwardWorld = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(HeadLocalForward, World));
    HeadLocalForward = HeadLocalForwardWorld;
    //TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(TargetLocalDirectionWorld, InverseWorld));
    //TargetLocalDirection = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(TargetLocalDirection, InverseWorld));

    // ��]���v�Z
    //DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(HeadLocalForward, TargetLocalDirectionWorld));
    DirectX::XMVECTOR Axis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(TargetLocalDirection, characterFrontInverse));
    //Axis = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(Axis, InverseWorld));
    // ��]�p�v�Z
   // const float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(HeadLocalForward, TargetLocalDirectionWorld)));
    characterFrontInverse = DirectX::XMVector3Normalize(characterFrontInverse);
    DirectX::XMVECTOR TargetLocalDirectionNormalize = DirectX::XMVector3Normalize(TargetLocalDirection);
    float angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(TargetLocalDirectionNormalize, characterFrontInverse)));
    angle = acosf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(
        DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&lookAtTargetPosition_), HeadPositionWorld)),
        DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&characterFront)))));
    DirectX::XMVECTOR wAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(
        DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&characterFront)),
        DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&lookAtTargetPosition_), HeadPositionWorld))));
    DirectX::XMVECTOR gAxis = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(wAxis, InverseWorld));
    gAxis = DirectX::XMVector3Normalize(World.r[2]);
    // ��]�p�������ȏꍇ�͉�]���Ȃ�
    if (fabsf(angle) > 1e-8f)
    {
        const int nodeIndex = GetNodeIndex(headNodeName_);

        // ��]���Ɖ�]�p�����]�N�H�[�^�j�I�������߂�
        const DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationAxis(wAxis, angle);

        // ���̃��[�J����]�l�ɉ�]�N�H�[�^�j�I������������
        DirectX::XMFLOAT4 headRotate = GetNodes()->at(nodeIndex).rotation_;
        DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&headRotate);

        DirectX::XMMATRIX lMat = DirectX::XMMatrixRotationQuaternion(Rotation);
        DirectX::XMVECTOR wMatF = DirectX::XMVector3TransformCoord(lMat.r[2], World);
        DirectX::XMVECTOR wMatF2 = World.r[2];
        Rotation = DirectX::XMQuaternionMultiply(Rotation, Q);
        DirectX::XMStoreFloat4(&headRotate, Rotation);

        // ���m�[�h�Ƃ��̎q�m�[�h�̃��[���h�s��X�V
    //    GetNodes()->at(nodeIndex).rotation_ = headRotate;

    
        //UpdateWorldTransforms(nodeIndex);
    //    UpdateWorldTransforms(nodeIndex, HeadPosition);
    }
#endif
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