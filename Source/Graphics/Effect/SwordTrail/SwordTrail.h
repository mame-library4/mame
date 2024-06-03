#pragma once
#include "PrimitiveRenderer.h"

class SwordTrail
{
public:
    SwordTrail() {}
    ~SwordTrail() {}

    void Update(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition);
    void Render();
    void DrawDebug();

private:
    PrimitiveRenderer primitiveRenderer_;
    //static const int MAX_POLYGON = 10;
    static const int MAX_POLYGON = 16;
    DirectX::XMFLOAT3 trailPositions[2][MAX_POLYGON] = {};
    DirectX::XMFLOAT2 texcoord_[2][MAX_POLYGON] = {};

    //bool splineCurve_ = true;
    bool splineCurve_ = false;
};

