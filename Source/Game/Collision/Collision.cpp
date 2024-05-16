#include "Collision.h"
#include "../Other/MathHelper.h"

// 球と球の交差判定
const bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, const float radiusA,
    const DirectX::XMFLOAT3& positionB, const float radiusB,
    DirectX::XMFLOAT3& outPosition)
{
    const DirectX::XMFLOAT3 vec = positionB - positionA;
    const float lengthSq = XMFloat3LengthSq(vec);
    const float range = radiusA + radiusB;

    if (lengthSq > (range * range)) return false;

    const DirectX::XMFLOAT3 vecN = XMFloat3Normalize(vec);
    outPosition = (positionA + vecN * range);

    return true;
}

const bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, const float radiusA,
    const DirectX::XMFLOAT3& positionB, const float radiusB)
{
    using DirectX::XMFLOAT3;

    const XMFLOAT3 vec      = positionB - positionA;
    const float    lengthSq = ::XMFloat3LengthSq(vec);
    const float    range    = radiusA + radiusB;

    if (lengthSq > (range * range)) return false;

    return true;
}

// ----- 球と球の交差判定 ( Y座標の押し出しを考慮しない ) -----
const bool Collision::IntersectSphereVsSphereNotConsiderY(const DirectX::XMFLOAT3& positionA, const float& radiusA, const DirectX::XMFLOAT3& positionB, const float& radiusB, DirectX::XMFLOAT3& outPositionB)
{
    // A -> B のベクトル算出
    DirectX::XMFLOAT3 vec = positionB - positionA;
    DirectX::XMFLOAT3 vecNormal = XMFloat3Normalize(vec);

    // 距離判定を行う
    float length = XMFloat3Length(vec);
    const float range = radiusA + radiusB;
    // 交差していない
    if (length > range) return false;
    
    
    DirectX::XMFLOAT2 posAxz = { positionA.x, positionA.z };
    DirectX::XMFLOAT2 posBxz = { positionB.x, positionB.z };
    DirectX::XMFLOAT2 vecXZ = posBxz - posAxz;
    
    float vecC = length;
    float vecC2 = radiusA + radiusB;

    float vecA = positionA.y - positionB.y;
    float vecB = sqrtf(vecC * vecC - vecA * vecA);
    float vecB2 = sqrtf(vecC2 * vecC2 - vecA * vecA);
    float vecLength = vecB2 - vecB;

    vecXZ = XMFloat2Normalize(vecXZ) * vecLength;
    outPositionB = positionB + DirectX::XMFLOAT3(vecXZ.x, 0, vecXZ.y);

    return true;
}

// ----- "円柱" と "円柱" の交差判定 -----
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA, const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB, DirectX::XMFLOAT3& outPosition)
{
    // A の足元が B の頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // A の頭が B の足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    vx /= distXZ;
    vz /= distXZ;
    if (distXZ > range)
    {
        return false;
    }

    // AがBを押し出す
    outPosition.x = positionA.x + (vx * range);
    outPosition.y = positionB.y;
    outPosition.z = positionA.z + (vz * range);

    return true;
}

const bool Collision::IntersectSphereVsRay(const DirectX::XMFLOAT3& spherePos, const float radius, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length)
{
    DirectX::XMFLOAT3 vec = spherePos - rayStart;

    //光線の方向と逆方向に球があった場合は処理しない
    if (XMFloat3Dot(XMFloat3Normalize(vec), rayDirection) < 0)return false;

    //光線の長さよりも球までの距離が長かった場合処理しない
    if (XMFloat3Length(vec) > length)return false;

    //光線から球までの最短距離
    float projectionLength = XMFloat3Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    //この距離が球の半径よりも短いなら交差している
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}

const bool Collision::IntersectSphereVsCapsule(const DirectX::XMFLOAT3& spherePos, const float sphereRadius, const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, const float length, const float capsuleRadius)
{
    DirectX::XMFLOAT3 vec = spherePos - capsuleStart;

    //光線の方向と逆方向に球があった場合は処理しない
    if (XMFloat3Dot(XMFloat3Normalize(vec), capsuleDirection) < 0)return false;

    //光線の長さよりも球までの距離が長かった場合処理しない
    if (XMFloat3Length(vec) > length + capsuleRadius)return false;

    //光線から球までの最短距離
    float projectionLength = XMFloat3Dot(vec, capsuleDirection);

    const DirectX::XMFLOAT3 p1 = capsuleStart + capsuleDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    //この距離が球の半径よりも短いなら交差している
    if (nearestLength <= sphereRadius + capsuleRadius)
    {
        return true;
    }

    return false;
}
