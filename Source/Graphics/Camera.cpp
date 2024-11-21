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

// ----- 初期化 -----
void Camera::Initialize()
{
    isDragonDeathCameraActive_ = false;

    dragonDeathState_ = 0;
    playerDeathState_ = 0;
    counterState_ = 0;


    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    
    GetTransform()->SetRotationX(DirectX::XMConvertToRadians(10.0f));
    offset_         = gameCameraOffset_;
    length_         = gameCameraLength_;

    lerpTimer_ = 0.0f;

    // ----- ロックオンカメラ -----
    targetJointName_.emplace_back("Dragon15_head");
    targetJointName_.emplace_back("Dragon15_spine2");
    targetJointName_.emplace_back("Dragon15_tail_02");
    currentTargetJointIndex_ = 0;
    useLockonCamera_ = false;
}

// ----- 更新 -----
void Camera::Update(const float& elapsedTime)
{
    // タイトル画面はとりあえずここで終了
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title ||
        SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Loading) return;

    // TODO:各種死亡カメラが使用されているときはカメラシェイクを行わない
    if (isPlayerDeathCameraActive_ || isDragonDeathCameraActive_)
    {
        ScreenVibrate(0.0f, 0.0f);
    }

    // 死亡カメラ使用時はここで終了
    if (UpdatePlayerDeathCamera(elapsedTime)) return; // Player死亡カメラ
    if (UpdateDragonDeathCamera(elapsedTime))  return; // Enemy死亡カメラ

    const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };

    target_ = XMFloat3Lerp(target_, cameraTargetPosition, lerpWeight_);

    if (PlayerManager::Instance().GetPlayer()->GetCurrentState() == Player::STATE::CounterCombo)
    {
        lerpWeight_ = 0.0f;
    }
    else
    {
        lerpWeight_ = 0.12f;
    }


    // ロックオンカメラ
    UpdateLockonCamera(elapsedTime);

    if(useLockonCamera_)
    {
        hitWallLerpTimer_ = isHitWall_ ? hitWallLerpTimer_ + elapsedTime * 2.0f : hitWallLerpTimer_ - elapsedTime * 2.0f;

        hitWallLerpTimer_ = min(1.0f, hitWallLerpTimer_);
        hitWallLerpTimer_ = max(0.0f, hitWallLerpTimer_);
    }

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

    if (useLockonCamera_)
    {
        DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(GetCurrentTargetJointName());
        const float targetLengthY = fabsf(groundNearest_ - targetPosition.y);
        const float currentLengthY = fabsf(view_.eye_.y - targetPosition.y);

        DirectX::XMFLOAT3 vec = XMFloat3Normalize(view_.eye_ - view_.focus_) * minLength_;
        vec = view_.focus_ + vec;

        if (vec.y < groundNearest_)
        {
            // 地面に埋まっているため
            length_ = (length_ * targetLengthY) / currentLengthY;
        }
        else
        {
            if (length_ < minLength_)
            {
                length_ = XMFloatLerp(length_, maxLength_, 0.5f);
            }
        }
    }

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

            if (useLockonCamera_)
            {
                DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(GetCurrentTargetJointName());
                view_.focus_ = XMFloat3Lerp(view_.focus_, targetPosition, hitWallLerpTimer_);
            }

            isHitWall_ = true;
        }
        else
        {
            if (useLockonCamera_)
            {
                DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(GetCurrentTargetJointName());
                view_.focus_ = XMFloat3Lerp(view_.focus_, targetPosition, hitWallLerpTimer_);
            }

            isHitWall_ = false;
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
            if (ImGui::Button("ReStartDragonDeathCamera")) dragonDeathState_ = 0;

            ImGui::TreePop();
        }


        ImGui::DragFloat3("Target", &target_.x);
        ImGui::DragFloat("LerpWegith", &lerpWeight_, 0.01f, 0.0f, 1.0f);

        ImGui::DragFloat("GroundNearest", &groundNearest_, 0.01f);

        ImGui::DragFloat("NearZ", &nearZ_);
        ImGui::DragFloat("FarZ", &farZ_);

        if (ImGui::TreeNode("LockonCamera"))
        {
            ImGui::BulletText(targetJointName_.at(currentTargetJointIndex_).c_str());
            ImGui::DragFloat("LockOnRotationSpeed", &lockOnRotationSpeed_, 0.1f);
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
        
        ImGui::DragFloat3("Offset", &offset_.x, 0.01f);

        ImGui::DragFloat("VerticalRotationSpeed", &verticalRotationSpeed_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("HorizontalRotationSpeed", &horizontalRotationSpeed_, 0.01f, 0.0f, 10.0f);

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

// ----- タイトル用カメラ設定 -----
void Camera::SetTitleCamera()
{
    //GetTransform()->SetRotationX(DirectX::XMConvertToRadians(16.0f));
    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180.0f));

    target_ = {};
}

// ----- ゲーム用カメラ設定 -----
void Camera::SetGameCamera()
{
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
    rotation.x = std::clamp(rotation.x, minXRotation_, maxXRotation_);

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

// ---- 自機死亡時カメラを使用する -----
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

    // ロックオンカメラを解除する。ロックオンUIも削除する
    useLockonCamera_ = false;
    UIManager::Instance().Remove(UIManager::UIType::UICrosshair);

}

// ----- カウンター時カメラを使用する -----
void Camera::SetUseCounterCamera()
{
    useCounterCamera_ = true;
    counterState_ = 0;
}

// ----- ロックオンカメラ更新 -----
void Camera::UpdateLockonCamera(const float& elapsedTime)
{
    // ロックオンする敵が存在しない
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;
    // ドラゴンが死んでいるためロックオンできない
    if (EnemyManager::Instance().GetEnemy(0)->GetIsDead()) return;

    // ロックオン入力判定
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_THUMB)
    {
        // 現在ロックオンしていない場合
        if (useLockonCamera_ == false)
        {
            // クロスヘアUIを生成する
            UICrosshair* uiCrosshair = new UICrosshair();

            // ロックオンするジョイントを変更可能にする
            isNextJointAccessible = true;
        }
        // 現在ロックオンしている場合
        else
        {
            // 現在使用しているクロスヘアUIを削除する
            UIManager::Instance().Remove(UIManager::UIType::UICrosshair);
        }

        useLockonCamera_ = !useLockonCamera_;
    }

    // ロックオンしていないのでここで終了
    if (useLockonCamera_ == false) return;

    // ロックオンするジョイントの切り替え判定
    const float aRx = Input::Instance().GetGamePad().GetAxisRX();
    // ジョイントを切り替えれる場合
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
    // ジョイントを切り替えれない場合
    else
    {
        // 入力がなくなったら他のジョイントへアクセスできるようになる
        if (aRx == 0.0f) isNextJointAccessible = true;
    }


    DirectX::XMFLOAT3 cameraPosition = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 playerHeadPosition = playerPosition;
    playerHeadPosition.y = 1.7f;
    DirectX::XMFLOAT3 targetPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(GetCurrentTargetJointName());

    {
        // XZ平面での処理
        DirectX::XMFLOAT3 cameraToPlayer = playerPosition - cameraPosition;
        DirectX::XMFLOAT3 playerToEnemy = targetPosition - playerPosition;

        DirectX::XMFLOAT2 vec0 = XMFloat2Normalize({ cameraToPlayer.x, cameraToPlayer.z });
        DirectX::XMFLOAT2 vec1 = XMFloat2Normalize({ playerToEnemy.x, playerToEnemy.z });

        const float angle = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenNormals(DirectX::XMLoadFloat2(&vec0), DirectX::XMLoadFloat2(&vec1)));

        float cross = XMFloat2Cross(vec0, vec1);

        if (cross < 0)
        {
            GetTransform()->AddRotationY(-angle * lockOnRotationSpeed_);
        }
        else
        {
            GetTransform()->AddRotationY(angle * lockOnRotationSpeed_);
        }
    }

    {
        DirectX::XMFLOAT3 cameraToPlayer = playerHeadPosition - cameraPosition;
        DirectX::XMFLOAT3 playerToEnemy = targetPosition - playerHeadPosition;
        DirectX::XMFLOAT2 vec0 = XMFloat2Normalize({ cameraToPlayer.y, XMFloat2Length({cameraToPlayer.x, cameraToPlayer.z}) });
        DirectX::XMFLOAT2 vec1 = XMFloat2Normalize({ playerToEnemy.y, XMFloat2Length({playerToEnemy.x, playerToEnemy.z}) });

        float angle = DirectX::XMVectorGetX(DirectX::XMVector2AngleBetweenNormals(DirectX::XMLoadFloat2(&vec0), DirectX::XMLoadFloat2(&vec1)));


        float cross = XMFloat2Cross(vec0, vec1);

        if (cross < 0)
        {
            GetTransform()->AddRotationX(-angle * lockOnRotationSpeed_);
        }
        else
        {
            GetTransform()->AddRotationX(angle * lockOnRotationSpeed_);
        }
    }


    // 角度調整
    DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
    if (rotation.y > DirectX::XM_2PI) rotation.y -= DirectX::XM_2PI;
    if (rotation.y < 0.0f)            rotation.y += DirectX::XM_2PI;
    GetTransform()->SetRotation(rotation);
}

// ----- カメラリセット更新 -----
void Camera::UpdateCameraReset(const float& elapsedTime)
{
    return;

    // ロックオンカメラを使用しているのでカメラリセットは使えない
    if (useLockonCamera_) return;

    // カメラリセット入力判定
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
    {
        // ----- プレイヤーの向きにリセットする -----
        
        // 現在の回転値を保存 & 回転の目的地を設定
        resetOldRotation_ = { GetTransform()->GetRotationX(), GetTransform()->GetRotationY() };
        resetTargetRotation_ = { DirectX::XMConvertToRadians(10.0f), PlayerManager::Instance().GetTransform()->GetRotationY() };

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
            useDeathTimer_ = true;
        }
        // 少しの間を作るためのタイマー
        if (useDeathTimer_)
        {
            deathTimer_ += elapsedTime;
            
            // 設定した時間を超えたらそのまま放置状態に入る
            const float maxTime = 1.0f;
            if (deathTimer_ > maxTime)
            {
                // カメラをもとに戻す
                GetTransform()->SetRotation({ 0, 0, 0 });
                offset_ = gameCameraOffset_;
                length_ = gameCameraLength_;

                // 敵死亡時カメラを使用終了
                isDragonDeathCameraActive_ = false;

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

// ----- カウンター攻撃時のカメラ更新 -----
const bool Camera::UpdateCounterAttackCamera(const float& elapsedTime)
{
    // カウンター時カメラ使用フラグが立っていないのでここで終了
    if (useCounterCamera_ == false) return false;

    Player* player = PlayerManager::Instance().GetPlayer().get();

    switch (static_cast<CounterAttackCamera>(counterState_))
    {
    case CounterAttackCamera::CounterInitialize:// カウンター初期化
        // 現在のカメラ項目を保存する
        oldLength_ = length_;
        oldRotate_ = GetTransform()->GetRotation();

        // 変数初期化
        easingTimer_ = 0.0f;

        // ステート変更
        SetState(CounterAttackCamera::CounterZoomOut);

        break;
    case CounterAttackCamera::CounterZoomOut:// カメラを引く
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
            // 変数初期化
            easingTimer_ = 0.0f;

            // ステート変更
            SetState(CounterAttackCamera::CounterIdle);
        }
    }
        break;
    case CounterAttackCamera::CounterIdle:// 次の行動待機
    {
        const Player::STATE currentState = player->GetCurrentState();

        if (currentState == Player::STATE::Idle ||
            currentState == Player::STATE::Damage)
        {
            // ステート変更
            SetState(CounterAttackCamera::CounterFinalize);
        }
        if (currentState == Player::STATE::CounterCombo)
        {
            // ステート変更
            SetState(CounterAttackCamera::CounterComboInitialize);
        }
    }
        break;
    case CounterAttackCamera::CounterFinalize:// カウンター終了化
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
            // 変数初期化
            easingTimer_ = 0.0f;

            // カウンターカメラ使用終了
            useCounterCamera_ = false;
        }
    }
        break;
    case CounterAttackCamera::CounterComboInitialize:// カウンターコンボ初期化

        // 現在のカメラの項目を保存する
        oldLength_ = length_;
        oldRotate_ = GetTransform()->GetRotation();

        // 変数初期化
        easingTimer_ = 0.0f;

        counterDelayTimer_ = 0.0f;

        // ステート変更
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

            // ステート変更
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

            // ステート変更
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

                // length設定
                length_ = XMFloatLerp(9.0f, 6.5f, counterLerpTimer_);

                // rotationX設定

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
