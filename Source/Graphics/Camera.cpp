#include "Camera.h"
#include "../Core/Application.h"

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"

// ----- ������ -----
void Camera::Initialize()
{
    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));
}

// ----- �X�V -----
void Camera::Update(const float& elapsedTime)
{
    if (GetAsyncKeyState('H') & 1) 
        ScreenVibrate(0.1f, 2.0f);

    // --- ��ʐU�� ---
    ScreenVibrationUpdate(elapsedTime);

    // --- �J������]���� ---
    Rotate(elapsedTime);
}

void Camera::SetPerspectiveFov()
{
    DirectX::XMFLOAT3 front = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rot   = GetTransform()->GetRotation();
    view_.eye_.x = target_.x + front.x - length_ * cosf(rot.x) * sinf(rot.y);
    view_.eye_.y = target_.y + front.y - length_ * sinf(rot.x);
    view_.eye_.z = target_.z + front.z - length_ * cosf(rot.x) * cosf(rot.y);
    view_.focus_ = target_;

    // --- projectionMatrix �ݒ� ---
    float aspectRatio = SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    float fov = DirectX::XMConvertToRadians(fov_);
    projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ_, farZ_);

    // --- viewMatrix �ݒ� ---
    DirectX::XMVECTOR eye, focus, up;

    eye     = DirectX::XMVectorSet(view_.eye_.x, view_.eye_.y, view_.eye_.z, 1.0f);
    focus   = DirectX::XMVectorSet(view_.focus_.x, view_.focus_.y, view_.focus_.z, 1.0f);
    focus   = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&screenVibrationOffset_), focus);
    up      = DirectX::XMVectorSet(view_.up_.x, view_.up_.y, view_.up_.z, 0.0f);
    
    viewMatrix_ = DirectX::XMMatrixLookAtLH(eye, focus, up);
}

void Camera::DrawDebug()
{
    if (ImGui::TreeNode("Camera"))
    {
        ImGui::DragFloat("length", &length_, 0.01f);
        ImGui::DragFloat("fov", &fov_, 0.01f);
        ImGui::DragFloat("inputThreshold", &inputThreshold_, 0.1f, 0.0f, 1.0f);
        
        float minXRotation = DirectX::XMConvertToDegrees(minXRotation_);
        float maxXRotation = DirectX::XMConvertToDegrees(maxXRotation_);
        ImGui::DragFloat("minXRotation", &minXRotation, 1.0f, -75.0f, -30.0f);
        ImGui::DragFloat("maxXRotation", &maxXRotation, 1.0f, -20.0f, 0.0f);
        minXRotation_ = DirectX::XMConvertToRadians(minXRotation);
        maxXRotation_ = DirectX::XMConvertToRadians(maxXRotation);

        transform_.DrawDebug();

        ImGui::TreePop();
    }
}

// ----- ��]���� -----
void Camera::Rotate(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    // �R���g���[���[�̌X�������ȏザ��Ȃ��Ɣ�������Ȃ�
    // ( ����X�g���X�y���̂��ߐ�����݂��� )
    // ���ړ��������̂ɏ㉺�ړ����Ă��܂�Ȃ��悤��...
    if (fabs(aRy) >= inputThreshold_)
    {
        rotate.x -= aRy * elapsedTime;
    }
    // ���ړ��͐����Ȃ�
    rotate.y += aRx * elapsedTime;

    // X��(�㉺)�̉�]����
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;
    if (rotate.x > maxXRotation_) rotate.x = maxXRotation_;

    // Y����]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (rotate.y < -DirectX::XM_PI) rotate.y += DirectX::XM_2PI;
    if (rotate.y >  DirectX::XM_PI) rotate.y -= DirectX::XM_2PI;

    GetTransform()->SetRotation(rotate);
}

// ----- ��ʐU�� (�U����, �U������) �ݒ� -----
void Camera::ScreenVibrate(const float& vibrationVolume/*�U����*/, const float& vibrationTime/*�U������*/)
{
    vibrationVolume_ = vibrationVolume;
    vibrationTimer_ = vibrationTime; 
    vibrationTime_ = vibrationTime;
}

// ----- ��ʐU�� �X�V -----
void Camera::ScreenVibrationUpdate(const float& elapsedTime)
{
    // �U���l���Z�b�g
    screenVibrationOffset_ = {};

    // �U�����Ȃ���Ώ������Ȃ�
    if (vibrationTimer_ <= 0) return;

    // �U�������̎w��(����)
    DirectX::XMFLOAT3 vibVec;
    DirectX::XMFLOAT3 right = GetTransform()->CalcRight();
    DirectX::XMFLOAT3 up    = GetTransform()->CalcUp();

    right = right * (rand() % 100 - 50.0f);
    up = up * (rand() % 100 - 50.0f);

    vibVec = { right.x + up.x, right.y + up.y, 0.0f };
    vibVec = XMFloat3Normalize(vibVec);

    // �C�[�W���O���g���o�ߎ��ԂŐU���ʂ𒲐�����
    float vibrationVolume = Easing::InSine(vibrationTimer_, vibrationTime_, vibrationVolume_, 0.0f);

    // �U���l������
    screenVibrationOffset_ = vibVec * vibrationVolume;

    vibrationTimer_ -= elapsedTime;
}

// ----- �O�x�N�g���擾 -----
const DirectX::XMFLOAT3 Camera::CalcForward()
{
    return XMFloat3Normalize(view_.focus_ - view_.eye_);
}

// ----- �E�x�N�g���擾 -----
const DirectX::XMFLOAT3 Camera::CalcRight()
{
    DirectX::XMFLOAT3 forward = CalcForward();
    DirectX::XMFLOAT3 up = { 0, 1, 0 };
    return XMFloat3Cross(up, forward);
}
