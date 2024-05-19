#include "Enemy.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- �R���X�g���N�^ -----
Enemy::Enemy(std::string filename)
    : Character(filename)
{
}

// ----- ��]���� -----
void Enemy::Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos)
{
    DirectX::XMFLOAT2 ownerFront = { GetTransform()->CalcForward().x , GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { GetTransform()->GetPositionX(), GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 target = { targetPos.x , targetPos.z };
    DirectX::XMFLOAT2 vec = XMFloat2Normalize(target - ownerPos);

    // �O�ςłǂ����ɉ�]���邩���f
    float cross = XMFloat2Cross(vec, ownerFront);

    // ���ςŉ�]�����Z�o
    ownerFront = XMFloat2Normalize(ownerFront);
    float dot = XMFloat2Dot(vec, ownerFront) - 1.0f;

    // ��]���x�ݒ�
    float rotateSpeed = GetRotateSpeed() * elapsedTime;

    // ��]����
    if (cross > 0)
    {
        GetTransform()->AddRotationY(dot * rotateSpeed);
    }
    else
    {
        GetTransform()->AddRotationY(-dot * rotateSpeed);
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
    if (dist < farAttackRadius_) return true;

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

// ----- �u�����h�A�j���[�V�����ݒ� -----
bool Enemy::PlayBlendAnimation(const TamamoAnimation& index, const bool& loop, const float& speed)
{
    const int currentAnimationIndex = GetCurrentBlendAnimationIndex();

    // ���݂̃A�j���[�V�����ƈ����̃A�j���[�V�����������ꍇ
    if (currentAnimationIndex == static_cast<int>(index))
    {
        // �΂ߕ����̎�����������x�A�j���[�V�����Đ����Ăق����Ȃ��̂ŏ������΂�
        if (index == TamamoAnimation::WalkLeft ||
            index == TamamoAnimation::WalkRight)
        {
            return false;
        }

        // ����walk����ڂ̃u�����h�����ɓ���Ă���
        Object::PlayBlendAnimation(static_cast<int>(Enemy::TamamoAnimation::Walk), static_cast<int>(index), loop, speed);
        SetWeight(1.0f);
        return true;
    }

    // �U���A�j���[�V�����̎���weight�l���P�ɂ���
    if (currentAnimationIndex != static_cast<int>(TamamoAnimation::Idle) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::Walk) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::WalkLeft) &&
        currentAnimationIndex != static_cast<int>(TamamoAnimation::WalkRight))
    {
        SetWeight(1.0f);
    }

    Object::PlayBlendAnimation(static_cast<int>(index), loop, speed);
    return true;
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
