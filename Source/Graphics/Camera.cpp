#include "Camera.h"
#include "../Core/Application.h"

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"

#include "SceneManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Player/PlayerManager.h"

// ----- ������ -----
void Camera::Initialize()
{
    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    //GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));

    // TODO:���߂�
    // ��������e�X�g���Ă邾���Ȃ̂ŕς��鑽��
    targetOffset_ = { 0, -1, 0 };
    cameraOffset_ = { 0, 2.5f, 0 };
    length_ = 6.0f;
    verticalRotationSpeed_ = 1.0f;
    horizontalRotationSpeed_ = 4.0f;
}

// ----- �X�V -----
void Camera::Update(const float& elapsedTime)
{
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title) return;

    // ���S�J�����g�p���͂����ŏI��
    if (useDeathCamera_) return;

    const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    Camera::Instance().SetTarget(cameraTargetPosition);

    // �h���S���̏㏸�U���̃J�����X�V
    if (UpdateRiseAttackCamera(elapsedTime)) return;

    // --- ��ʐU�� ---
    ScreenVibrationUpdate(elapsedTime);

    // --- �J������]���� ---
    Rotate(elapsedTime);

}

void Camera::SetPerspectiveFov()
{
    DirectX::XMFLOAT3 front = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rot   = GetTransform()->GetRotation();
    view_.eye_.x = target_.x + cameraOffset_.x + front.x - length_ * cosf(rot.x) * sinf(rot.y);
    view_.eye_.y = target_.y + cameraOffset_.y + front.y - length_ * sinf(rot.x);
    view_.eye_.z = target_.z + cameraOffset_.z + front.z - length_ * cosf(rot.x) * cosf(rot.y);
    view_.focus_ = target_ + cameraOffset_ + targetOffset_;

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
    if (ImGui::BeginMenu("Camera"))
    {
        ImGui::Checkbox("InvertVertical", &invertVertical_);

        ImGui::DragFloat("Length", &length_, 0.01f);
        ImGui::DragFloat("MinLength", &minLength_, 0.01f);
        ImGui::DragFloat("MaxLength", &maxLength_, 0.01f);

        ImGui::DragFloat("Fov", &fov_, 0.01f);
        ImGui::DragFloat("InputThreshold", &inputThreshold_, 0.1f, 0.0f, 1.0f);
        
        ImGui::DragFloat3("FocusOffset", &targetOffset_.x);
        ImGui::DragFloat3("CameraOffset", &cameraOffset_.x);

        ImGui::DragFloat("VerticalRotationSpeed", &verticalRotationSpeed_);
        ImGui::DragFloat("HorizontalRotationSpeed", &horizontalRotationSpeed_);

        float minXRotation = DirectX::XMConvertToDegrees(minXRotation_);
        float maxXRotation = DirectX::XMConvertToDegrees(maxXRotation_);
        ImGui::DragFloat("MinXRotation", &minXRotation, 1.0f, -75.0f, -20.0f);
        ImGui::DragFloat("MaxXRotation", &maxXRotation, 1.0f, -20.0f, 20.0f);
        minXRotation_ = DirectX::XMConvertToRadians(minXRotation);
        maxXRotation_ = DirectX::XMConvertToRadians(maxXRotation);

        transform_.DrawDebug();

        ImGui::EndMenu();
    }
}

// ----- ��]���� -----
void Camera::Rotate(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    const float invertVertical = invertVertical_ ? -1 : 1;

    // �R���g���[���[�̌X�������ȏザ��Ȃ��Ɣ�������Ȃ�
    // ( ����X�g���X�y���̂��ߐ�����݂��� )
    // ���ړ��������̂ɏ㉺�ړ����Ă��܂�Ȃ��悤��...
    //if (fabs(aRy) >= inputThreshold_)
    //{
    //    rotate.x -= aRy * invertVertical * verticalRotationSpeed_ * elapsedTime;
    //}
    // ���ړ��͐����Ȃ�
    rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

    // length����
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;
    //length_ += addLength * aRy * elapsedTime;
    //if (length_ < minLength_) length_ = minLength_;
    if (length_ > maxLength_) length_ = maxLength_;

    // X��(�㉺)�̉�]����
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;
    //if (rotate.x > maxXRotation_) rotate.x = maxXRotation_;

    
    if (fabs(aRy) >= inputThreshold_)
    {
        if (rotate.x > maxXRotation_)
        {
            constexpr float maxRotate = DirectX::XMConvertToRadians(40);
            const float addAmountSpeed = 2.0f;
            const float rotateAmount = (maxRotate - maxXRotation_) * addAmountSpeed;
            const float lengthAmount = (minLength_ - 2.5f) * addAmountSpeed;

            rotate.x -= aRy * invertVertical * rotateAmount * elapsedTime;

            if (rotate.x > maxRotate) rotate.x = maxRotate;

            length_ += aRy * lengthAmount * elapsedTime;
            if (length_ < 2.5f) length_ = 2.5f;
        }
        else
        {
            rotate.x -= aRy * invertVertical * verticalRotationSpeed_ * elapsedTime;

            length_ += addLength * aRy * elapsedTime;
        }
    }


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

// ----- �h���S���㏸�U�����̃J���� -----
const bool Camera::UpdateRiseAttackCamera(const float& elapsedTime)
{
    const float maxLength = 10.0f;
    const float totalFrame = 0.4f;
    const float totalFrame1= 0.7f;
    constexpr float maxRotate = DirectX::XMConvertToRadians(-10.0f);
    //constexpr float maxRotate = DirectX::XMConvertToRadians(-3.0f);
    //constexpr float maxRotate = DirectX::XMConvertToRadians(10.0f);
    float rotateX = 0.0f;
    bool returnFlag = false;

    const float maxOffsetX = 4.0f;
    const float minOffsetX = 2.5f;

    switch (riseAttackState_)
    {
    case 0:// ������
        riseAttackEasingTimer_ = 0.0f;
        oldCameraLength_ = length_;
        oldRotateX_ = GetTransform()->GetRotationX();
        riseAttackState_ = 1;
        returnFlag = true;
        break;
    case 1:
    {
        length_ = Easing::OutSine(riseAttackEasingTimer_, totalFrame, maxLength, oldCameraLength_);
        rotateX = Easing::OutSine(riseAttackEasingTimer_, totalFrame, maxRotate, oldRotateX_);
        //cameraOffset_.y = Easing::InSine(riseAttackEasingTimer_, totalFrame, maxOffsetX, minOffsetX);

        riseAttackEasingTimer_ += elapsedTime;
        if (riseAttackEasingTimer_ > totalFrame)
        {
            length_ = maxLength;
            rotateX = maxRotate;
            //cameraOffset_.y = maxOffsetX;

            riseAttackState_ = 2;
            riseAttackEasingTimer_ = 0.0f;
        }

        returnFlag = true;
        break;
    }
    // �ҋ@... �������Ȃ� 
    case 2:   
        returnFlag = true;
        break;
    // ���g�p
    case 5:   
        returnFlag = false;
        break;
    // RiseAttack�I������
    default:
    {
        const float resetLength = 6.15f;
        length_ = Easing::InSine(riseAttackEasingTimer_, totalFrame1, resetLength, maxLength);
        rotateX = Easing::InSine(riseAttackEasingTimer_, totalFrame1, DirectX::XMConvertToRadians(7.0f), maxRotate);
        //cameraOffset_.y = Easing::InSine(riseAttackEasingTimer_, totalFrame, minOffsetX, maxOffsetX);

        riseAttackEasingTimer_ += elapsedTime;
        if (riseAttackEasingTimer_ > totalFrame1)
        {
            //length_ = oldCameraLength_;
            length_ = resetLength;
            rotateX = DirectX::XMConvertToRadians(7.0f);
            //cameraOffset_.y = minOffsetX;

            riseAttackEasingTimer_ = 0.0f;
            riseAttackState_ = 5;// ���g�p�ɂ���
        }
        returnFlag = true;
    }
        break;
    }

    // ��]����
    if (returnFlag == true)
    {
        GamePad& gamePad = Input::Instance().GetGamePad();
        float aRx = gamePad.GetAxisRX();
        DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();
                
        rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

        // Y����]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
        if (rotate.y < -DirectX::XM_PI) rotate.y += DirectX::XM_2PI;
        if (rotate.y > DirectX::XM_PI) rotate.y -= DirectX::XM_2PI;

        if (rotateX != 0.0f) rotate.x = rotateX;

        GetTransform()->SetRotation(rotate);
    }

    return returnFlag;
}
