#pragma once
#include <DirectXMath.h>

class DarkFireballEmitter
{
public:
    DarkFireballEmitter() {}
    ~DarkFireballEmitter() {}

    void Initilaize(const int& emitNum, const DirectX::XMFLOAT3& staffPosition, const DirectX::XMFLOAT3& ownerForward, const DirectX::XMFLOAT3& ownerRight);
    [[nodiscard]] const bool Update(const float& elapsedTime);
    void DrawDebug();

private:
    DirectX::XMFLOAT3 staffPosition_    = {};
    DirectX::XMFLOAT3 ownerForward_     = {};
    DirectX::XMFLOAT3 ownerRight_       = {};

    int currentEmitCount_   = 0;
    int maxEmitNum_         = 0;

    float emitTimer_    = 0.0f;
    float emitTime_     = 0.1f;

    float moveSpeed_    = 15.0f;
    float changeTime_   = 0.1f;
};

