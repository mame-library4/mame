#include "Stone.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
Stone::Stone(const std::string filename)
    : Object(filename)
{
}

// ----- デストラクタ -----
Stone::~Stone()
{
}

// ----- 初期化 -----
void Stone::Initialize(const DirectX::XMFLOAT3& basePosition)
{
    DirectX::XMFLOAT3 pos = basePosition;
    
    GetTransform()->SetScaleFactor(rand() % 11 / 10.0f + 0.7f);

    // 地面に隠れておいてほしいので Y は -0.3
    pos.y = -0.2f;

    // XZ 平面にランダムな位置に生成
    pos.x += (rand() % 50 - 25) / 10.0f;
    pos.z += (rand() % 50 - 25) / 10.0f;

    GetTransform()->SetPosition(pos);

    // 移動方向を設定
    moveDirection_ = XMFloat3Normalize(pos - basePosition);

    // 移動速度を設定
    moveSpeed_ = rand() % 3 + 1;

    // 上昇速度を設定
    //ascendSpeed_ = 20.0f;
    ascendSpeed_ = std::min(20.0f, 20.0f * ( 2.1f - GetTransform()->GetScaleFactor()));
    //ascendSpeed_ = (rand() % 11 + 1) / 10.0f * 20.0f;

    // 初期回転値を設定
    DirectX::XMFLOAT3 rotate = GetTransform()->GetRotation();
    rotate.x = DirectX::XMConvertToRadians(rand() % 180);
    rotate.y = DirectX::XMConvertToRadians(rand() % 180);
    rotate.z = DirectX::XMConvertToRadians(rand() % 180);

    // 回転率を設定
    turnRate_.x = DirectX::XMConvertToRadians(rand() % 20);
    turnRate_.y = DirectX::XMConvertToRadians(rand() % 20);
    turnRate_.z = DirectX::XMConvertToRadians(rand() % 20);

    // 回転速度を設定
    turnSpeed_ = 7.0f;
}

// ----- 更新 -----
void Stone::Update(const float& elapsedTime)
{
    if (GetTransform()->GetPositionY() < -10.0f)
    {
        GetTransform()->SetScaleFactor(0.0f);
    }

    // Y 軸移動処理
    float gravity = 9.8f * elapsedTime;
    ascendSpeed_ -= gravity * 3.5f;
    GetTransform()->AddPositionY(ascendSpeed_ * elapsedTime);

    // XZ平面移動処理
    DirectX::XMFLOAT3 moveVec = moveDirection_ * moveSpeed_ * elapsedTime;
    moveVec.y = 0;
    GetTransform()->AddPosition(moveVec);

    // 回転処理
    GetTransform()->AddRotationX(turnRate_.x * turnSpeed_ * elapsedTime);
    GetTransform()->AddRotationY(turnRate_.y * turnSpeed_ * elapsedTime);
    GetTransform()->AddRotationZ(turnRate_.z * turnSpeed_ * elapsedTime);
}

// ----- 描画 -----
void Stone::Render(const float& scaleFacter)
{
    Object::Render(scaleFacter);
}

// ----- ImGui用 -----
void Stone::DrawDebug()
{

}
