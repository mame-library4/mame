#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
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

    // ---------- �x�N�g���擾 ( �O, �E ) ---------------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcForward();
    [[nodiscard]] const DirectX::XMFLOAT3 CalcRight();

    // ---------- �^�[�Q�b�g --------------------
    [[nodiscard]] const DirectX::XMFLOAT3 GetTarget() const { return target_; }
    void SetTarget(const DirectX::XMFLOAT3& target) { target_ = target; }

private:
    Transform           transform_          = {};
    DirectX::XMMATRIX   viewMatrix_         = {};
    DirectX::XMMATRIX   projectionMatrix_   = {};
    View                view_               = {};

    DirectX::XMFLOAT3   target_             = {};
    float               nearZ_              = 0.1f;
    float               farZ_               = 1000.0f;
    float               fov_                = 45.0f;    // ����p
    float               length_             = 10.0f;    // focus��eye�܂ł̋���
    float               inputThreshold_     = 0.3f;     // ���͔���l
    
    // ---------- ��]�p����p ----------
    float               minXRotation_       = DirectX::XMConvertToRadians(-30.0f);
    float               maxXRotation_       = DirectX::XMConvertToRadians(-1.0f);

    // ---------- ��ʐU�� ----------
    DirectX::XMFLOAT3   screenVibrationOffset_  = {};   // �U���\���p�̍��W
    float               vibrationVolume_        = 0.0f; // �U����
    float               vibrationTime_          = 0.0f; // �U������
    float               vibrationTimer_         = 0.0f; // �U�����Ԃ𑪂�^�C�}�[

};
