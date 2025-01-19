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
#include "AudioManager.h"

#include "System/SystemManager.h"

// ----- 初期化 -----
void Camera::Initialize()
{
    isDragonDeathCameraActive_ = false;

    dragonDeathState_ = 0;
    playerDeathState_ = 0;
    counterState_ = 0;


    lerpWeight_ = gameCameraLerpWeight_;

    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    
    GetTransform()->SetRotationX(DirectX::XMConvertToRadians(10.0f));
    offset_         = gameCameraOffset_;
    length_         = gameCameraLength_;
}

// ----- 更新 -----
void Camera::Update(const float& elapsedTime)
{
    // タイトル画面はとりあえずここで終了
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title ||
        SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Loading) return;

    if (GetAsyncKeyState('1') & 0x01)
    {
        invertVertical_ = !invertVertical_;
    }

    // TODO:各種死亡カメラが使用されているときはカメラシェイクを行わない
    if (isPlayerDeathCameraActive_ || isDragonDeathCameraActive_)
    {
        ScreenVibrate(0.0f, 0.0f);
    }

    // 登場演出カメラ
    if (UpdateDragonAppearCamera(elapsedTime)) return;

    // 死亡カメラ使用時はここで終了
    if (UpdatePlayerDeathCamera(elapsedTime)) return; // Player死亡カメラ
    if (UpdateDragonDeathCamera(elapsedTime))  return; // Enemy死亡カメラ    

    // カウンターカメラ
    UpdateCounterCamera(elapsedTime);
    // カウンター攻撃カメラ
    UpdateCounterAttackCamera(elapsedTime);

    UpdateHelmbreakerCamera(elapsedTime);

    // 魔法攻撃カメラ
    UpdateMageAttackCamera(elapsedTime);

    // 専用カメラを使用していないとき
    if (isCounterCameraActive_ == false && isCounterAttackCameraActive_ == false && isHelmbreakerCameraActive_ == false && isMageAttackCameraAcitve_ == false)
    {
        if (length_ != gameCameraLength_)
        {
            length_ -= gameCameraLengthReturnSpeed_ * elapsedTime;
            if (length_ <= gameCameraLength_)
            {
                length_ = gameCameraLength_;
                lerpWeight_ = gameCameraLerpWeight_;
            }
        }
    }

    DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    if (isHelmbreakerCameraActive_) cameraTargetPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    
    target_ = XMFloat3Lerp(target_, cameraTargetPosition, lerpWeight_);

    // ターゲットカメラ
    UpdateTargetCamera(elapsedTime);

    // カメラリセット
    UpdateCameraReset(elapsedTime);

    // カメラ回転処理
    Rotate(elapsedTime);

    // --- 画面振動 ---
    ScreenVibrationUpdate(elapsedTime);
}

void Camera::SetPerspectiveFov()
{
    DirectX::XMFLOAT3 forward = XMFloat3Normalize(GetTransform()->CalcForward());

    view_.eye_   = target_ + offset_ - forward * length_;
    view_.focus_ = target_ + offset_;

    GetTransform()->SetPosition(view_.eye_);

    // ----- カメラをステージ内に収める -----
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

    // --- projectionMatrix 設定 ---
    float aspectRatio = SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    float fov = DirectX::XMConvertToRadians(fov_);
    projectionMatrix_ = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ_, farZ_);

    // --- viewMatrix 設定 ---
    DirectX::XMVECTOR eye, focus, up;

    eye     = DirectX::XMVectorSet(view_.eye_.x, view_.eye_.y, view_.eye_.z, 1.0f);
    focus   = DirectX::XMVectorSet(view_.focus_.x, view_.focus_.y, view_.focus_.z, 1.0f);
    focus   = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&screenVibrationOffset_), focus);
    up      = DirectX::XMVectorSet(view_.up_.x, view_.up_.y, view_.up_.z, 0.0f);
    
    viewMatrix_ = DirectX::XMMatrixLookAtLH(eye, focus, up);
}

// ----- ImGui用 -----
void Camera::DrawDebug()
{
    if (ImGui::BeginMenu("Camera"))
    {
        if (ImGui::TreeNodeEx("DragonAppearCamera", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("===== Animation =====", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("AnimationStartFrame", &dragonAppearCamera_.animationStartFrame_);
                ImGui::DragFloat("AnimationEndFrame", &dragonAppearCamera_.animationEndFrame_);
                ImGui::DragFloat("DescentStartFrame", &dragonAppearCamera_.descentStartFrame_);
                ImGui::DragFloat("DescentEndFrame", &dragonAppearCamera_.descentEndFrame_);
                
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("===== Length =====", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Init Length", &dragonAppearCamera_.initializeLength_);
                ImGui::DragFloat("1 Target Length", &dragonAppearCamera_.firstTargetLength_);
                ImGui::DragFloat("2 Target Length", &dragonAppearCamera_.secondTargetLength_);
                
                ImGui::DragFloat("2 Target Length Frame", &dragonAppearCamera_.secondTargetLengthTotalFrame_, 0.01f);
                ImGui::DragFloat("2 Target Length Timer", &dragonAppearCamera_.secondTargetLengthTimer_, 0.01f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("===== Rotation =====", ImGuiTreeNodeFlags_DefaultOpen))
            {
                float rotationX = DirectX::XMConvertToDegrees(dragonAppearCamera_.initializeRotationX_);
                ImGui::DragFloat("Init RotationY", &rotationX, 0.1f);
                dragonAppearCamera_.initializeRotationX_ = DirectX::XMConvertToRadians(rotationX);
                rotationX = DirectX::XMConvertToDegrees(dragonAppearCamera_.firstTargetRotationX_);
                ImGui::DragFloat("1 Target RotationY", &rotationX, 0.1f);
                dragonAppearCamera_.firstTargetRotationX_ = DirectX::XMConvertToRadians(rotationX);

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("===== Offset =====", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Init Offset", &dragonAppearCamera_.initializeCameraOffsetY_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

#pragma region ---------- PlayerDeathCamera ----------
        if (ImGui::TreeNodeEx("PlayerDeathCamera", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::Checkbox("Active", &isPlayerDeathCameraActive_);
            ImGui::DragInt("State", &playerDeathState_, 1, 0, 3);
            ImGui::DragFloat3("Offset", &playerDeathOffset_.x, 0.01f);
            ImGui::DragFloat3("Rotation", &playerDeathRotation_.x, 0.01f);
            ImGui::DragFloat("Length", &playerDeathLength_, 0.01f, 1.0f, 20.0f);
            ImGui::DragFloat("MinLength", &playerDeathMinLength_, 0.01f, 1.0f, 20.0f);
            ImGui::DragFloat("Time", &playerDeathTime_, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("MaxRotationX", &playerDeathMaxRotationX_, 0.01f);
            ImGui::DragFloat("MaxRotationY", &playerDeathMaxRotationY_, 0.01f);

            if (ImGui::Button("ReStartPlayerDeathCamera"))
            {
                PlayerManager::Instance().GetPlayer()->SetHealth(0);
                playerDeathState_ = 0;
            }

            ImGui::TreePop();
        }
#pragma endregion ---------- PlayerDeathCamera ----------

#pragma region ---------- DragonDeathCamera ----------
        if (ImGui::TreeNodeEx("DragonDeathCamera", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::Checkbox("Active", &isDragonDeathCameraActive_);
            ImGui::DragInt("State", &dragonDeathState_, 1, 0, 4);
            ImGui::DragFloat("ChangeFrame", &dragonDeathChangeFrame_);
            if (ImGui::TreeNodeEx("---------- FirstCamera ----------", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::DragFloat3("Offset", &dragonDeathFirstOffset_.x, 0.01f);
                ImGui::DragFloat3("Rotation", &dragonDeathFirstRotation_.x, 0.01f);
                ImGui::DragFloat("Length", &dragonDeathFirstLength_, 0.01f, 1.0f, 20.0f);
                ImGui::DragFloat("Time", &dragonDeathFirstTime_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("MaxRotationY", &dragonDeathFirstMaxRotationY_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- SecondCamera ----------", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::DragFloat3("Rotation", &dragonDeathSecondRotation_.x, 0.01f);
                ImGui::DragFloat("Length", &dragonDeathSecondLength_, 0.01f, 1.0f, 20.0f);
                ImGui::DragFloat("Time", &dragonDeathSecondTime_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("MaxRotationX", &dragonDeathSecondMaxRotationX_, 0.01f);
                ImGui::DragFloat("MaxRotationY", &dragonDeathSecondMaxRotationY_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- ThirdCamera ----------", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::DragFloat3("Rotation", &dragonDeathThirdRotation_.x, 0.01f);
                ImGui::DragFloat("Length", &dragonDeathThirdLength_, 0.01f, 1.0f, 20.0f);
                ImGui::DragFloat("Time", &dragonDeathThirdTime_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("MaxRotationX", &dragonDeathThirdMaxRotationX_, 0.01f);
                ImGui::DragFloat("MaxRotationY", &dragonDeathThirdMaxRotationY_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::Button("ReStartDragonDeathCamera"))
            {
                EnemyManager::Instance().GetEnemy(0)->SetHealth(0.0f);
                EnemyManager::Instance().GetEnemy(0)->PlayAnimation(Enemy::DragonAnimation::Death, false);
                isDragonDeathCameraActive_ = true;
                dragonDeathState_ = 0;
            }

            ImGui::TreePop();
        }
#pragma endregion ---------- DragonDeathCamera ----------

#pragma region ---------- CounterCamera ----------
        if (ImGui::TreeNodeEx("CounterCamera", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragInt("State", &counterState_);
            if(ImGui::TreeNodeEx("CounterCamera", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Checkbox("Active", &isCounterCameraActive_);
                ImGui::DragFloat("Time", &counterTime_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("ReturnTime", &counterReturnTime_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("Length", &counterLength_, 0.01f, 1.0f, 20.0f);
                ImGui::DragFloat("LerpWeight", &counterLerpWegiht_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("MinRotationX", &counterMinRotationX_, 0.01f);


                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("CounterAttackCamera", ImGuiTreeNodeFlags_Framed))
            {
                ImGui::Checkbox("Active", &isCounterAttackCameraActive_);
                ImGui::DragFloat("ZoomTime", &counterAttackZoomTime_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("ZoomLength", &counterAttackZoomLength_, 0.01f, 1.0f, 20.0f);

                ImGui::DragFloat("SlowTime", &counterAttackSlowTime_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("SlowLength", &counterAttackSlowLength_, 0.01f, 1.0f, 20.0f);

                ImGui::DragFloat("Time", &counterAttackTime_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("Length", &counterAttackLength_, 0.01f, 1.0f, 20.0f);
                ImGui::DragFloat("LerpWeight", &counterAttackLerpWeight_, 0.01f, 0.0f, 1.0f);

                ImGui::DragFloat("LengthReturnSpeed", &gameCameraLengthReturnSpeed_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("LerpWeightReturnSpeed", &gameCameraLerpWeightReturnSpeed_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
#pragma endregion ---------- CounterCamera ----------

#pragma region ---------- MageAttackCamera ----------
        if (ImGui::TreeNodeEx("MageAttackCamera", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("ZoomOutTotalFrame", &zoomOutTotalFrame_, 0.01f);
            ImGui::DragFloat("ZoomOutMaxLength", &zoomOutMaxLength_, 0.01f);

            ImGui::DragFloat("ZoomInTotalFrame", &zoomInTotalFrame_, 0.01f);
            ImGui::DragFloat("ZoomInMaxLength", &zoomInMaxLength_, 0.01f);

            ImGui::DragFloat("MageAttackTotalFrame", &mageAttackTotalFrame_, 0.01f);
            ImGui::DragFloat("MageAttackMaxLength", &mageAttackMaxLength_, 0.01f);

            float angle = DirectX::XMConvertToDegrees(mageAttackMinRotationX_);
            ImGui::DragFloat("MinRotation", &angle, 0.01f);
            mageAttackMinRotationX_ = DirectX::XMConvertToRadians(angle);

            ImGui::TreePop();
        }
#pragma endregion ---------- MageAttackCamera ----------

#pragma region ---------- TargetCamera ----------
        if (ImGui::TreeNodeEx("TargetCamera", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat2("StartRotation", &startRotation_.x);
            ImGui::DragFloat2("TargetRotation", &targetRotation_.x);
            ImGui::DragFloat("LerpTimer", &targetCameraLerpTimer_);
            ImGui::DragFloat("LerpSpeed", &targetCameraLerpSpeed_);
            ImGui::Checkbox("Active", &isTargetCameraActive_);

            ImGui::TreePop();
        }

#pragma endregion ---------- TargetCamera ----------

        ImGui::DragFloat3("Target", &target_.x);
        ImGui::DragFloat("LerpWegith", &lerpWeight_, 0.01f, 0.0f, 1.0f);

        ImGui::DragFloat("GroundNearest", &groundNearest_, 0.01f);

        ImGui::DragFloat("NearZ", &nearZ_);
        ImGui::DragFloat("FarZ", &farZ_);

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


        ImGui::DragFloat("Length", &length_, 0.01f);
        ImGui::DragFloat("MinLength", &minLength_, 0.01f);
        ImGui::DragFloat("MaxLength", &maxLength_, 0.01f);

        ImGui::DragFloat("Fov", &fov_, 0.01f);
        ImGui::DragFloat("InputThreshold", &inputThreshold_, 0.1f, 0.0f, 1.0f);
        
        ImGui::DragFloat3("Offset", &offset_.x, 0.01f);

        ImGui::DragFloat("VerticalRotationSpeed", &verticalRotationSpeed_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("HorizontalRotationSpeed", &horizontalRotationSpeed_, 0.01f, 0.0f, 10.0f);

        float minXRotation = DirectX::XMConvertToDegrees(minRotationX_);
        float maxXRotation = DirectX::XMConvertToDegrees(maxRotationX_);
        ImGui::DragFloat("MinXRotation", &minXRotation, 1.0f);
        ImGui::DragFloat("MaxXRotation", &maxXRotation, 1.0f);
        minRotationX_ = DirectX::XMConvertToRadians(minXRotation);
        maxRotationX_ = DirectX::XMConvertToRadians(maxXRotation);

        transform_.DrawDebug();

        ImGui::EndMenu();
    }
}

// ----- タイトル用カメラ設定 -----
void Camera::SetTitleCamera()
{
    GetTransform()->SetRotation(titleCameraRotation_);
    offset_ = titleCameraOffset_;
    length_ = titleCameraLength_;

    target_ = {};
}

// ----- ゲーム用カメラ設定 -----
void Camera::SetGameCamera()
{
}

// ----- ローディング用カメラ設定 -----
void Camera::SetLoadingCamera()
{
    DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f);
    GetTransform()->SetRotation(rotation);

    offset_ = titleCameraOffset_;
    length_ = titleCameraLength_;

    target_ = {};
}

// ----- 回転処理 -----
void Camera::Rotate(const float& elapsedTime)
{
    const float aRX = Input::Instance().GetGamePad().GetAxisRX();
    const float aRY = Input::Instance().GetGamePad().GetAxisRY();
    const float aRYValue = invertVertical_ ? aRY * -1.0f : aRY;
    DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();

    // 横移動は制限なし
    rotation.y += aRX * horizontalRotationSpeed_ * elapsedTime;

    rotation.x += aRYValue * verticalRotationSpeed_ * elapsedTime;
    rotation.x = std::clamp(rotation.x, minRotationX_, maxRotationX_);

    GetTransform()->SetRotation(rotation);
}

// ----- 画面振動 (振動量, 振動時間) 設定 -----
void Camera::ScreenVibrate(const float& vibrationVolume/*振動量*/, const float& vibrationTime/*振動時間*/)
{
    vibrationVolume_ = vibrationVolume;
    vibrationTimer_ = vibrationTime; 
    vibrationTime_ = vibrationTime;
}

// ----- 画面振動 更新 -----
void Camera::ScreenVibrationUpdate(const float& elapsedTime)
{
    // 振動値リセット
    screenVibrationOffset_ = {};

    // 振動がなければ処理しない
    if (vibrationTimer_ <= 0) return;

    // 振動方向の指定(乱数)
    DirectX::XMFLOAT3 vibVec;
    DirectX::XMFLOAT3 right = GetTransform()->CalcRight();
    DirectX::XMFLOAT3 up    = GetTransform()->CalcUp();

    right = right * (rand() % 100 - 50.0f);
    up = up * (rand() % 100 - 50.0f);

    vibVec = { right.x + up.x, right.y + up.y, 0.0f };
    vibVec = XMFloat3Normalize(vibVec);

    // イージングを使い経過時間で振動量を調整する
    float vibrationVolume = Easing::InSine(vibrationTimer_, vibrationTime_, vibrationVolume_, 0.0f);

    // 振動値を入れる
    screenVibrationOffset_ = vibVec * vibrationVolume;

    vibrationTimer_ -= elapsedTime;
}

// ----- カメラから見たスティックの入力値を算出する -----
const DirectX::XMFLOAT2 Camera::ConvertTo2DVectorFromCamera(const DirectX::XMFLOAT2& v)
{
    DirectX::XMFLOAT2 result = {};

    const DirectX::XMFLOAT3 forward = CalcForward();
    const DirectX::XMFLOAT3 right   = CalcRight();

    result.x = forward.x * v.y + right.x * v.x;
    result.y = forward.z * v.y + right.z * v.x;

    return XMFloat2Normalize(result);
}

// ----- 前ベクトル取得 -----
const DirectX::XMFLOAT3 Camera::CalcForward()
{
    return XMFloat3Normalize(view_.focus_ - view_.eye_);
}

// ----- 右ベクトル取得 -----
const DirectX::XMFLOAT3 Camera::CalcRight()
{
    DirectX::XMFLOAT3 forward = CalcForward();
    DirectX::XMFLOAT3 up = { 0, 1, 0 };
    return XMFloat3Normalize(XMFloat3Cross(up, forward));
}

#pragma region ---------- 各種カメラ使用設定 ----------
// ---- プレイヤー死亡時カメラを使用する -----
void Camera::SetUsePlayerDeathCmaera(const float& flag)
{
    isPlayerDeathCameraActive_ = flag;
    playerDeathState_ = 0;
}

// ----- ドラゴンの死亡演出カメラを使用する -----
void Camera::UseDragonDeathCamera()
{
    // 敵死亡カメラ変数を設定
    isDragonDeathCameraActive_ = true;
    dragonDeathState_ = 0;

    // -------------------------------------
    //  現在使用しているカメラをすべて解除する
    // -------------------------------------

}

// ----- カウンターカメラを使用する -----
void Camera::UseCounterCamera()
{
    counterState_          = 0;
    ResetCameraFlags();

    isCounterCameraActive_ = true;
}

// ----- カウンター攻撃カメラを使用する -----
void Camera::UseCounterAttackCamera()
{
    counterState_                = 0;
    ResetCameraFlags();

    isCounterAttackCameraActive_ = true;  // カウンター攻撃カメラを使用
}

void Camera::UseHelmbreakerCamera()
{
    ResetCameraFlags();

    isHelmbreakerCameraActive_   = true;

    helmbreakerCameraState_ = 0;
}

void Camera::UseMageAttackCamera()
{
    ResetCameraFlags();

    isMageAttackCameraAcitve_ = true;
    mageAttackState_ = MageAttackCamera::Initialize;
}

void Camera::ResetCameraFlags()
{
    isCounterCameraActive_          = false; // カウンターカメラ
    isCounterAttackCameraActive_    = false; // カウンター攻撃カメラ
    isHelmbreakerCameraActive_      = false; // 兜割りカメラ
    isMageAttackCameraAcitve_       = false; // 魔法攻撃カメラ
}

#pragma endregion ---------- 各種カメラ使用設定 ----------

// ----- ターゲットカメラ -----
void Camera::UpdateTargetCamera(const float& elapsedTime)
{
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
    {
        DirectX::XMFLOAT3 dragonPosition = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
        DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
        DirectX::XMFLOAT2 targetPosition = DirectX::XMFLOAT2(dragonPosition.x, dragonPosition.z);
        DirectX::XMFLOAT2 startPosition  = DirectX::XMFLOAT2(playerPosition.x, playerPosition.z);
        DirectX::XMFLOAT2 cameraPosition = DirectX::XMFLOAT2(view_.eye_.x, view_.eye_.z);
        DirectX::XMFLOAT2 vec = XMFloat2Normalize(cameraPosition - startPosition);
        DirectX::XMFLOAT2 targetVec = XMFloat2Normalize(startPosition - targetPosition);
        float angle = acosf(std::clamp(XMFloat2Dot(vec, targetVec), -1.0f, 1.0f));
        const float cross = XMFloat2Cross(vec, targetVec);
        if (cross < 0) angle *= -1;
        
        startRotation_ = DirectX::XMFLOAT2(GetTransform()->GetRotationX(), GetTransform()->GetRotationY());
        targetRotation_ = DirectX::XMFLOAT2(DirectX::XMConvertToRadians(-5.0f), GetTransform()->GetRotationY() + angle);

        targetCameraLerpTimer_  = 0.0f;
        isTargetCameraActive_   = true;
    }

    if (isTargetCameraActive_ == false) return;

    targetCameraLerpTimer_ += targetCameraLerpSpeed_ * elapsedTime;
    targetCameraLerpTimer_ = min(targetCameraLerpTimer_, 1.0f);

    const DirectX::XMFLOAT2 rotation = XMFloat2Lerp(startRotation_, targetRotation_, targetCameraLerpTimer_);

    GetTransform()->SetRotationX(rotation.x);
    GetTransform()->SetRotationY(rotation.y);

    if (targetCameraLerpTimer_ == 1.0f) isTargetCameraActive_ = false;
}

// ----- カメラリセット更新 -----
void Camera::UpdateCameraReset(const float& elapsedTime)
{
    // カメラリセット入力判定
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_THUMB)
    {
        // ----- プレイヤーの向きにリセットする -----
        
        // 現在の回転値を保存 & 回転の目的地を設定
        resetOldRotation_ = { GetTransform()->GetRotationX(), GetTransform()->GetRotationY() };
        resetTargetRotation_ = { DirectX::XMConvertToRadians(-5.0f), PlayerManager::Instance().GetTransform()->GetRotationY() };

        // 回転角がある場合は処理をする
        const float rotationDeltaY = resetTargetRotation_.y - resetOldRotation_.y;
        if (rotationDeltaY != 0.0f)
        {
            // 回転角が180度以上の場合は 180度に収まるようにする
            if (rotationDeltaY > DirectX::XM_PI)  resetTargetRotation_.y -= DirectX::XM_2PI;
            if (rotationDeltaY < -DirectX::XM_PI) resetTargetRotation_.y += DirectX::XM_2PI;

            resetLerpTimer_ = 0.0f;
            cameraResetFlag_ = true;
        }
    }

    // フラグが立っていないのでここで終了
    if (cameraResetFlag_ == false) return;

    // Lerpをつかっていい感じに回転させる
    resetLerpTimer_ += resetLerpSpeed_ * elapsedTime;
    resetLerpTimer_ = min(resetLerpTimer_, 1.0f);

    const float rotationX = XMFloatLerp(resetOldRotation_.x, resetTargetRotation_.x, resetLerpTimer_);
    const float rotationY = XMFloatLerp(resetOldRotation_.y, resetTargetRotation_.y, resetLerpTimer_);

    GetTransform()->SetRotationX(rotationX);
    GetTransform()->SetRotationY(rotationY);

    // 終了確認
    if (resetLerpTimer_ == 1.0f) cameraResetFlag_ = false;
}

// ----- ドラゴン登場演出カメラ -----
const bool Camera::UpdateDragonAppearCamera(const float& elapsedTime)
{
    if (isDragonAppearCameraActive_ == false) return false;
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return false;

    target_ = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();

    switch (dragonAppearCameraState_)
    {
    case 0:// 初期化
        length_ = dragonAppearCamera_.initializeLength_;
        oldRotate_ = GetTransform()->GetRotation();
        GetTransform()->SetRotationX(dragonAppearCamera_.initializeRotationX_);

        offset_.y = dragonAppearCamera_.initializeCameraOffsetY_;

        dragonAppearCameraState_ = 1;

        break;
    case 1:
    {
        // ステート変更チェック
        if (EnemyManager::Instance().GetEnemy(0)->GetAnimationIndex() != static_cast<int>(Enemy::DragonAnimation::Fly1))
        {
            dragonAppearCameraState_ = 2;
            return true;
        }

        float dragonAnimationSeconds = EnemyManager::Instance().GetEnemy(0)->GetAnimationSeconds();
        float totalFrame    = 0.0f;
        float currentFrame  = 0.0f;
        if (dragonAnimationSeconds < dragonAppearCamera_.descentStartFrame_)
        {
            totalFrame = dragonAppearCamera_.descentStartFrame_ - dragonAppearCamera_.animationStartFrame_;
            currentFrame = dragonAppearCamera_.descentStartFrame_ - dragonAnimationSeconds;

            // 回転
            const float rotationX = Easing::InSine(currentFrame, totalFrame, dragonAppearCamera_.initializeRotationX_, dragonAppearCamera_.firstTargetRotationX_);
            GetTransform()->SetRotationX(rotationX);

        }
        else if(dragonAnimationSeconds <= dragonAppearCamera_.descentEndFrame_)
        {
            totalFrame = dragonAppearCamera_.descentEndFrame_ - dragonAppearCamera_.descentStartFrame_;
            currentFrame = dragonAppearCamera_.descentEndFrame_ - dragonAnimationSeconds;

            //length_ = Easing::InCubic(currentFrame, totalFrame, dragonAppearCamera_.initializeLength_, dragonAppearCamera_.targetLength_);
            length_ = Easing::InSine(currentFrame, totalFrame, dragonAppearCamera_.initializeLength_, dragonAppearCamera_.firstTargetLength_);

            offset_.y = Easing::InSine(currentFrame, totalFrame, dragonAppearCamera_.initializeCameraOffsetY_, dragonAppearCamera_.firstCameraOffsetY_);
        }
        else
        {
            dragonAppearCamera_.secondTargetLengthTimer_ += elapsedTime;
            dragonAppearCamera_.secondTargetLengthTimer_ = min(dragonAppearCamera_.secondTargetLengthTimer_, dragonAppearCamera_.secondTargetLengthTotalFrame_);
            totalFrame = dragonAppearCamera_.secondTargetLengthTotalFrame_;
            currentFrame = dragonAppearCamera_.secondTargetLengthTimer_;

            length_ = Easing::InQuint(currentFrame, totalFrame, dragonAppearCamera_.secondTargetLength_, dragonAppearCamera_.firstTargetLength_);
            //length_ = Easing::InQuint(currentFrame, totalFrame, dragonAppearCamera_.firstTargetLength_, dragonAppearCamera_.secondTargetLength_);

            offset_.y = Easing::InSine(currentFrame, totalFrame, dragonAppearCamera_.secondCameraOffsetY_, dragonAppearCamera_.firstCameraOffsetY_);
        }
    }
        break;
    case 2:// ドラゴン登場演出カメラ使用終了

        // プレイヤーを操作可にする
        SystemManager::Instance().SetPlayerSlowSpeed(1.0f);
        length_ = gameCameraLength_;
        offset_ = gameCameraOffset_;
        GetTransform()->SetRotation(oldRotate_);

        isDragonAppearCameraActive_ = false;
        break;
    }

    // カメラシェイクを使いたいのでここで処理
    ScreenVibrationUpdate(elapsedTime);

    return true;
}

void Camera::UseDragonAppearCamera()
{
    isDragonAppearCameraActive_ = true;
    dragonAppearCameraState_ = 0;

    dragonAppearCamera_.secondTargetLengthTimer_ = 0.0f;

    // プレイヤーを操作不可にする
    SystemManager::Instance().SetPlayerSlowSpeed(0.0f);
}

void Camera::SetDragonAppearCameraParameter(const float& animationStartFrame, const float& animationEndFrame, const float& descentStartFrame, const float& descentEndFrame)
{
    dragonAppearCamera_.animationStartFrame_    = animationStartFrame;
    dragonAppearCamera_.animationEndFrame_      = animationEndFrame;
    dragonAppearCamera_.descentStartFrame_      = descentStartFrame;
    dragonAppearCamera_.descentEndFrame_        = descentEndFrame;
}

// ----- プレイヤー死亡カメラ -----
const bool Camera::UpdatePlayerDeathCamera(const float& elapsedTime)
{
    // プレイヤー死亡カメラを使用しない
    if (isPlayerDeathCameraActive_ == false) return false;

    target_ = PlayerManager::Instance().GetPlayer()->GetJointPosition("spine_02");
    target_.y = 0.5f;

    switch (playerDeathState_)
    {
    case 0:// 初期化
    {
        // カメラの各種項目を設定する
        length_ = playerDeathLength_;
        offset_ = playerDeathOffset_;
        const float playerRotationY = PlayerManager::Instance().GetTransform()->GetRotationY();
        const DirectX::XMFLOAT3 rotation =
        {
            DirectX::XMConvertToRadians(playerDeathRotation_.x),
            DirectX::XMConvertToRadians(playerDeathRotation_.y) + playerRotationY,
            DirectX::XMConvertToRadians(playerDeathRotation_.z),
        };
        GetTransform()->SetRotation(rotation);
        oldRotate_ = GetTransform()->GetRotation();

        // 初期化
        easingTimer_ = 0.0f;

        // ステート変更
        playerDeathState_ = 1;
    }
        break;
    case 1:
    {
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, playerDeathTime_);

        const float maxRotationX = DirectX::XMConvertToRadians(playerDeathMaxRotationX_);
        const float maxRotationY = DirectX::XMConvertToRadians(playerDeathMaxRotationY_);

        DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
        rotation.x = oldRotate_.x + Easing::OutQuad(easingTimer_, playerDeathTime_, maxRotationX, 0.0f);
        rotation.y = oldRotate_.y + Easing::InSine(easingTimer_, playerDeathTime_, maxRotationY, 0.0f);

        length_ = Easing::OutCubic(easingTimer_, playerDeathTime_, playerDeathMinLength_, playerDeathLength_);

        GetTransform()->SetRotation(rotation);

        if (easingTimer_ == playerDeathTime_)
        {
            return true;
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

// ----- ドラゴン死亡時カメラ -----
const bool Camera::UpdateDragonDeathCamera(const float& elapsedTime)
{
    // ドラゴン死亡カメラを使用しない
    if (isDragonDeathCameraActive_ == false) return false;
    if (EnemyManager::Instance().GetEnemyCount() == 0) return false;
    
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    std::string nodeName = "Dragon15_spine2";
    //std::string nodeName = (dragonDeathState_ >= 3) ? "root" : "Dragon15_spine2";
    target_ = enemy->GetJointPosition(nodeName);

    switch (static_cast<EnemyDeathCamera>(dragonDeathState_))
    {
    case EnemyDeathCamera::Initialize:// 初期化
#pragma region ---------- 初期化 ----------
        // カメラの各種項目を設定する
        length_ = dragonDeathFirstLength_;
        offset_ = dragonDeathFirstOffset_;
        const DirectX::XMFLOAT3 rotation =
        {
            DirectX::XMConvertToRadians(dragonDeathFirstRotation_.x),
            DirectX::XMConvertToRadians(rand() % 360),
            DirectX::XMConvertToRadians(dragonDeathFirstRotation_.z),
        };
        GetTransform()->SetRotation(rotation);
        oldRotate_ = GetTransform()->GetRotation();

        // ステート遷移フレームを設定
        dragonDeathChangeFrame_ = dragonDeathFirstTime_; 
        // 初期化
        easingTimer_ = 0.0f;
        dragonDeathtimerActive_ = false;

        // ステート変更
        SetState(EnemyDeathCamera::FirstCamera);
#pragma endregion ---------- 初期化 ----------
        break;
    case EnemyDeathCamera::FirstCamera:// １つ目のカメラ
#pragma region ---------- １つ目のカメラ ----------
    {
        // タイマー加算＆上限管理
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, dragonDeathFirstTime_);

        // 回転処理
        const float maxRotateY = DirectX::XMConvertToRadians(dragonDeathFirstMaxRotationY_);
        const float rotateY = oldRotate_.y + Easing::InSine(easingTimer_, dragonDeathFirstTime_, maxRotateY, 0.0f);
        GetTransform()->SetRotationY(rotateY);

        // 指定のフレームを超えたら次のステートへ
        if (enemy->GetAnimationSeconds() > dragonDeathChangeFrame_)
        {
            // カメラの初期位置を設定する
            length_ = dragonDeathSecondLength_;
            const DirectX::XMFLOAT3 rotation =
            {
                DirectX::XMConvertToRadians(dragonDeathSecondRotation_.x),
                DirectX::XMConvertToRadians(rand() % 360),
                DirectX::XMConvertToRadians(dragonDeathSecondRotation_.z),
            };
            GetTransform()->SetRotation(rotation);
            oldRotate_ = GetTransform()->GetRotation();

            // ステート遷移フレームを設定
            dragonDeathChangeFrame_ += dragonDeathSecondTime_;

            // 初期化
            easingTimer_ = 0.0f;

            // ステート変更
            SetState(EnemyDeathCamera::SecondCamera);
        }
    }
#pragma endregion ---------- １つ目のカメラ ----------
        break;
    case EnemyDeathCamera::SecondCamera:// ２つ目のカメラ
#pragma region ---------- ２つ目のカメラ ----------
    {
        // タイマー加算＆上限管理
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, dragonDeathSecondTime_);

        // 回転処理
        const float maxRotateX = DirectX::XMConvertToRadians(dragonDeathSecondMaxRotationX_);
        const float maxRotateY = DirectX::XMConvertToRadians(dragonDeathSecondMaxRotationY_);
        DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
        rotation.x = oldRotate_.x + Easing::InSine(easingTimer_, dragonDeathSecondTime_, maxRotateX, 0.0f);
        rotation.y = oldRotate_.y + Easing::InSine(easingTimer_, dragonDeathSecondTime_, maxRotateY, 0.0f);
        GetTransform()->SetRotation(rotation);

        // 指定のフレームを超えたら次のステートへ
        if (enemy->GetAnimationSeconds() > dragonDeathChangeFrame_)
        {
            // カメラの初期位置を設定する
            length_ = dragonDeathThirdLength_;

            const DirectX::XMFLOAT3 rotation =
            {
                DirectX::XMConvertToRadians(dragonDeathThirdRotation_.x),
                DirectX::XMConvertToRadians(dragonDeathThirdRotation_.y),
                DirectX::XMConvertToRadians(dragonDeathThirdRotation_.z),
            };
            GetTransform()->SetRotation(rotation);
            oldRotate_ = GetTransform()->GetRotation();

            // ステート遷移フレームを設定
            dragonDeathChangeFrame_ += dragonDeathThirdTime_;

            // 初期化
            easingTimer_ = 0.0f;

            // ステート変更
            SetState(EnemyDeathCamera::ThirdCamera);
        }
    }
#pragma endregion ---------- ２つ目のカメラ ----------
        break;
    case EnemyDeathCamera::ThirdCamera:// ３つ目のカメラ
#pragma region ---------- ３つ目のカメラ ----------
    {
        // タイマー加算＆上限管理
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, dragonDeathThirdTime_);

        // 回転処理
        DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
        const float maxRotateX = DirectX::XMConvertToRadians(dragonDeathThirdMaxRotationX_);
        const float maxRotateY = DirectX::XMConvertToRadians(dragonDeathThirdMaxRotationY_);
        rotation.x = oldRotate_.x + Easing::InSine(easingTimer_, dragonDeathThirdTime_, maxRotateX, 0.0f);
        rotation.y = oldRotate_.y + Easing::InSine(easingTimer_, dragonDeathThirdTime_, maxRotateY, 0.0f);
        GetTransform()->SetRotation(rotation);
        
        // -------------------------------------------------------
        //  死亡アニメーションが終了後,少しの時間カメラを回転させておく
        // -------------------------------------------------------
        // 死亡アニメーションが終了した
        if (enemy->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::Death) &&
            enemy->IsPlayAnimation() == false)
        {
            // 死亡ループモーションを再生
            enemy->PlayAnimation(Enemy::DragonAnimation::DeathLoop, true);

            // 死亡時タイマーを使用する
            dragonDeathtimerActive_ = true;
            dragonDeathTimer_ = 0.0f;
        }
        // 少しの間を作るためのタイマー
        if (dragonDeathtimerActive_)
        {
            dragonDeathTimer_ += elapsedTime;
            
            // 設定した時間を超えたらそのまま放置状態に入る
            const float maxTime = 1.0f;
            if (dragonDeathTimer_ > maxTime)
            {
                // カメラをもとに戻す
                GetTransform()->SetRotation({ 0, 0, 0 });
                offset_ = gameCameraOffset_;
                length_ = gameCameraLength_;

                // 敵死亡時カメラを使用終了
                isDragonDeathCameraActive_ = false;

                AudioManager::Instance().StopBGM(BGM::GameClearJingle);
                AudioManager::Instance().PlayBGM(BGM::GameClear);

                // ステート変更
                SetState(EnemyDeathCamera::Death);
            }
        }
    }
#pragma endregion ---------- ３つ目のカメラ ----------
        break;
    case EnemyDeathCamera::Death:// 死亡ループ
        // 死亡しているので何もしない
        return false;
        break;
    }

    return true;
}

#pragma region ---------- カウンターカメラ ----------
// ----- カウンターカメラ -----
void Camera::UpdateCounterCamera(const float& elapsedTime)
{
    // カウンターカメラを使用しない
    if (isCounterCameraActive_ == false) return;

    lerpWeight_ = counterLerpWegiht_;

    switch (counterState_)
    {
    case 0:// 初期化
#pragma region ---------- 初期化 ----------
        easingTimer_ = 0.0f;

        oldRotate_ = GetTransform()->GetRotation();

        counterState_ = 1;
#pragma endregion ---------- 初期化 ----------
        break;
    case 1:// カメラを引く
#pragma region ---------- カメラを引く ----------
    {
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, counterTime_);

        const float minRotationX = DirectX::XMConvertToRadians(counterMinRotationX_);
        if (oldRotate_.x < minRotationX)
        {
            const float rotationX = Easing::InSine(easingTimer_, counterTime_, minRotationX, oldRotate_.x);
            GetTransform()->SetRotationX(rotationX);
        }

        length_ = Easing::InSine(easingTimer_, counterTime_, counterLength_, gameCameraLength_);

        if (easingTimer_ == counterTime_)
        {
            minRotationX_ = DirectX::XMConvertToRadians(counterMinRotationX_);
            counterState_ = 2;
        }
    }
#pragma endregion ---------- カメラを引く ----------
        break;
    case 2:// 待機
#pragma region ---------- 待機 ----------
        if (PlayerManager::Instance().GetPlayer()->GetCurrentState() != Player::STATE::Counter)
        {
            easingTimer_ = 0.0f;
            counterState_ = 3;
        }
#pragma endregion ---------- 待機 ----------
        break;
    case 3:// カメラを戻す
#pragma region ---------- カメラを戻す ----------
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, counterReturnTime_);

        length_ = Easing::InSine(easingTimer_, counterReturnTime_, gameCameraLength_, counterLength_);

        if (easingTimer_ == counterReturnTime_)
        {
            minRotationX_ = gameCameraMinRotationX_;
            easingTimer_ = 0.0f;
            counterState_ = 0;
            isCounterCameraActive_ = false;
        }
#pragma endregion ---------- カメラを戻す ----------
        break;
    }
}

// ----- カウンター攻撃カメラ -----
void Camera::UpdateCounterAttackCamera(const float& elapsedTime)
{
    // カウンター攻撃カメラを使用しない
    if (isCounterAttackCameraActive_ == false) return;

    lerpWeight_ = counterAttackLerpWeight_;

    switch (counterState_)
    {
    case 0:// 初期化
#pragma region ---------- 初期化 ----------
        easingTimer_ = 0.0f;

        oldLength_ = length_;

        counterState_ = 1;
#pragma endregion ---------- 初期化 ----------
        break;
    case 1:
#pragma region ---------- カメラを寄せる ----------
        lerpWeight_ = gameCameraLerpWeight_ * 0.5f;
        
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, counterAttackZoomTime_);

        length_ = Easing::InSine(easingTimer_, counterAttackZoomTime_, counterAttackZoomLength_, oldLength_);
        
        if (easingTimer_ == counterAttackZoomTime_)
        {
            oldLength_ = length_;
            easingTimer_ = 0.0f;
            counterState_ = 2;
        }

#pragma endregion ---------- カメラを寄せる ----------
        break;
    case 2:
        lerpWeight_ = gameCameraLerpWeight_ * 0.5f;

        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, counterAttackSlowTime_);

        length_ = Easing::InSine(easingTimer_, counterAttackSlowTime_, counterAttackSlowLength_, oldLength_);

        if (easingTimer_ == counterAttackSlowTime_)
        {
            oldLength_ = length_;
            easingTimer_ = 0.0f;
            counterState_ = 3;
        }

        break;
    case 3:
#pragma region ---------- カメラを引く ----------
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, counterAttackTime_);

        length_ = Easing::InSine(easingTimer_, counterAttackTime_, counterAttackLength_, oldLength_);

        if (easingTimer_ == counterAttackTime_)
        {
            counterState_ = 4;
        }
#pragma endregion ---------- カメラを引く ----------
        break;
    case 4:
#pragma region ---------- ----------
    {

        const float animationSeconds = PlayerManager::Instance().GetPlayer()->GetAnimationSeconds();

        if (animationSeconds > 1.6f)
        {
            minRotationX_ = gameCameraMinRotationX_;

            lerpWeight_ = gameCameraLerpWeight_ * gameCameraLerpWeightReturnSpeed_;
            isCounterAttackCameraActive_ = false;
        }
    }
#pragma endregion ---------- ----------
        break;
    }
}

#pragma endregion ---------- カウンターカメラ ----------

// ----- 魔法攻撃カメラ -----
void Camera::UpdateMageAttackCamera(const float& elapsedTime)
{
    // 魔法攻撃カメラを使わない
    if (isMageAttackCameraAcitve_ == false) return;

    switch (mageAttackState_)
    {
    case MageAttackCamera::Initialize:// 初期化
        easingTimer_    = 0.0f;
        oldLength_      = length_;
        oldRotate_      = GetTransform()->GetRotation();

        mageAttackState_ = MageAttackCamera::ZoomOut;

        break;
    case MageAttackCamera::Idle:// 待機

        if (PlayerManager::Instance().GetPlayer()->GetAnimationIndex() == static_cast<int>(Player::Animation::MageAttack1_3Loop))
        {
            mageAttackState_ = MageAttackCamera::ZoomOut;
        }

        break;
    case MageAttackCamera::ZoomOut:// カメラを引く
    {
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, zoomOutTotalFrame_);

        length_         = Easing::InSine(easingTimer_, zoomOutTotalFrame_, zoomOutMaxLength_, oldLength_);
        minRotationX_   = Easing::InSine(easingTimer_, zoomOutTotalFrame_, zoomOutMinRotationX_, oldRotate_.x);

        if (easingTimer_ == zoomOutTotalFrame_)
        {
            easingTimer_    = 0.0f;
            oldLength_      = length_;
            oldRotate_      = GetTransform()->GetRotation();

            mageAttackState_ = MageAttackCamera::ZoomIn;
        }
    }
        break;
    case MageAttackCamera::ZoomIn:// カメラを近づける
    {
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, zoomInTotalFrame_);

        length_ = Easing::OutSine(easingTimer_, zoomInTotalFrame_, zoomInMaxLength_, oldLength_);

        const Player::Animation playerAnimationIndex = static_cast<Player::Animation>(PlayerManager::Instance().GetPlayer()->GetAnimationIndex());
        if (playerAnimationIndex == Player::Animation::MageAttack1_3End)
        {
            const float animationSeconds = PlayerManager::Instance().GetPlayer()->GetAnimationSeconds();
            if (animationSeconds >= 0.3f)
            {
                easingTimer_ = 0.0f;
                oldLength_ = length_;
                oldRotate_ = GetTransform()->GetRotation();

                mageAttackState_ = MageAttackCamera::Attack;
            }
        }
    }
        break;
    case MageAttackCamera::Attack:
    {
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, mageAttackTotalFrame_);

        length_ = Easing::InSine(easingTimer_, mageAttackTotalFrame_, mageAttackMaxLength_, oldLength_);

        // アニメーション終了していたら
        const Player::Animation playerAnimationIndex = static_cast<Player::Animation>(PlayerManager::Instance().GetPlayer()->GetAnimationIndex());
        if (playerAnimationIndex != Player::Animation::MageAttack1_3End)
        {
            easingTimer_ = 0.0f;
            oldLength_ = length_;
            oldRotate_ = GetTransform()->GetRotation();

            mageAttackState_ = MageAttackCamera::Finalize;
        }
    }
        break;
    case MageAttackCamera::Finalize:// 終了化
    {
        const float totalFrame = 0.5f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        length_ = Easing::InSine(easingTimer_, totalFrame, gameCameraLength_, oldLength_);

        minRotationX_ = Easing::InSine(easingTimer_, totalFrame, gameCameraMinRotationX_, mageAttackMinRotationX_);

        if (easingTimer_ == totalFrame)
        {
            minRotationX_ = gameCameraMinRotationX_;

            isMageAttackCameraAcitve_ = false;
        }
    }
        break;
    }
}

// ----- 兜割りカメラ -----
void Camera::UpdateHelmbreakerCamera(const float& elapsedTime)
{
    // 兜割りカメラを使わない
    if (isHelmbreakerCameraActive_ == false) return;

    lerpWeight_ = helmbreakerCameraLerpWeight_;

    switch (helmbreakerCameraState_)
    {
    case 0:// 初期化
        easingTimer_ = 0.0f;
        oldLength_ = length_;
        helmbreakerCameraState_ = 1;

        break;
    case 1:
    {
        const float playerAnimationSeconds = PlayerManager::Instance().GetPlayer()->GetAnimationSeconds();        

        if(PlayerManager::Instance().GetPlayer()->GetAnimationIndex() != static_cast<int>(Player::Animation::Attack4_0))
        {
            helmbreakerCameraState_ = 2;
        }
        else
        {
            length_ = Easing::InSine(playerAnimationSeconds, 0.4f, 5.5f, oldLength_);
        }
    }
        break;
    case 2:

        if (PlayerManager::Instance().GetPlayer()->GetCurrentState() != Player::STATE::Helmbreaker)
        {
            isHelmbreakerCameraActive_ = false;
        }

        break;
    }
}