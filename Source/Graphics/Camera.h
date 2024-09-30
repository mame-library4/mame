#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "../Other/Transform.h"

class Camera
{
private:
    Camera()  {}
    ~Camera() {}

public:
    static Camera& Instance()
    {
        static Camera camera;
        return camera;
    }

    void Initialize();    
    void Update(const float& elapsedTime);

    void SetPerspectiveFov();
    void DrawDebug();

    // ---------- ��]���� ---------------
    void Rotate(const float& elapsedTime);

    // ---------- ��ʐU�� --------------------
    void ScreenVibrate(const float& vibrationVolume/*�U����*/, const float& vibrationTime/*�U������*/);
    void ScreenVibrationUpdate(const float& elapsedTime);

    struct View
    {
        DirectX::XMFLOAT3 eye_      = { 0.0f, 0.0f, -10.0f };
        DirectX::XMFLOAT3 focus_    = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 up_       = { 0.0f, 1.0f, 0.0f };
    };

public:// --- �擾�E�ݒ� ---
    // ---------- Transform -------------------------
    Transform* GetTransform() { return &transform_; }

    // ---------- �J�����s�� -------------------------
    DirectX::XMMATRIX GetViewMatrix() { return viewMatrix_; }
    DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix_; }
    void SetViewMatrix(DirectX::XMMATRIX v) { viewMatrix_ = v; }
    void SetProjectionMatrix(DirectX::XMMATRIX p) { projectionMatrix_ = p; }

    [[nodiscard]] const DirectX::XMFLOAT3 GetEye() { return view_.eye_; }

    // ---------- �x�N�g���擾 ( �O, �E ) ---------------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcForward();
    [[nodiscard]] const DirectX::XMFLOAT3 CalcRight();

    // ---------- �^�[�Q�b�g --------------------
    [[nodiscard]] const DirectX::XMFLOAT3 GetTarget() const { return target_; }
    void SetTarget(const DirectX::XMFLOAT3& target) { target_ = target; }

    // ---------- Lenght -------------------------
    [[nodiscard]] const float GetLength() const { return length_; }
    void SetLength(const float length) { length_ = length; }

    // ---------- Fov ------------------------------
    [[nodiscard]] const float GetFov() const { return fov_; }
    void SetFov(const float& fov) { fov_ = fov; }

    // ---------- Offset -------------------------
    [[nodiscard]] const DirectX::XMFLOAT3 GetTargetOffset() const { return targetOffset_; }
    void SetTargetOffset(const DirectX::XMFLOAT3& offset) { targetOffset_ = offset; }
    [[nodiscard]] const DirectX::XMFLOAT3 GetCameraOffset() const { return cameraOffset_; }
    void SetCameraOffset(const DirectX::XMFLOAT3& offset) { cameraOffset_ = offset; }

    // ---------- ����ȓ�������p ----------
    void SetUsePlayerDeathCmaera(const float& flag = true);
    void SetUseEnemyDeathCamera();
    void SetUseCounterCamera();

    // ---------- ���b�N�I���J���� ----------
    [[nodiscard]] const std::string GetCurrentTargetJointName() const { return targetJointName_.at(currentTargetJointIndex_); }

private:
#pragma region---------- �e��J�����̒萔 ----------
    enum class EnemyDeathCamera
    {
        Initialize,     // ������
        FirstCamera,    // �P�ڂ̃J����
        SecondCamera,   // �Q�ڂ̃J����
        ThirdCamera,    // �R�ڂ̃J����
        Death,          // ���S���[�v
    };
    enum class CounterAttackCamera
    {
        CounterInitialize,          // �J�E���^�[������
        CounterZoomOut,             // �J����������
        CounterIdle,                // ���̍s���҂�
        CounterFinalize,            // �J�E���^�[�I����

        CounterComboInitialize,     // �J�E���^�[�R���{������
        CounterComboZoomIn,         // �J�E���^�[�U���J�����߂Â���
        CounterComboZoomOut,        // �J�E���^�[�U���J�����Ђ�

        Finalize,
    };
#pragma endregion ---------- �e��J�����̒萔 ----------


private:
    void UpdateLockonCamera(const float& elapsedTime); // ���b�N�I���J�����X�V
    void UpdateCameraReset(const float& elapsedTime);  // �J�������Z�b�g�X�V


    // ---------- ���@���S�J���� ----------
    [[nodiscard]] const bool UpdatePlayerDeathCamera(const float& elapsedTime);
    // ---------- �G���S�J���� ----------
    [[nodiscard]] const bool UpdateEnemyDeathCamera(const float& elapsedTime);
    // ---------- �h���S���㏸�U�����̃J���� ----------
    [[nodiscard]] const bool UpdateRiseAttackCamera(const float& elapsedTime);
    // ---------- �J�E���^�[�U�����̃J�����X�V ----------
    [[nodiscard]] const bool UpdateCounterAttackCamera(const float& elapsedTime);

    void SetState(const EnemyDeathCamera& state)    { enemyDeathstate_ = static_cast<int>(state); }
    void SetState(const CounterAttackCamera& state) { counterState_ = static_cast<int>(state); }

private:
    Transform           transform_          = {};
    DirectX::XMMATRIX   viewMatrix_         = {};
    DirectX::XMMATRIX   projectionMatrix_   = {};
    View                view_               = {};

    DirectX::XMFLOAT3   target_             = {};
    DirectX::XMFLOAT3   targetOffset_       = {};       // �^�[�Q�b�g(�����_)�ɑ΂���I�t�Z�b�g
    DirectX::XMFLOAT3   cameraOffset_       = {};       // �J�����̈ʒu�ɑ΂���I�t�Z�b�g
    float               nearZ_              = 0.1f;
    float               farZ_               = 1000.0f;
    float               fov_                = 45.0f;    // ����p
    float               length_             = 10.0f;    // focus��eye�܂ł̋���
    float               inputThreshold_     = 0.3f;     // ���͔���l
    

    float               verticalRotationSpeed_      = 1.0f; // ������]���x
    float               horizontalRotationSpeed_    = 1.0f; // ������]���x

    bool                invertVertical_     = false;    // �㉺���]�t���O

    // ---------- ��]�p����p ----------
    float               minXRotation_       = DirectX::XMConvertToRadians(-15.0f);
    float               maxXRotation_       = DirectX::XMConvertToRadians(13.0f);

    // ---------- �J�����ʒu����p ----------
    float               minLength_          = 6.0f;
    float               maxLength_          = 6.5f;

    // ---------- ��ʐU�� ----------
    DirectX::XMFLOAT3   screenVibrationOffset_  = {};   // �U���\���p�̍��W
    float               vibrationVolume_        = 0.0f; // �U����
    float               vibrationTime_          = 0.0f; // �U������
    float               vibrationTimer_         = 0.0f; // �U�����Ԃ𑪂�^�C�}�[

    // ---------- ����ȓ�������p ----------    
    int     playerDeathState_       = 0;
    
    int     enemyDeathstate_        = 0;

    float counterDelayTimer_ = 0.0f;

    float   easingTimer_            = 0.0f;
    bool    useEnemyDeathCamera_    = false; // �G���S�J����
    bool    usePlayerDeathCamera_   = false; // ���@���S�J����
    bool    useRiseAttackCamera_    = false; // �㏸�U���J����
    

    float   deathTimer_             = 0.0f;  // ���S���^�C�}�[
    bool    useDeathTimer_          = false; // ���S���^�C�}�[���g����


    DirectX::XMFLOAT3   oldRotate_ = {};
    float               oldLength_ = 0.0f;
    float               oldCameraOffsetY_ = 0.0f;

    bool isAdjustCameraLength_ = false; // �n�ʂɖ��܂�Ȃ��悤��

    float lerpTimer_ = 0.0f;

    // ---------- �J�E���^�[�J���� ----------
    int     counterState_       = 0;
    bool    useCounterCamera_   = false; // �J�E���^�[�J����

    DirectX::XMFLOAT3 counterDelay_ = {};
    bool isCounterDelay_ = false;
    float counterLerpTimer_ = 0.0f;

    // ---------- ���b�N�I���J���� ----------
    std::vector<std::string>    targetJointName_;                    // �^�[�Q�b�g�ɂ���W���C���g�̖��O
    int                         currentTargetJointIndex_    = 0;     // ���ݑI�𒆂̃W���C���g�ԍ�
    float                       lockonInputThreshold_       = 0.3f;  // �W���C���g�ύX�̓���臒l
    bool                        useLockonCamera_            = false; // ���b�N�I���J�����g�p�t���O
    bool                        isNextJointAccessible       = true;  // ���̃^�[�Q�b�g��I���ł��邩

    // ---------- �J�������Z�b�g ----------
    DirectX::XMFLOAT2   resetOldRotation_       = {};    // �����J�n���̊p�x
    DirectX::XMFLOAT2   resetTargetRotation_    = {};    // �ڕW�n�_�̊p�x
    float               resetLerpTimer_         = 0.0f;  // lerp�Ɏg�p
    float               resetLerpSpeed_         = 15.0f; // lerp�̑��x
    bool                cameraResetFlag_        = false; // �J�������Z�b�g�����邩�̃t���O
};
