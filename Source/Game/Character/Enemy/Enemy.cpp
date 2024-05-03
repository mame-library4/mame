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

    // ��]����
    if (cross > 0)
    {
        GetTransform()->AddRotationY(dot);
    }
    else
    {
        GetTransform()->AddRotationY(-dot);
    }
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
const float Enemy::CalcPlayerDistance()
{
    DirectX::XMFLOAT3 ownerPos = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();

    return XMFloat3Length(playerPos - ownerPos);
}
