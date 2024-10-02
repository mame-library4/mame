#include "Camera.h"
#include "../Core/Application.h"

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"

#include "SceneManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Player/PlayerManager.h"

#include "UI/UIManager.h"
#include "UI/UICrosshair.h"

#include "GameScene.h"

// ----- ������ -----
void Camera::Initialize()
{
    useEnemyDeathCamera_ = false;

    enemyDeathstate_ = 0;
    playerDeathState_ = 0;
    counterState_ = 0;


    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    //GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));

    // TODO:���߂�
    // ��������e�X�g���Ă邾���Ȃ̂ŕς��鑽��
    targetOffset_ = { 0, -1, 0 };
    cameraOffset_ = { 0, 2.5f, 0 };
    length_ = 6.0f;
    verticalRotationSpeed_ = 1.0f;
    horizontalRotationSpeed_ = 4.0f;

    lerpTimer_ = 0.0f;

    // ----- ���b�N�I���J���� -----
    targetJointName_.emplace_back("Dragon15_head");
    targetJointName_.emplace_back("Dragon15_spine2");
    targetJointName_.emplace_back("Dragon15_tail_02");
    currentTargetJointIndex_ = 0;
    useLockonCamera_ = false;
}

// ----- �X�V -----
void Camera::Update(const float& elapsedTime)
{
    // �^�C�g����ʂ͂Ƃ肠���������ŏI��
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title) return;

    // ���S�J�����g�p���͂����ŏI��
    if (UpdatePlayerDeathCamera(elapsedTime)) return; // Player���S�J����
    if (UpdateEnemyDeathCamera(elapsedTime))  return; // Enemy���S�J����

    const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    
#if 1
    if (isCounterDelay_ == false) target_ = cameraTargetPosition;

#else
    if (lerpTimer_ >= 1.0f)
    {
        target_ = cameraTargetPosition;
    }
    else
    {
        target_ = XMFloat3Lerp(target_, cameraTargetPosition, lerpTimer_);
    }

    GamePad& gamePad = Input::Instance().GetGamePad();
    const float aLx = gamePad.GetAxisLX();
    const float aLy = gamePad.GetAxisLY();
    if (aLx == 0.0f && aLy == 0.0f)
    {
        lerpTimer_ += elapsedTime * 0.05;
    
        lerpTimer_ = min(lerpTimer_, 1.0f);
    }
    else
    {
        lerpTimer_ = 0.05f;
    }
#endif

    // �h���S���̏㏸�U���̃J�����X�V
    //if (UpdateRiseAttackCamera(elapsedTime)) return;

    // �J�E���^�[�U���̃J�����X�V
    if (UpdateCounterAttackCamera(elapsedTime)) return;


    // ���b�N�I���J����
    UpdateLockonCamera(elapsedTime);

    // �J�������Z�b�g
    UpdateCameraReset(elapsedTime);

    // �J������]����
    Rotate(elapsedTime);

    // --- ��ʐU�� ---
    ScreenVibrationUpdate(elapsedTime);

}

void Camera::SetPerspectiveFov()
{
    DirectX::XMFLOAT3 front = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rot = GetTransform()->GetRotation();
    view_.eye_.x = target_.x + cameraOffset_.x + front.x - length_ * cosf(rot.x) * sinf(rot.y);
    view_.eye_.y = target_.y + cameraOffset_.y + front.y - length_ * sinf(rot.x);
    view_.eye_.z = target_.z + cameraOffset_.z + front.z - length_ * cosf(rot.x) * cosf(rot.y);
    view_.focus_ = target_ + cameraOffset_ + targetOffset_;

    GetTransform()->SetPosition(view_.eye_);

    // ----- �J�������X�e�[�W���Ɏ��߂� -----
    if(SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Game)
    {
#if 1 
        const float radius = 29.8f;
        DirectX::XMFLOAT3 cameraPosition = GetTransform()->GetPosition();
        DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
        playerPosition.y = cameraPosition.y;
        DirectX::XMFLOAT3 direction = XMFloat3Normalize(playerPosition - cameraPosition);

        DirectX::XMFLOAT3 stageCenter = GameScene::stageCenter_;
        stageCenter.y = cameraPosition.y;
        DirectX::XMFLOAT3 stageToCamera = stageCenter - cameraPosition;

        float length = XMFloat3Length(stageToCamera);
        if (length > radius)
        {

            float a = XMFloat3Dot(direction, direction);
            float b = -2 * XMFloat3Dot(stageToCamera, direction);
            float c = XMFloat3Dot(stageToCamera, stageToCamera) - radius * radius;

            float root = b * b - 4 * a * c;

            float root0 = (-b - sqrt(root)) / (2 * a);

            DirectX::XMFLOAT3 d = direction * root0;
            view_.eye_ = cameraPosition + d;
        }
#endif
    }

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
        if (ImGui::TreeNode("LockonCamera"))
        {
            ImGui::BulletText(targetJointName_.at(currentTargetJointIndex_).c_str());
            ImGui::DragInt("TargetJointIndex", &currentTargetJointIndex_, 1, 0, 2);
            ImGui::DragFloat("LockonInputThreshold", &lockonInputThreshold_, 0.1f, 0.1f, 1.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("ResetCamera"))
        {
            DirectX::XMFLOAT2 oldRotation = {};
            oldRotation.x = DirectX::XMConvertToDegrees(resetOldRotation_.x);
            oldRotation.y = DirectX::XMConvertToDegrees(resetOldRotation_.y);
            ImGui::DragFloat2("OldRotation", &oldRotation.x);
            resetOldRotation_.x = DirectX::XMConvertToRadians(oldRotation.x);
            resetOldRotation_.y = DirectX::XMConvertToRadians(oldRotation.y);
            
            DirectX::XMFLOAT2 targetRotation = {};
            targetRotation.x = DirectX::XMConvertToDegrees(resetTargetRotation_.x);
            targetRotation.y = DirectX::XMConvertToDegrees(resetTargetRotation_.y);
            ImGui::DragFloat2("TargetRotation", &targetRotation.x);
            resetTargetRotation_.x = DirectX::XMConvertToRadians(targetRotation.x);
            resetTargetRotation_.y = DirectX::XMConvertToRadians(targetRotation.y);
            
            
            ImGui::DragFloat("ResetLerpTimer", &resetLerpTimer_);
            ImGui::DragFloat("ResetLerpSpeed", &resetLerpSpeed_);

            ImGui::TreePop();
        }

        ImGui::Checkbox("InvertVertical", &invertVertical_);

        ImGui::DragFloat("LerpTimer", &lerpTimer_);

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
    if (useLockonCamera_) return;

    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    // ���ړ��͐����Ȃ�
    rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

    // length����
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;    

    // ���s�s�ȏc��]�����Ȃ��悤��臒l�ȏ�Ȃ���͂��擾����
    if (fabs(aRy) >= inputThreshold_)
    {
        // �㉺���͔��]�𔻒�
        const float aRyValue = invertVertical_ ? aRy * -1 * elapsedTime : aRy * elapsedTime;

        if (rotate.x > maxXRotation_)
        {
            isAdjustCameraLength_ = true;

            constexpr float maxRotate = DirectX::XMConvertToRadians(40);
            const float addAmountSpeed = 2.0f;
            const float rotateAmount = (maxRotate - maxXRotation_) * addAmountSpeed;
            const float lengthAmount = (minLength_ - 2.5f) * addAmountSpeed;

            rotate.x -= rotateAmount * aRyValue;

            if (rotate.x > maxRotate) rotate.x = maxRotate;

            length_ += lengthAmount * aRyValue;
            if (length_ < 2.5f) length_ = 2.5f;
        }
        else
        {
            isAdjustCameraLength_ = false;

            rotate.x -= verticalRotationSpeed_ * aRyValue;

            length_ += addLength * aRyValue;
        }
    }

    if (length_ > maxLength_) length_ = maxLength_;

    // X��(�㉺)�̉�]����
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;

    // Y����]�l�� 0.0f ~ XM_2PI �Ɏ��܂�悤�ɂ���
    if (rotate.y > DirectX::XM_2PI) rotate.y -= DirectX::XM_2PI;
    if (rotate.y < 0.0f)            rotate.y += DirectX::XM_2PI;


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

// ---- ���@���S���J�������g�p���� -----
void Camera::SetUsePlayerDeathCmaera(const float& flag)
{
    usePlayerDeathCamera_ = flag;
    playerDeathState_ = 0;
}

// ----- �G���S���J�����g�p���� -----
void Camera::SetUseEnemyDeathCamera()
{
    useEnemyDeathCamera_ = true;
    enemyDeathstate_ = 0;
}

// ----- �J�E���^�[���J�������g�p���� -----
void Camera::SetUseCounterCamera()
{
    useCounterCamera_ = true;
    counterState_ = 0;
}

// ----- ���b�N�I���J�����X�V -----
void Camera::UpdateLockonCamera(const float& elapsedTime)
{
    // ���b�N�I�����͔���
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_THUMB)
    {
        // ���݃��b�N�I�����Ă��Ȃ��ꍇ
        if (useLockonCamera_ == false)
        {
            // �N���X�w�AUI�𐶐�����
            UICrosshair* uiCrosshair = new UICrosshair();

            // ���b�N�I������W���C���g��ύX�\�ɂ���
            isNextJointAccessible = true;
        }
        // ���݃��b�N�I�����Ă���ꍇ
        else
        {
            // ���ݎg�p���Ă���N���X�w�AUI���폜����
            UIManager::Instance().Remove(UIManager::UIType::UICrosshair);
        }

        useLockonCamera_ = !useLockonCamera_;
    }

    // ���b�N�I�����Ă��Ȃ��̂ł����ŏI��
    if (useLockonCamera_ == false) return;

    // ���b�N�I������W���C���g�̐؂�ւ�����
    const float aRx = Input::Instance().GetGamePad().GetAxisRX();
    // �W���C���g��؂�ւ����ꍇ
    if (isNextJointAccessible)
    {
        if (aRx > lockonInputThreshold_)
        {
            if (currentTargetJointIndex_ > 0) --currentTargetJointIndex_;
            else currentTargetJointIndex_ = 2;

            isNextJointAccessible = false;
        }
        else if (aRx < -lockonInputThreshold_)
        {
            if (currentTargetJointIndex_ < 2) ++currentTargetJointIndex_;
            else currentTargetJointIndex_ = 0;

            isNextJointAccessible = false;
        }
    }
    // �W���C���g��؂�ւ���Ȃ��ꍇ
    else
    {
        // ���͂��Ȃ��Ȃ����瑼�̃W���C���g�փA�N�Z�X�ł���悤�ɂȂ�
        if (aRx == 0.0f) isNextJointAccessible = true;
    }


    // �v���C���[�ƓG�̃W���C���g�̊Ԃ�target�ɐݒ肷��
    // TODO: �����݂ł��Ă��Ȃ�

    DirectX::XMFLOAT3 cameraPosition = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(GetCurrentTargetJointName());

    {
        // XZ���ʂł̏���
        DirectX::XMFLOAT3 cameraToEnemy = targetPosition - cameraPosition;
        DirectX::XMFLOAT3 playerToEnemy = targetPosition - playerPosition;

        DirectX::XMFLOAT2 vec0 = XMFloat2Normalize({ cameraToEnemy.x, cameraToEnemy.z });
        DirectX::XMFLOAT2 vec1 = XMFloat2Normalize({ playerToEnemy.x, playerToEnemy.z });

        const float angle = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenVectors(DirectX::XMLoadFloat2(&vec0), DirectX::XMLoadFloat2(&vec1)));

        float cross = XMFloat2Cross(vec0, vec1);

        if (cross < 0)
        {
            GetTransform()->AddRotationY(-angle);
        }
        else
        {
            GetTransform()->AddRotationY(angle);
        }
    }

    {
        GetTransform()->SetRotationX(DirectX::XMConvertToRadians(10.0f));

        // YZ���ʂł̏���
        //vec0 = XMFloat2Normalize({ cameraToEnemy.y, cameraToEnemy.z });
        //vec1 = XMFloat2Normalize({ playerToEnemy.y, playerToEnemy.z });

        //dot = XMFloat2Dot(vec0, vec1) - 1.0f;

        //cross = XMFloat2Cross(vec0, vec1);

        //if (cross < 0)
        //{
        //    GetTransform()->SetRotationX(dot);
        //}
        //else
        //{
        //    GetTransform()->SetRotationX(-dot);
        //}
    }
}

// ----- �J�������Z�b�g�X�V -----
void Camera::UpdateCameraReset(const float& elapsedTime)
{
    // ���b�N�I���J�������g�p���Ă���̂ŃJ�������Z�b�g�͎g���Ȃ�
    if (useLockonCamera_) return;

    // �J�������Z�b�g���͔���
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
    {
        // ----- �v���C���[�̌����Ƀ��Z�b�g���� -----
        
        // ���݂̉�]�l��ۑ� & ��]�̖ړI�n��ݒ�
        resetOldRotation_ = { GetTransform()->GetRotationX(), GetTransform()->GetRotationY() };
        resetTargetRotation_ = { DirectX::XMConvertToRadians(10.0f), PlayerManager::Instance().GetTransform()->GetRotationY() };

        // ��]�p������ꍇ�͏���������
        const float rotationDeltaY = resetTargetRotation_.y - resetOldRotation_.y;
        if (rotationDeltaY != 0.0f)
        {
            // ��]�p��180�x�ȏ�̏ꍇ�� 180�x�Ɏ��܂�悤�ɂ���
            if (rotationDeltaY > DirectX::XM_PI)  resetTargetRotation_.y -= DirectX::XM_2PI;
            if (rotationDeltaY < -DirectX::XM_PI) resetTargetRotation_.y += DirectX::XM_2PI;

            resetLerpTimer_ = 0.0f;
            cameraResetFlag_ = true;
        }
    }

    // �t���O�������Ă��Ȃ��̂ł����ŏI��
    if (cameraResetFlag_ == false) return;

    // Lerp�������Ă��������ɉ�]������
    resetLerpTimer_ += resetLerpSpeed_ * elapsedTime;
    resetLerpTimer_ = min(resetLerpTimer_, 1.0f);

    const float rotationX = XMFloatLerp(resetOldRotation_.x, resetTargetRotation_.x, resetLerpTimer_);
    const float rotationY = XMFloatLerp(resetOldRotation_.y, resetTargetRotation_.y, resetLerpTimer_);

    GetTransform()->SetRotationX(rotationX);
    GetTransform()->SetRotationY(rotationY);

    // �I���m�F
    if (resetLerpTimer_ == 1.0f) cameraResetFlag_ = false;
}

// ----- ���@���S�J���� -----
const bool Camera::UpdatePlayerDeathCamera(const float& elapsedTime)
{
    // ���@���S���J�����̎g�p�t���O�������Ă��Ȃ��̂ł����ŏI��
    if (usePlayerDeathCamera_ == false) return false;

    DirectX::XMFLOAT3 target = PlayerManager::Instance().GetPlayer()->GetJointPosition("spine_02");
    target.y = 0.5f;
    target_ = target;

    switch (playerDeathState_)
    {
    case 0:
        easingTimer_ = 0.0f;

        length_ = 4.5f;
        //length_ = 4.0f;
        targetOffset_ = { 0.0f, -1.0f, 0.0f };
        cameraOffset_ = { 0.0f, 1.0f, 0.0f };

        playerDeathState_ = 1;

        break;
    case 1:
    {
        const float totalFrame = 3.5f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        const float rotateX = Easing::OutQuad(easingTimer_, totalFrame, -20.0f, 10.0f);
        const float rotateY = Easing::InSine(easingTimer_, totalFrame, 70.0f, 160.0f);

        length_ = Easing::OutCubic(easingTimer_, totalFrame, 4.0f, 4.5f);

        GetTransform()->SetRotationX(DirectX::XMConvertToRadians(rotateX));
        GetTransform()->SetRotationY(DirectX::XMConvertToRadians(rotateY));

        if (easingTimer_ == totalFrame)
        {
            easingTimer_ = 0.0f;
            playerDeathState_ = 3;
        }
    }
        break;
    case 2:
        break;

    }


    return true;
}

// ----- �G���S���J���� -----
const bool Camera::UpdateEnemyDeathCamera(const float& elapsedTime)
{
    // �G���S���J�����̎g�p�t���O�������Ă��Ȃ��̂ł����ŏI��
    if (useEnemyDeathCamera_ == false) return false;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    std::string nodeName = (enemyDeathstate_ >= 3) ? "root" : "Dragon15_spine2";
    target_ = enemy->GetJointPosition(nodeName);

    switch (static_cast<EnemyDeathCamera>(enemyDeathstate_))
    {
    case EnemyDeathCamera::Initialize:// ������
#pragma region ---------- ������ ----------
        // �A�j���[�V�����ݒ�
        enemy->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

        // �����o������𖳌�������
        PlayerManager::Instance().SetUseCollisionDetection(false);

        // �J�����̊e�퍀�ڂ�ݒ肷��
        targetOffset_   = { 0.0f, -7.0f, 0.0f };
        cameraOffset_   = { 0.0f, 7.0f, 0.0f };
        length_         = 10.0f;
        GetTransform()->SetRotation({ 0.0f, DirectX::XMConvertToRadians(rand() % 360), 0.0f });
        oldRotate_ = GetTransform()->GetRotation();

        // �ϐ�������
        easingTimer_ = 0.0f;

        // �X�e�[�g�ύX
        SetState(EnemyDeathCamera::FirstCamera);
#pragma endregion ---------- ������ ----------
        break;
    case EnemyDeathCamera::FirstCamera:// �P�ڂ̃J����
#pragma region ---------- �P�ڂ̃J���� ----------
    {
        // �^�C�}�[���Z������Ǘ�
        const float totalFrame = 2.0f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // ��]����
        constexpr float minRotateY = DirectX::XMConvertToRadians(0.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(-40.0f);
        const float rotateY = oldRotate_.y + Easing::InSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotationY(rotateY);

        // �w��̃t���[���𒴂����玟�̃X�e�[�g��
        const float changeStateFrame = 2.0f;
        if (enemy->GetAnimationSeconds() > changeStateFrame)
        {
            // �J�����̏����ʒu��ݒ肷��
            GetTransform()->SetRotation({ 0.0f, DirectX::XMConvertToRadians(rand() % 360), 0.0f });
            oldRotate_ = GetTransform()->GetRotation();

            // �ϐ�������
            easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(EnemyDeathCamera::SecondCamera);
        }
    }
#pragma endregion ---------- �P�ڂ̃J���� ----------
        break;
    case EnemyDeathCamera::SecondCamera:// �Q�ڂ̃J����
#pragma region ---------- �Q�ڂ̃J���� ----------
    {
        // �^�C�}�[���Z������Ǘ�
        const float totalFrame = 2.2f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // ��]����
        constexpr float minRotateX = DirectX::XMConvertToRadians(20.0f);
        constexpr float maxRotateX = DirectX::XMConvertToRadians(30.0f);
        constexpr float minRotateY = DirectX::XMConvertToRadians(0.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(30.0f);
        DirectX::XMFLOAT3 cameraRotate = {};
        cameraRotate.x = Easing::InSine(easingTimer_, totalFrame, maxRotateX, minRotateX);
        cameraRotate.y = oldRotate_.y + Easing::InSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotation(cameraRotate);

        // �w��̃t���[���𒴂����玟�̃X�e�[�g��
        const float changeStateFrame = 4.4f;
        if (enemy->GetAnimationSeconds() > changeStateFrame)
        {
            // �J�����̏����ʒu��ݒ肷��
            length_ = 6.5f;
            GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-30.0f));

            // �ϐ�������
            easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(EnemyDeathCamera::ThirdCamera);
        }
    }
#pragma endregion ---------- �Q�ڂ̃J���� ----------
        break;
    case EnemyDeathCamera::ThirdCamera:// �R�ڂ̃J����
#pragma region ---------- �R�ڂ̃J���� ----------
    {
        // �^�C�}�[���Z������Ǘ�
        const float totalFrame = 2.5f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // ��]����
        constexpr float minRotateY = DirectX::XMConvertToRadians(250.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(220.0f);
        const float rotateY = Easing::OutSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotationY(rotateY);

        // ����p
        fov_ = Easing::InSine(easingTimer_, totalFrame, 50.0f, 45.0f);

        // targetOffset
        targetOffset_.x = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
        targetOffset_.y = Easing::InSine(easingTimer_, totalFrame, -5.0f, -7.0f);
        targetOffset_.z = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
        
        // -------------------------------------------------------
        //  ���S�A�j���[�V�������I����,�����̎��ԃJ��������]�����Ă���
        // -------------------------------------------------------
        // ���S�A�j���[�V�������I������
        if (enemy->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::Death) &&
            enemy->IsPlayAnimation() == false)
        {
            // ���S���[�v���[�V�������Đ�
            enemy->PlayAnimation(Enemy::DragonAnimation::DeathLoop, true);

            // ���S���^�C�}�[���g�p����
            useDeathTimer_ = true;
        }
        // �����̊Ԃ���邽�߂̃^�C�}�[
        if (useDeathTimer_)
        {
            deathTimer_ += elapsedTime;
            
            // �ݒ肵�����Ԃ𒴂����炻�̂܂ܕ��u��Ԃɓ���
            const float maxTime = 1.0f;
            if (deathTimer_ > maxTime)
            {
                // �J���������Ƃɖ߂�
                GetTransform()->SetRotation({ 0, 0, 0 });
                length_ = 6.0f;
                targetOffset_ = { 0.0f, -1.0f, 0.0f };
                cameraOffset_ = { 0.0f, 2.5f, 0.0f };
                fov_ = 45.0f;

                // �G���S���J�������g�p�I��
                useEnemyDeathCamera_ = false;

                // �X�e�[�g�ύX
                SetState(EnemyDeathCamera::Death);
            }
        }
    }
#pragma endregion ---------- �R�ڂ̃J���� ----------
        break;
    case EnemyDeathCamera::Death:// ���S���[�v
        // ���S���Ă���̂ŉ������Ȃ�
        return false;
        break;
    }

    return true;
}

// ----- �h���S���㏸�U�����̃J���� -----
const bool Camera::UpdateRiseAttackCamera(const float& elapsedTime)
{
#if 0
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

    switch (state_)
    {
    case 0:// ������
        easingTimer_ = 0.0f;
        oldCameraLength_ = length_;
        oldRotateX_ = GetTransform()->GetRotationX();
        state_ = 1;
        returnFlag = true;
        break;
    case 1:
    {
        length_ = Easing::OutSine(easingTimer_, totalFrame, maxLength, oldCameraLength_);
        rotateX = Easing::OutSine(easingTimer_, totalFrame, maxRotate, oldRotateX_);
        //cameraOffset_.y = Easing::InSine(riseAttackEasingTimer_, totalFrame, maxOffsetX, minOffsetX);

        easingTimer_ += elapsedTime;
        if (easingTimer_ > totalFrame)
        {
            length_ = maxLength;
            rotateX = maxRotate;
            //cameraOffset_.y = maxOffsetX;

            state_ = 2;
            easingTimer_ = 0.0f;
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
        length_ = Easing::InSine(easingTimer_, totalFrame1, resetLength, maxLength);
        rotateX = Easing::InSine(easingTimer_, totalFrame1, DirectX::XMConvertToRadians(7.0f), maxRotate);
        //cameraOffset_.y = Easing::InSine(riseAttackEasingTimer_, totalFrame, minOffsetX, maxOffsetX);

        easingTimer_ += elapsedTime;
        if (easingTimer_ > totalFrame1)
        {
            //length_ = oldCameraLength_;
            length_ = resetLength;
            rotateX = DirectX::XMConvertToRadians(7.0f);
            //cameraOffset_.y = minOffsetX;

            easingTimer_ = 0.0f;
            state_ = 5;// ���g�p�ɂ���
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
#endif
    return false;
}

// ----- �J�E���^�[�U�����̃J�����X�V -----
const bool Camera::UpdateCounterAttackCamera(const float& elapsedTime)
{
    // �J�E���^�[���J�����g�p�t���O�������Ă��Ȃ��̂ł����ŏI��
    if (useCounterCamera_ == false) return false;

    Player* player = PlayerManager::Instance().GetPlayer().get();

    switch (static_cast<CounterAttackCamera>(counterState_))
    {
    case CounterAttackCamera::CounterInitialize:// �J�E���^�[������
        // ���݂̃J�������ڂ�ۑ�����
        oldLength_ = length_;
        oldRotate_ = GetTransform()->GetRotation();

        // �ϐ�������
        easingTimer_ = 0.0f;

        // �X�e�[�g�ύX
        SetState(CounterAttackCamera::CounterZoomOut);

        break;
    case CounterAttackCamera::CounterZoomOut:// �J����������
    {        
        if (player->GetAnimationSeconds() < 0.25f) break;

        const float totalFrame = 0.2f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        if (isAdjustCameraLength_ == false)
        {
            length_ = Easing::InSine(easingTimer_, totalFrame, 7.0f, oldLength_);

            const float maxRotateX = oldRotate_.x + DirectX::XMConvertToRadians(-1.5f);
            const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotateX, oldRotate_.x);
            GetTransform()->SetRotationX(rotateX);
        }

        if (easingTimer_ == totalFrame)
        {
            // �ϐ�������
            easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(CounterAttackCamera::CounterIdle);
        }
    }
        break;
    case CounterAttackCamera::CounterIdle:// ���̍s���ҋ@
    {
        const Player::STATE currentState = player->GetCurrentState();

        if (currentState == Player::STATE::Idle ||
            currentState == Player::STATE::Damage)
        {
            // �X�e�[�g�ύX
            SetState(CounterAttackCamera::CounterFinalize);
        }
        if (currentState == Player::STATE::CounterCombo)
        {
            // �X�e�[�g�ύX
            SetState(CounterAttackCamera::CounterComboInitialize);
        }
    }
        break;
    case CounterAttackCamera::CounterFinalize:// �J�E���^�[�I����
    {
        const float totalFrame = 0.5f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        if (isAdjustCameraLength_ == false)
        {
            length_ = Easing::InSine(easingTimer_, totalFrame, oldLength_, 7.0f);

            const float minRotateX = oldRotate_.x + DirectX::XMConvertToRadians(-1.5f);
            const float rotateX = Easing::InSine(easingTimer_, totalFrame, oldRotate_.x, minRotateX);
            GetTransform()->SetRotationX(rotateX);
        }

        if (easingTimer_ == totalFrame)
        {
            // �ϐ�������
            easingTimer_ = 0.0f;

            // �J�E���^�[�J�����g�p�I��
            useCounterCamera_ = false;
        }
    }
        break;
    case CounterAttackCamera::CounterComboInitialize:// �J�E���^�[�R���{������

        // ���݂̃J�����̍��ڂ�ۑ�����
        oldLength_ = length_;
        oldRotate_ = GetTransform()->GetRotation();

        // �ϐ�������
        easingTimer_ = 0.0f;

        counterDelayTimer_ = 0.0f;

        // �X�e�[�g�ύX
        SetState(CounterAttackCamera::CounterComboZoomIn);

        break;  
    case CounterAttackCamera::CounterComboZoomIn:
    {
        const float totalFrame = 0.2f;
        //const float totalFrame = 0.17f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

#if 0
        length_ = Easing::InSine(easingTimer_, totalFrame, 4.5f, oldLength_);

        const float maxRotateX = oldRotate_.x + DirectX::XMConvertToRadians(3.0f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotateX, oldRotate_.x);
        GetTransform()->SetRotationX(rotateX);
#else
        length_ = Easing::InSine(easingTimer_, totalFrame, 5.5f, oldLength_);

        const float maxRotateX = oldRotate_.x + DirectX::XMConvertToRadians(3.0f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotateX, oldRotate_.x);
        GetTransform()->SetRotationX(rotateX);
#endif

        if (easingTimer_ == totalFrame)
        {
            counterDelayTimer_ += elapsedTime;

            if (counterDelayTimer_ >= 0.1f)
            {
                easingTimer_ = 0.0f;

                counterDelay_ = target_;
                isCounterDelay_ = true;


                SetState(CounterAttackCamera::CounterComboZoomOut);
            }

            //easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            //SetState(CounterAttackCamera::CounterComboZoomOut);
        }
    }
        break;
    case CounterAttackCamera::CounterComboZoomOut:
    {
        const float totalFrame = 0.2f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

#if 0
        length_ = Easing::InSine(easingTimer_, totalFrame, 9.0f, 4.5f);
#else
        length_ = Easing::InSine(easingTimer_, totalFrame, 9.0f, 5.5f);
#endif

        const float maxRotate = oldRotate_.x + DirectX::XMConvertToRadians(-3.5f);
        const float minRotate = oldRotate_.x + DirectX::XMConvertToRadians(3.0f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotate, minRotate);
        GetTransform()->SetRotationX(rotateX);

        if (easingTimer_ == totalFrame)
        {
            easingTimer_ = 0.0f;
            counterDelayTimer_ = 0.0f;

            counterLerpTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(CounterAttackCamera::Finalize);
        }
    }
        break;
    case CounterAttackCamera::Finalize:
    {
        counterDelayTimer_ += elapsedTime;
        if (counterDelayTimer_ > 0.5f)
        {

            
            counterLerpTimer_ += counterLerpSpeed_ * elapsedTime;
            counterLerpTimer_ = min(counterLerpTimer_, 1.0f);

            if (isCounterDelay_)
            {
                const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
                DirectX::XMVECTOR vec = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&counterDelay_), DirectX::XMLoadFloat3(&cameraTargetPosition), counterLerpTimer_);
                DirectX::XMFLOAT3 pos = {};
                DirectX::XMStoreFloat3(&pos, vec);

                // length�ݒ�
                length_ = XMFloatLerp(9.0f, 6.5f, counterLerpTimer_);

                // rotationX�ݒ�

                target_ = pos;
            }
            if (counterLerpTimer_ == 1.0f)
            {
                isCounterDelay_ = false;
                useCounterCamera_ = false;
            }
        }

#if 0
        counterDelayTimer_ += elapsedTime;
        if (counterDelayTimer_ > 0.5f)
        {


        const float totalFrame = 0.6f;

        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);
        
        length_ = Easing::InSine(easingTimer_, totalFrame, 6.0f, 9.0f);

        const float minRotate = oldRotate_.x + DirectX::XMConvertToRadians(-3.5f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, 0.0f, minRotate);
        GetTransform()->SetRotationX(rotateX);

        if(easingTimer_ == totalFrame)
        {
            useCounterCamera_ = false;
        }

        }
#endif

    }
        break;
    }

    return true;
}
