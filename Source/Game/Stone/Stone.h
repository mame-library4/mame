#pragma once
#include "Object.h"

class Stone : public Object
{
public:
    Stone(const std::string filename);
    ~Stone();

    void Initialize(const DirectX::XMFLOAT3& basePosition);
    void Update(const float& elapsedTime);
    void Render(const float& scaleFacter);
    void DrawDebug();

private:
    DirectX::XMFLOAT3   moveDirection_  = {};   // 移動方向
    float               moveSpeed_      = 0.0f; // 移動速度
    float               ascendSpeed_    = 0.0f; // 上昇速度
    DirectX::XMFLOAT3   turnRate_       = {};   // 回転率
    float               turnSpeed_      = 0.0f; // 回転速度
};

