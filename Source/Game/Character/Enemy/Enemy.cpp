#include "Enemy.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"
#include "UI/UIPartDestruction.h"

// ----- �R���X�g���N�^ -----
Enemy::Enemy(const std::string& filename, const float& scaleFactor)
    : Character(filename, scaleFactor)
{
}

// ----- ��]���� -----
void Enemy::Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos)
{
    DirectX::XMFLOAT2 ownerFront = { GetTransform()->CalcForward().x , GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 target = { targetPos.x , targetPos.z };
    DirectX::XMFLOAT2 vec = XMFloat2Normalize(target - ownerPos);

    ownerFront = XMFloat2Normalize(ownerFront);

    // �O�ςłǂ����ɉ�]���邩���f
    float cross = XMFloat2Cross(vec, ownerFront);

    // ���ςŉ�]�����Z�o
    float dot = XMFloat2Dot(vec, ownerFront);
    float angle = acosf(dot);

    if (angle < DirectX::XMConvertToRadians(1)) return;

    const float speed = GetRotateSpeed() * elapsedTime;
    angle *= speed;

    // ��]����
    if (cross > 0)
    {
        GetTransform()->AddRotationY(-angle);
    }
    else
    {
        GetTransform()->AddRotationY(angle);
    }
}

// ----- �v���C���[��T�� ( �퓬��ԂɈڍs�ł��邩 ) -----
const bool Enemy::SearchPlayer()
{
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };

    DirectX::XMFLOAT2 vec = ownerPos - playerPos;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);

    // �ߋ����U���͈͈ȓ��ɂ���̂ŋ����I�ɐ퓬��Ԃɂ���
    if (dist < nearAttackRadius_) return true;

    // �퓬�͈͂ɂ���
    if (dist < battleRadius_)
    {
        // �P�ʃx�N�g����
        vec = vec / dist;
        
        // �����x�N�g����
        DirectX::XMFLOAT2 frontVec = { GetTransform()->CalcForward().x, GetTransform()->CalcForward().z };
        frontVec = XMFloat2Normalize(frontVec);

        // �O�㔻��
        float dot = XMFloat2Dot(frontVec, vec);
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;
}

// ----- �m�[�h�X�V -----
void Enemy::UpdateNode(const float& elapsedTime)
{
    // ���ݎ��s����m�[�h������΁A�r�w�C�r�A�c���[����m�[�h�����s
    if (activeNode_ != nullptr)
    {
        activeNode_ = behaviorTree_->Run(activeNode_, behaviorData_.get(), elapsedTime);
    }
    // ���ݎ��s����Ă���m�[�h���Ȃ���΁A���Ɏ��s����m�[�h�𐄘_����
    else
    {
        activeNode_ = behaviorTree_->ActiveNodeInference(behaviorData_.get());
    }
}

// ----- �v���C���[�܂ł̋������Z�o -----
const float Enemy::CalcDistanceToPlayer()
{
    DirectX::XMFLOAT3 ownerPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();

    return XMFloat3Length(playerPos - ownerPos);
}

// ----- �������g����v���C���[�ւ̃x�N�g�� -----
const DirectX::XMFLOAT3 Enemy::CalcDirectionToPlayer()
{
    DirectX::XMFLOAT3 ownerPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();

    return playerPos - ownerPos;
}

// ----- �_���[�W���� -----
void Enemy::AddDamage(const float& damage, const int& dataIndex)
{
    // �_���[�W���� ( �̗͂��_���[�W�������� )
    Character::AddDamage(damage);

    // ���ʂ̗̑͂��_���[�W������
    AddDamagePart(damage, dataIndex);

    // ���ʔj�󔻒�
    CheckPartDestruction();
}

void Enemy::SetHitStop()
{
    hitStopFrame_ = normalHitStopFrame_;
    currentHitStopFrame_ = 0;
    isHitStopActive_ = true;
}

// ----- ���ʔj�󔻒� -----
void Enemy::CheckPartDestruction()
{
    for (int partIndex = 0; partIndex < static_cast<int>(PartName::Max); ++partIndex)
    {
        // ���ʔj�󂪂���Ă��Ȃ�
        if (isPartDestruction_[partIndex] == false)
        {
            // ���ʂ̗̑͂��Ȃ��Ȃ�����UI�𐶐����ăt���O�𗧂Ă�
            if (partHealth_[partIndex] <= 0.0f)
            {
                // ���ʔj��UI����
                UIPartDestruction* ui = new UIPartDestruction();

                isPartDestruction_[partIndex] = true;
            }
        }
    }
}
