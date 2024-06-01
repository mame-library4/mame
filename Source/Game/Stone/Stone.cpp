#include "Stone.h"
#include "../Other/MathHelper.h"

// ----- �R���X�g���N�^ -----
Stone::Stone(const std::string filename)
    : Object(filename)
{
}

// ----- �f�X�g���N�^ -----
Stone::~Stone()
{
}

// ----- ������ -----
void Stone::Initialize(const DirectX::XMFLOAT3& basePosition)
{
    DirectX::XMFLOAT3 pos = basePosition;
    
    GetTransform()->SetScaleFactor(rand() % 11 / 10.0f + 0.7f);

    // �n�ʂɉB��Ă����Ăق����̂� Y �� -0.3
    pos.y = -0.2f;

    // XZ ���ʂɃ����_���Ȉʒu�ɐ���
    pos.x += (rand() % 50 - 25) / 10.0f;
    pos.z += (rand() % 50 - 25) / 10.0f;

    GetTransform()->SetPosition(pos);

    // �ړ�������ݒ�
    moveDirection_ = XMFloat3Normalize(pos - basePosition);

    // �ړ����x��ݒ�
    moveSpeed_ = rand() % 3 + 1;

    // �㏸���x��ݒ�
    //ascendSpeed_ = 20.0f;
    ascendSpeed_ = std::min(20.0f, 20.0f * ( 2.1f - GetTransform()->GetScaleFactor()));
    //ascendSpeed_ = (rand() % 11 + 1) / 10.0f * 20.0f;

    // ������]�l��ݒ�
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();
    rotate.x = DirectX::XMConvertToRadians(rand() % 180);
    rotate.y = DirectX::XMConvertToRadians(rand() % 180);
    rotate.z = DirectX::XMConvertToRadians(rand() % 180);

    // ��]����ݒ�
    turnRate_.x = DirectX::XMConvertToRadians(rand() % 20);
    turnRate_.y = DirectX::XMConvertToRadians(rand() % 20);
    turnRate_.z = DirectX::XMConvertToRadians(rand() % 20);

    // ��]���x��ݒ�
    turnSpeed_ = 7.0f;
}

// ----- �X�V -----
void Stone::Update(const float& elapsedTime)
{
    if (GetTransform()->GetPositionY() < -10.0f)
    {
        GetTransform()->SetScaleFactor(0.0f);
    }

    // Y ���ړ�����
    float gravity = 9.8f * elapsedTime;
    ascendSpeed_ -= gravity * 3.5f;
    GetTransform()->AddPositionY(ascendSpeed_ * elapsedTime);

    // XZ���ʈړ�����
    DirectX::XMFLOAT3 moveVec = moveDirection_ * moveSpeed_ * elapsedTime;
    moveVec.y = 0;
    GetTransform()->AddPosition(moveVec);

    // ��]����
    GetTransform()->AddRotationX(turnRate_.x * turnSpeed_ * elapsedTime);
    GetTransform()->AddRotationY(turnRate_.y * turnSpeed_ * elapsedTime);
    GetTransform()->AddRotationZ(turnRate_.z * turnSpeed_ * elapsedTime);
}

// ----- �`�� -----
void Stone::Render(const float& scaleFacter)
{
    Object::Render(scaleFacter);
}

// ----- ImGui�p -----
void Stone::DrawDebug()
{

}
