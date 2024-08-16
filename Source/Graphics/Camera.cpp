#include "Camera.h"
#include "../Core/Application.h"

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"

#include "SceneManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Player/PlayerManager.h"

// ----- 初期化 -----
void Camera::Initialize()
{
    useEnemyDeathCamera_ = false;

    enemyDeathstate_ = 0;
    playerDeathState_ = 0;
    playerCounterStae_ = 0;


    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    //GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));

    // TODO:かめら
    // ここからテストしてるだけなので変える多分
    targetOffset_ = { 0, -1, 0 };
    cameraOffset_ = { 0, 2.5f, 0 };
    length_ = 6.0f;
    verticalRotationSpeed_ = 1.0f;
    horizontalRotationSpeed_ = 4.0f;
}

// ----- 更新 -----
void Camera::Update(const float& elapsedTime)
{
    // タイトル画面はとりあえずここで終了
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Title) return;

    // 死亡カメラ使用時はここで終了
    if (UpdateEnemyDeathCamera(elapsedTime)) return;    

    const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    Camera::Instance().SetTarget(cameraTargetPosition);

    // ドラゴンの上昇攻撃のカメラ更新
    //if (UpdateRiseAttackCamera(elapsedTime)) return;

    // カウンター攻撃のカメラ更新
    if (UpdateCounterAttackCamera(elapsedTime)) return;

    // --- カメラ回転処理 ---
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

// ----- 回転処理 -----
void Camera::Rotate(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    const float invertVertical = invertVertical_ ? -1 : 1;

    // コントローラーの傾きが一定以上じゃないと判定を取らない
    // ( 操作ストレス軽減のため制限を設ける )
    // 横移動したいのに上下移動してしまわないように...
    //if (fabs(aRy) >= inputThreshold_)
    //{
    //    rotate.x -= aRy * invertVertical * verticalRotationSpeed_ * elapsedTime;
    //}
    // 横移動は制限なし
    rotate.y += aRx * horizontalRotationSpeed_ * elapsedTime;

    // length制御
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;
    //length_ += addLength * aRy * elapsedTime;
    //if (length_ < minLength_) length_ = minLength_;
    if (length_ > maxLength_) length_ = maxLength_;

    // X軸(上下)の回転制御
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;
    //if (rotate.x > maxXRotation_) rotate.x = maxXRotation_;

    
    if (fabs(aRy) >= inputThreshold_)
    {
        if (rotate.x > maxXRotation_)
        {
            isAdjustCameraLength_ = true;

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
            isAdjustCameraLength_ = false;

            rotate.x -= aRy * invertVertical * verticalRotationSpeed_ * elapsedTime;

            length_ += addLength * aRy * elapsedTime;
        }
    }


    // Y軸回転値を-3.14~3.14に収まるようにする
    if (rotate.y < -DirectX::XM_PI) rotate.y += DirectX::XM_2PI;
    if (rotate.y >  DirectX::XM_PI) rotate.y -= DirectX::XM_2PI;

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

// ----- 敵死亡時カメラ使用する -----
void Camera::SetUseEnemyDeathCamera()
{
    useEnemyDeathCamera_ = true;
    enemyDeathstate_ = 0;
}

// ----- カウンター時カメラを使用する -----
void Camera::SetUseCounterCamera()
{
    useCounterCamera_ = true;
    playerCounterStae_ = 0;
}

// ----- 敵死亡時カメラ -----
const bool Camera::UpdateEnemyDeathCamera(const float& elapsedTime)
{
    // 敵死亡時カメラ使用フラグが立っていないのでここで終了
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

    switch (static_cast<CounterAttackCamera>(playerCounterStae_))
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
        if (player->GetCurrentState() == Player::STATE::Idle)
        {
            // ステート変更
            SetState(CounterAttackCamera::CounterFinalize);
        }
        if (player->GetCurrentState() == Player::STATE::CounterCombo)
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

        // ステート変更
        SetState(CounterAttackCamera::CounterComboZoomIn);

        break;  
    case CounterAttackCamera::CounterComboZoomIn:
    {
        const float totalFrame = 0.17f;
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
            easingTimer_ = 0.0f;

            // ステート変更
            SetState(CounterAttackCamera::CounterComboZoomOut);
        }
    }
        break;
    case CounterAttackCamera::CounterComboZoomOut:
    {
        const float totalFrame = 0.4f;
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

            // ステート変更
            SetState(CounterAttackCamera::Finalize);
        }
    }
        break;
    case CounterAttackCamera::Finalize:
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
        break;
    }

#if 0
    switch (state_)
    {
    case 1:
    {
        const float totalFrame = 0.17f;
        length_ = Easing::InSine(easingTimer_, totalFrame, 4.5f, oldCameraLength_);

        const float maxRotateX = oldRotateX_ + DirectX::XMConvertToRadians(3.0f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotateX, oldRotateX_);
        GetTransform()->SetRotationX(rotateX);


        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        if (easingTimer_ == totalFrame)
        {
            state_ = 2;
            easingTimer_ = 0.0f;
        }

    }
        break;
    case 2:
    {
        const float totalFrame = 0.2f;
        length_ = Easing::InSine(easingTimer_, totalFrame, 9.0f, 4.5f);
        
        const float minRotateX = oldRotateX_ + DirectX::XMConvertToRadians(3.0f);
        const float maxRotateX = oldRotateX_ + DirectX::XMConvertToRadians(-3.5f);
        const float rotateX = Easing::InSine(easingTimer_, totalFrame, maxRotateX, minRotateX);
        GetTransform()->SetRotationX(rotateX);


        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        if (easingTimer_ == totalFrame)
        {
            state_ = 3;
            easingTimer_ = 0.0f;
        }
    }
        break;
    case 3:
        break;
    }
#endif

    return true;
}
