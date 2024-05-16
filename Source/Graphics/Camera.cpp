#include "Camera.h"
#include "../Core/Application.h"

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"

// ----- 初期化 -----
void Camera::Initialize()
{
    GetTransform()->SetRotationY(DirectX::XMConvertToRadians(180));
    GetTransform()->SetRotationX(DirectX::XMConvertToRadians(-9));

    // TODO:かめら
    // ここからテストしてるだけなので変える多分
    targetOffset_ = { 0,-1,0 };
    cameraOffset_ = { 0,3,0 };
    length_ = 7.0f;
}

// ----- 更新 -----
void Camera::Update(const float& elapsedTime)
{
    if (GetAsyncKeyState('H') & 1) 
        ScreenVibrate(0.1f, 2.0f);

    // --- 画面振動 ---
    ScreenVibrationUpdate(elapsedTime);

    // --- カメラ回転処理 ---
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
    if (ImGui::TreeNode("Camera"))
    {
        ImGui::DragFloat("Length", &length_, 0.01f);
        ImGui::DragFloat("MinLength", &minLength_, 0.01f);
        ImGui::DragFloat("MaxLength", &maxLength_, 0.01f);

        ImGui::DragFloat("fov", &fov_, 0.01f);
        ImGui::DragFloat("inputThreshold", &inputThreshold_, 0.1f, 0.0f, 1.0f);
        
        ImGui::DragFloat3("FocusOffset", &targetOffset_.x);
        ImGui::DragFloat3("CameraOffset", &cameraOffset_.x);

        float minXRotation = DirectX::XMConvertToDegrees(minXRotation_);
        float maxXRotation = DirectX::XMConvertToDegrees(maxXRotation_);
        ImGui::DragFloat("minXRotation", &minXRotation, 1.0f, -75.0f, -20.0f);
        ImGui::DragFloat("maxXRotation", &maxXRotation, 1.0f, -20.0f, 20.0f);
        minXRotation_ = DirectX::XMConvertToRadians(minXRotation);
        maxXRotation_ = DirectX::XMConvertToRadians(maxXRotation);

        transform_.DrawDebug();

        ImGui::TreePop();
    }
}

// ----- 回転処理 -----
void Camera::Rotate(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float aRx = gamePad.GetAxisRX();
    float aRy = gamePad.GetAxisRY();
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();

    // コントローラーの傾きが一定以上じゃないと判定を取らない
    // ( 操作ストレス軽減のため制限を設ける )
    // 横移動したいのに上下移動してしまわないように...
    if (fabs(aRy) >= inputThreshold_)
    {
        rotate.x -= aRy * elapsedTime;
    }
    // 横移動は制限なし
    rotate.y += aRx * elapsedTime;

    // length制御
    float deltaLength = maxLength_ - minLength_;
    float deltaRotate = fabs(maxXRotation_) + fabs(minXRotation_);
    float addLength = deltaLength / deltaRotate;
    length_ += addLength * aRy * elapsedTime;
    if (length_ < minLength_) length_ = minLength_;
    if (length_ > maxLength_) length_ = maxLength_;

    // X軸(上下)の回転制御
    if (rotate.x < minXRotation_) rotate.x = minXRotation_;
    if (rotate.x > maxXRotation_) rotate.x = maxXRotation_;

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
