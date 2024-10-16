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
    useEnemyDeathCamera_ = false;

    enemyDeathstate_ = 0;
    playerDeathState_ = 0;
    counterState_ = 0;


    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    //GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));

    // TODO:かめら
    // ここからテストしてるだけなので変える多分
    targetOffset_ = { 0, -1, 0 };
    cameraOffset_ = { 0, 2.5f, 0 };
    length_ = 6.0f;
    verticalRotationSpeed_ = 1.0f;
    horizontalRotationSpeed_ = 4.0f;

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
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title) return;

    // 死亡カメラ使用時はここで終了
    if (UpdatePlayerDeathCamera(elapsedTime)) return; // Player死亡カメラ
    if (UpdateEnemyDeathCamera(elapsedTime))  return; // Enemy死亡カメラ

    const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    
#if 1
    if (isCounterDelay_ == false) target_ = cameraTargetPosition;

    if (PlayerManager::Instance().GetPlayer()->GetCurrentState() == Player::STATE::Skill)
    {
        
    }

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

    // ドラゴンの上昇攻撃のカメラ更新
    //if (UpdateRiseAttackCamera(elapsedTime)) return;

    // カウンター攻撃のカメラ更新
    if (UpdateCounterAttackCamera(elapsedTime)) return;

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
    DirectX::XMFLOAT3 front = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rot = GetTransform()->GetRotation();
    view_.eye_.x = target_.x + cameraOffset_.x + front.x - length_ * cosf(rot.x) * sinf(rot.y);
    view_.eye_.y = target_.y + cameraOffset_.y + front.y - length_ * sinf(rot.x);
    view_.eye_.z = target_.z + cameraOffset_.z + front.z - length_ * cosf(rot.x) * cosf(rot.y);
    view_.focus_ = target_ + cameraOffset_ + targetOffset_;

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

void Camera::DrawDebug()
{
    if (ImGui::BeginMenu("Camera"))
    {
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

// ----- 回転処理 -----
void Camera::Rotate(const float& elapsedTime)
{
    if (useLockonCamera_) return;

    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    // 横移動は制限なし
    rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

    // length制御
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;    

    // 理不尽な縦回転を入れないように閾値以上なら入力を取得する
    if (fabs(aRy) >= inputThreshold_)
    {
        // 上下入力反転を判定
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

    // X軸(上下)の回転制御
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;

    GetTransform()->SetRotation(rotate);
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
    return XMFloat3Cross(up, forward);
}

// ---- 自機死亡時カメラを使用する -----
void Camera::SetUsePlayerDeathCmaera(const float& flag)
{
    usePlayerDeathCamera_ = flag;
    playerDeathState_ = 0;
}

// ----- 敵死亡時カメラ使用する -----
void Camera::SetUseEnemyDeathCamera()
{
    // 敵死亡カメラ変数を設定
    useEnemyDeathCamera_ = true;
    enemyDeathstate_ = 0;

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


    // プレイヤーと敵のジョイントの間をtargetに設定する


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



        ////if (fabsf(angle) > DirectX::XM_PIDIV2)
        ////{
        ////    angle = (angle > 0.0f) ? angle - DirectX::XM_PI : angle + DirectX::XM_PI;
        ////}

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

    // length 6.05
    {
#if 0
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
#endif

        //float rotationX = GetTransform()->GetRotationX();

        //if (rotationX > maxXRotation_)
        //{
        //    float minRotation = maxXRotation_;
        //    constexpr float maxRotate = DirectX::XMConvertToRadians(40);

        //    //const float addAmountSpeed = 2.0f;
        //    const float rotateAmount = (maxRotate - minRotation);
        //    const float lengthAmount = (minLength_ - 2.5f);
        //    
        //    float value = (rotationX - minRotation) / rotateAmount;

        //    float lenght = lengthAmount * value;

        //    length_ = lenght;            
        //}


    }

#if 0
    // 横移動は制限なし
    rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

    // length制御
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;

    // 理不尽な縦回転を入れないように閾値以上なら入力を取得する
    if (fabs(aRy) >= inputThreshold_)
    {
        // 上下入力反転を判定
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
#endif

    // 角度調整
    DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
    if (rotation.y > DirectX::XM_2PI) rotation.y -= DirectX::XM_2PI;
    if (rotation.y < 0.0f)            rotation.y += DirectX::XM_2PI;
    GetTransform()->SetRotation(rotation);
}

// ----- カメラリセット更新 -----
void Camera::UpdateCameraReset(const float& elapsedTime)
{
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

// ----- 自機死亡カメラ -----
const bool Camera::UpdatePlayerDeathCamera(const float& elapsedTime)
{
    // 自機死亡時カメラの使用フラグが立っていないのでここで終了
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

// ----- 敵死亡時カメラ -----
const bool Camera::UpdateEnemyDeathCamera(const float& elapsedTime)
{
    // 敵死亡時カメラの使用フラグが立っていないのでここで終了
    if (useEnemyDeathCamera_ == false) return false;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    std::string nodeName = (enemyDeathstate_ >= 3) ? "root" : "Dragon15_spine2";
    target_ = enemy->GetJointPosition(nodeName);

    switch (static_cast<EnemyDeathCamera>(enemyDeathstate_))
    {
    case EnemyDeathCamera::Initialize:// 初期化
#pragma region ---------- 初期化 ----------
        // アニメーション設定
        enemy->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

        // 押し出し判定を無効化する
        PlayerManager::Instance().SetUseCollisionDetection(false);

        // カメラの各種項目を設定する
        targetOffset_   = { 0.0f, -7.0f, 0.0f };
        cameraOffset_   = { 0.0f, 7.0f, 0.0f };
        length_         = 10.0f;
        GetTransform()->SetRotation({ 0.0f, DirectX::XMConvertToRadians(rand() % 360), 0.0f });
        oldRotate_ = GetTransform()->GetRotation();

        // 変数初期化
        easingTimer_ = 0.0f;

        // ステート変更
        SetState(EnemyDeathCamera::FirstCamera);
#pragma endregion ---------- 初期化 ----------
        break;
    case EnemyDeathCamera::FirstCamera:// １つ目のカメラ
#pragma region ---------- １つ目のカメラ ----------
    {
        // タイマー加算＆上限管理
        const float totalFrame = 2.0f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // 回転処理
        constexpr float minRotateY = DirectX::XMConvertToRadians(0.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(-40.0f);
        const float rotateY = oldRotate_.y + Easing::InSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotationY(rotateY);

        // 指定のフレームを超えたら次のステートへ
        const float changeStateFrame = 2.0f;
        if (enemy->GetAnimationSeconds() > changeStateFrame)
        {
            // カメラの初期位置を設定する
            GetTransform()->SetRotation({ 0.0f, DirectX::XMConvertToRadians(rand() % 360), 0.0f });
            oldRotate_ = GetTransform()->GetRotation();

            // 変数初期化
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
        const float totalFrame = 2.2f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // 回転処理
        constexpr float minRotateX = DirectX::XMConvertToRadians(20.0f);
        constexpr float maxRotateX = DirectX::XMConvertToRadians(30.0f);
        constexpr float minRotateY = DirectX::XMConvertToRadians(0.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(30.0f);
        DirectX::XMFLOAT3 cameraRotate = {};
        cameraRotate.x = Easing::InSine(easingTimer_, totalFrame, maxRotateX, minRotateX);
        cameraRotate.y = oldRotate_.y + Easing::InSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotation(cameraRotate);

        // 指定のフレームを超えたら次のステートへ
        const float changeStateFrame = 4.4f;
        if (enemy->GetAnimationSeconds() > changeStateFrame)
        {
            // カメラの初期位置を設定する
            length_ = 6.5f;
            GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-30.0f));

            // 変数初期化
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
        const float totalFrame = 2.5f;
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // 回転処理
        constexpr float minRotateY = DirectX::XMConvertToRadians(250.0f);
        constexpr float maxRotateY = DirectX::XMConvertToRadians(220.0f);
        const float rotateY = Easing::OutSine(easingTimer_, totalFrame, maxRotateY, minRotateY);
        GetTransform()->SetRotationY(rotateY);

        // 視野角
        fov_ = Easing::InSine(easingTimer_, totalFrame, 50.0f, 45.0f);

        // targetOffset
        targetOffset_.x = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
        targetOffset_.y = Easing::InSine(easingTimer_, totalFrame, -5.0f, -7.0f);
        targetOffset_.z = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
        
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
                length_ = 6.0f;
                targetOffset_ = { 0.0f, -1.0f, 0.0f };
                cameraOffset_ = { 0.0f, 2.5f, 0.0f };
                fov_ = 45.0f;

                // 敵死亡時カメラを使用終了
                useEnemyDeathCamera_ = false;

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

// ----- ドラゴン上昇攻撃時のカメラ -----
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
    case 0:// 初期化
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
    // 待機... 何もしない 
    case 2:   
        returnFlag = true;
        break;
    // 未使用
    case 5:   
        returnFlag = false;
        break;
    // RiseAttack終了処理
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
            state_ = 5;// 未使用にする
        }
        returnFlag = true;
    }
        break;
    }

    // 回転処理
    if (returnFlag == true)
    {
        GamePad& gamePad = Input::Instance().GetGamePad();
        float aRx = gamePad.GetAxisRX();
        DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();
                
        rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

        // Y軸回転値を-3.14~3.14に収まるようにする
        if (rotate.y < -DirectX::XM_PI) rotate.y += DirectX::XM_2PI;
        if (rotate.y > DirectX::XM_PI) rotate.y -= DirectX::XM_2PI;

        if (rotateX != 0.0f) rotate.x = rotateX;

        GetTransform()->SetRotation(rotate);
    }

    return returnFlag;
#endif
    return false;
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
