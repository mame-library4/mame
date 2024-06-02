#pragma once
#include "Object.h"

class Stone : public Object
{
public:
    Stone(const std::string filename);
    ~Stone();

    void Initialize(const DirectX::XMFLOAT3& basePosition);
    void Update(const float& elapsedTime);
    void Render(const float& scaleFacter, ID3D11PixelShader* psShader = nullptr);
    void DrawDebug();

private:
    DirectX::XMFLOAT3   moveDirection_  = {};   // ˆÚ“®•ûŒü
    float               moveSpeed_      = 0.0f; // ˆÚ“®‘¬“x
    float               ascendSpeed_    = 0.0f; // ã¸‘¬“x
    DirectX::XMFLOAT3   turnRate_       = {};   // ‰ñ“]—¦
    float               turnSpeed_      = 0.0f; // ‰ñ“]‘¬“x
};

