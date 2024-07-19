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

    // ---------- 回転処理 ---------------
    void Rotate(const float& elapsedTime);

    // ---------- 画面振動 --------------------
    void ScreenVibrate(const float& vibrationVolume/*振動量*/, const float& vibrationTime/*振動時間*/);
    void ScreenVibrationUpdate(const float& elapsedTime);

    struct View
    {
        DirectX::XMFLOAT3 eye_      = { 0.0f, 0.0f, -10.0f };
        DirectX::XMFLOAT3 focus_    = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 up_       = { 0.0f, 1.0f, 0.0f };
    };

public:// --- 取得・設定 ---
    // ---------- Transform -------------------------
    Transform* GetTransform() { return &transform_; }

    // ---------- カメラ行列 -------------------------
    DirectX::XMMATRIX GetViewMatrix() { return viewMatrix_; }
    DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix_; }
    void SetViewMatrix(DirectX::XMMATRIX v) { viewMatrix_ = v; }
    void SetProjectionMatrix(DirectX::XMMATRIX p) { projectionMatrix_ = p; }

    [[nodiscard]] const DirectX::XMFLOAT3 GetEye() { return view_.eye_; }

    // ---------- ベクトル取得 ( 前, 右 ) ---------------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcForward();
    [[nodiscard]] const DirectX::XMFLOAT3 CalcRight();

    // ---------- ターゲット --------------------
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

    // ---------- 特殊な動き制御用 ----------
    void SetRiseAttackState(const int& state) { riseAttackState_ = state; }

    // ---------- 死亡カメラ ----------
    [[nodiscard]] const bool GetUseDeathCamera() const { return useDeathCamera_; }
    void SetUseDeathCamera(const bool& flag) { useDeathCamera_ = flag; }

private:
    // ---------- ドラゴン上昇攻撃時のカメラ ----------
    [[nodiscard]] const bool UpdateRiseAttackCamera(const float& elapsedTime);

private:
    Transform           transform_          = {};
    DirectX::XMMATRIX   viewMatrix_         = {};
    DirectX::XMMATRIX   projectionMatrix_   = {};
    View                view_               = {};

    DirectX::XMFLOAT3   target_             = {};
    DirectX::XMFLOAT3   targetOffset_       = {};       // ターゲット(注視点)に対するオフセット
    DirectX::XMFLOAT3   cameraOffset_       = {};       // カメラの位置に対するオフセット
    float               nearZ_              = 0.1f;
    float               farZ_               = 1000.0f;
    float               fov_                = 45.0f;    // 視野角
    float               length_             = 10.0f;    // focusとeyeまでの距離
    float               inputThreshold_     = 0.3f;     // 入力判定値
    

    float               verticalRotationSpeed_      = 1.0f; // 垂直回転速度
    float               horizontalRotationSpeed_    = 1.0f; // 水平回転速度

    bool                invertVertical_     = false;    // 上下反転フラグ

    // ---------- 回転角制御用 ----------
    float               minXRotation_       = DirectX::XMConvertToRadians(-15.0f);
    float               maxXRotation_       = DirectX::XMConvertToRadians(13.0f);

    // ---------- カメラ位置制御用 ----------
    float               minLength_          = 6.0f;
    float               maxLength_          = 6.5f;

    // ---------- 画面振動 ----------
    DirectX::XMFLOAT3   screenVibrationOffset_  = {};   // 振動表現用の座標
    float               vibrationVolume_        = 0.0f; // 振動量
    float               vibrationTime_          = 0.0f; // 振動時間
    float               vibrationTimer_         = 0.0f; // 振動時間を測るタイマー

    // ---------- 特殊な動き制御用 ----------
    float               riseAttackEasingTimer_  = 0.0f;
    int                 riseAttackState_ = 5;
    float               oldCameraLength_ = 0.0f;
    float               oldRotateX_ = 0.0f;

    bool useDeathCamera_ = false; // 死亡カメラ
};
