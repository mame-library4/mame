#pragma once

#include <DirectXMath.h>

// RW
class Collision
{
public:
    // ΖΜπ·»θ
    static const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPosition
    );
    // ΅o΅Θ΅o[W
    static const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB
    );

    // ---------- ΖΜπ·»θ Playerp ( YΐWΜ΅o΅πlΆ΅Θ’ ) ----------
    static const bool IntersectSphereVsSphereNotConsiderY(
        const DirectX::XMFLOAT3& positionA, const float& radiusA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB,
        DirectX::XMFLOAT3& outPositionB
    );




    // ----- "~" Ζ "~" Μπ·»θ -----
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB,
        DirectX::XMFLOAT3& outPosition);


    //ΖΌόΜπ·»θ
    static const bool IntersectSphereVsRay(
        const DirectX::XMFLOAT3& spherePos, const float radius,
        const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length
    );

    //ΖΌόΜπ·»θ
    static const bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePos, const float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, 
        const float length, const float capsuleRadius
    );
};

