#include "Enemy.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

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
    float dot = XMFloat2Dot(vec, ownerFront) - 1.0f;

    // ��]�ʂ����Ȃ���΂����ŏI��
    if (dot > -0.005f) return;

    // ��]���x�ݒ�
    float rotateSpeed = GetRotateSpeed() * elapsedTime;
    float rotateY = dot * rotateSpeed;
    rotateY = std::min(rotateY, -0.7f * rotateSpeed);

    // ��]����
    if (cross > 0)
    {
        GetTransform()->AddRotationY(rotateY);
    }
    else
    {
        GetTransform()->AddRotationY(-rotateY);
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

