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

    void SetTitleCamera();   // タイトル用カメラ設定
    void SetGameCamera();    // ゲーム用カメラ設定
    void SetLoadingCamera(); // ローディング用カメラ設定

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


    // ---------- カメラから見たスティックの入力値を算出する ----------
    [[nodiscard]] const DirectX::XMFLOAT2 ConvertTo2DVectorFromCamera(const DirectX::XMFLOAT2& v);
    

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

    // ---------- 各種カメラ使用設定 ----------
    void SetUsePlayerDeathCmaera(const float& flag = true); // プレイヤー死亡演出カメラを使用する
    void UseDragonDeathCamera();                            // ドラゴンの死亡演出カメラを使用する
    void UseCounterCamera();        // カウンターカメラを使用する
    void UseCounterAttackCamera();  // カウンター攻撃カメラを使用する
    void UseHelmbreakerCamera();
    void UseMageAttackCamera();

    void ResetCameraFlags();

private:
#pragma region---------- 各種カメラの定数 ----------
    enum class EnemyDeathCamera
    {
        Initialize,     // 初期化
        FirstCamera,    // １つ目のカメラ
        SecondCamera,   // ２つ目のカメラ
        ThirdCamera,    // ３つ目のカメラ
        Death,          // 死亡ループ
    };
    enum class CounterAttackCamera
    {
        CounterInitialize,          // カウンター初期化
        CounterZoomOut,             // カメラを引く
        CounterIdle,                // 次の行動待ち
        CounterFinalize,            // カウンター終了化

        CounterComboInitialize,     // カウンターコンボ初期化
        CounterComboZoomIn,         // カウンター攻撃カメラ近づける
        CounterComboZoomOut,        // カウンター攻撃カメラひく

        Finalize,
    };
#pragma endregion ---------- 各種カメラの定数 ----------


private:
    void UpdateTargetCamera(const float& elapsedTime); // ターゲットカメラ

    void UpdateCameraReset(const float& elapsedTime);  // カメラリセット更新


    // ---------- プレイヤー死亡カメラ ----------
    [[nodiscard]] const bool UpdatePlayerDeathCamera(const float& elapsedTime);
    // ---------- ドラゴン死亡カメラ ----------
    [[nodiscard]] const bool UpdateDragonDeathCamera(const float& elapsedTime);
    // ---------- カウンター攻撃時のカメラ更新 ----------
    void UpdateCounterCamera(const float& elapsedTime);
    void UpdateCounterAttackCamera(const float& elapsedTime);
    
    // ---------- 魔法攻撃カメラ ----------
    void UpdateMageAttackCamera(const float& elapsedTime);

    void UpdateHelmbreakerCamera(const float& elapsedTime);

    void SetState(const EnemyDeathCamera& state)    { dragonDeathState_ = static_cast<int>(state); }
    void SetState(const CounterAttackCamera& state) { counterState_ = static_cast<int>(state); }

private:
    Transform           transform_          = {};
    DirectX::XMMATRIX   viewMatrix_         = {};
    DirectX::XMMATRIX   projectionMatrix_   = {};
    View                view_               = {};

    DirectX::XMFLOAT3   target_             = {};
    DirectX::XMFLOAT3   offset_             = {};
    float               nearZ_              = 0.1f;
    float               farZ_               = 150.0f;
    float               fov_                = 45.0f;    // 視野角
    float               length_             = 10.0f;    // focusとeyeまでの距離
    
    // ---------- 入力による回転の詳細 ----------
    float               inputThreshold_          = 0.3f;  // 入力判定値    
    float               verticalRotationSpeed_   = 1.7f;  // 垂直回転速度
    float               horizontalRotationSpeed_ = 4.0f;  // 水平回転速度
    bool                invertVertical_          = false; // 上下反転フラグ
    //bool                invertVertical_          = true; // 上下反転フラグ

    // ---------- 回転角制御用 ----------
    float               minRotationX_       = DirectX::XMConvertToRadians(-15.0f);
    float               maxRotationX_       = DirectX::XMConvertToRadians(35.0f);

    // ---------- カメラ位置制御用 ----------
    float               minLength_          = 6.0f;
    float               maxLength_          = 6.5f;

    float groundNearest_ = 0.2f;

    // ---------- 画面振動 ----------
    DirectX::XMFLOAT3   screenVibrationOffset_  = {};   // 振動表現用の座標
    float               vibrationVolume_        = 0.0f; // 振動量
    float               vibrationTime_          = 0.0f; // 振動時間
    float               vibrationTimer_         = 0.0f; // 振動時間を測るタイマー

    // ---------- ターゲットカメラ ----------
    DirectX::XMFLOAT2   startRotation_          = {};
    DirectX::XMFLOAT2   targetRotation_         = {};
    float               targetCameraLerpTimer_  = 0.0f;
    float               targetCameraLerpSpeed_  = 10.0f;
    bool                isTargetCameraActive_   = false;

    // ---------- カメラリセット ----------
    DirectX::XMFLOAT2   resetOldRotation_       = {};    // 処理開始時の角度
    DirectX::XMFLOAT2   resetTargetRotation_    = {};    // 目標地点の角度
    float               resetLerpTimer_         = 0.0f;  // lerpに使用
    float               resetLerpSpeed_         = 10.0f; // lerpの速度
    bool                cameraResetFlag_        = false; // カメラリセットをするかのフラグ

    // ---------- カメラ補間 ----------
    float   lerpWeight_     = 0.0f;

    // ---------- カメラ演出用 ----------
    float               easingTimer_    = 0.0f;
    DirectX::XMFLOAT3   oldRotate_      = {};
    float               oldLength_      = 0.0f;

    // ---------- タイトルカメラ用 ----------
    DirectX::XMFLOAT3   titleCameraOffset_      = DirectX::XMFLOAT3(0.0f, 1.5f, 0.0f);
    DirectX::XMFLOAT3   titleCameraRotation_    = DirectX::XMFLOAT3(DirectX::XMConvertToRadians(-10.0f), DirectX::XMConvertToRadians(180.0f), 0.0f);
    float               titleCameraLength_      = 4.5f;

    // ---------- ゲームカメラ用 ----------
    DirectX::XMFLOAT3   gameCameraOffset_       = DirectX::XMFLOAT3(0.0f, 1.6f, 0.0f);
    float               gameCameraLength_       = 4.5f;
    float               gameCameraLerpWeight_   = 0.12f;
    float               gameCameraMinRotationX_ = DirectX::XMConvertToRadians(-15);

    float gameCameraLengthReturnSpeed_ = 5.0f;
    float gameCameraLerpWeightReturnSpeed_ = 0.6f;

    // ---------- 必殺技カメラ ----------
    float   helmbreakerCameraLerpWeight_    = 0.35f;
    int     helmbreakerCameraState_         = 0;
    bool    isHelmbreakerCameraActive_      = false;

    // ---------- カウンターカメラ ----------
    float               counterTime_                    = 0.2f;
    float               counterReturnTime_              = 0.5f;
    float               counterLength_                  = 5.5f;
    float               counterMinRotationX_            = -5.0f;
    float               counterLerpWegiht_              = 0.1f;
    float               counterAttackZoomTime_          = 0.2f;
    float               counterAttackSlowTime_          = 0.15f;
    float               counterAttackTime_              = 0.2f;
    float               counterAttackZoomLength_        = 4.0f;
    float               counterAttackSlowLength_        = 3.8f;
    float               counterAttackLength_            = 8.0f;
    float               counterAttackLerpWeight_        = 0.0f;
    int                 counterState_                   = 0;
    bool                isCounterCameraActive_          = false;
    bool                isCounterAttackCameraActive_    = false;

    // ---------- 魔法攻撃カメラ ----------
    float   mageAttackTotalFrame_   = 0.5f;
    float   mageAttackMaxLength_    = 10.0f;
    float   mageAttackMinRotationX_ = DirectX::XMConvertToRadians(5.0f);
    int     mageAttackState_        = 0;
    bool    isMageAttackCameraAcitve_ = false;

    // ---------- プレイヤー死亡カメラ ----------
    DirectX::XMFLOAT3   playerDeathOffset_              = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    DirectX::XMFLOAT3   playerDeathRotation_            = DirectX::XMFLOAT3(-20.0f, 210.0f, 0.0f);
    float               playerDeathTime_                = 3.5f;
    float               playerDeathLength_              = 4.5f;
    float               playerDeathMinLength_           = 4.0f;
    float               playerDeathMaxRotationX_        = 30.0f;
    float               playerDeathMaxRotationY_        = 90.0f;
    int                 playerDeathState_               = 0;
    bool                isPlayerDeathCameraActive_      = false; // プレイヤー死亡カメラ

    // ---------- ドラゴン死亡カメラ ----------
    DirectX::XMFLOAT3   dragonDeathFirstOffset_         = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    DirectX::XMFLOAT3   dragonDeathFirstRotation_       = DirectX::XMFLOAT3(-10.0f, 0.0f, 0.0f);
    float               dragonDeathFirstTime_           = 2.0f;  // １つ目のカメラの使用時間
    float               dragonDeathFirstLength_         = 9.0f;  
    float               dragonDeathFirstMaxRotationY_   = -40.0f;
    DirectX::XMFLOAT3   dragonDeathSecondRotation_      = DirectX::XMFLOAT3(20.0f, 0.0f, 0.0f);
    float               dragonDeathSecondTime_          = 2.2f;  // ２つ目のカメラの使用時間
    float               dragonDeathSecondLength_        = 10.0f;
    float               dragonDeathSecondMaxRotationX_  = 10.0f;
    float               dragonDeathSecondMaxRotationY_  = 30.0f;
    DirectX::XMFLOAT3   dragonDeathThirdRotation_       = DirectX::XMFLOAT3(55.0f, 250.0f, 0.0f);
    float               dragonDeathThirdTime_           = 2.5f;  // ３つ目のカメラの使用時間
    float               dragonDeathThirdLength_         = 10.0f;
    float               dragonDeathThirdMaxRotationX_   = 15.0f;
    float               dragonDeathThirdMaxRotationY_   = -50.0f;
    float               dragonDeathChangeFrame_         = 0.0f;  // カメラ切り替えフレーム
    float               dragonDeathTimer_               = 0.0f;
    int                 dragonDeathState_               = 0;     // 制御用
    bool                dragonDeathtimerActive_         = false;
    bool                isDragonDeathCameraActive_      = false; // 敵死亡カメラ
};
