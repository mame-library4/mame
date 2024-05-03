#include "Collision.h"
#include "../Other/MathHelper.h"

// ���Ƌ��̌�������
const bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, const float radiusA,
    const DirectX::XMFLOAT3& positionB, const float radiusB,
    DirectX::XMFLOAT3* outPosition)
{
#if 0
    // B �� A�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3Length(vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range)
    {
        return false;
    }

    // A �� B �������o��
    float dist = range - lengthSq;                      // �߂荞��ł��鋗�����Z�o
    vec = DirectX::XMVectorScale(vec, dist);            // �x�N�g���ɕϊ�
    PositionB = DirectX::XMVectorAdd(PositionB, vec);   // �����o���ʒu
    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    return true;

#else
    using DirectX::XMFLOAT3;

    const XMFLOAT3 vec      = positionB - positionA;
    const float    lengthSq = ::XMFloat3LengthSq(vec);
    const float    range    = radiusA + radiusB;

    if (lengthSq > (range * range)) return false;

    const XMFLOAT3 vecN = ::XMFloat3Normalize(vec);
    (*outPosition) = (positionA + vecN * range);

    return true;

#endif
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

// ----- "�~��" �� "�~��" �̌������� -----
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA, const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB, DirectX::XMFLOAT3& outPosition)
{
    // A �̑����� B �̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }
    // A �̓��� B �̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }

    // XZ ���ʂł͈̔̓`�F�b�N
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

    // A��B�������o��
    outPosition.x = positionA.x + (vx * range);
    outPosition.y = positionB.y;
    outPosition.z = positionA.z + (vz * range);

    return true;
}

const bool Collision::IntersectSphereVsRay(const DirectX::XMFLOAT3& spherePos, const float radius, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length)
{
    DirectX::XMFLOAT3 vec = spherePos - rayStart;

    //�����̕����Ƌt�����ɋ����������ꍇ�͏������Ȃ�
    if (XMFloat3Dot(XMFloat3Normalize(vec), rayDirection) < 0)return false;

    //�����̒����������܂ł̋��������������ꍇ�������Ȃ�
    if (XMFloat3Length(vec) > length)return false;

    //�������狅�܂ł̍ŒZ����
    float projectionLength = XMFloat3Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    //���̋��������̔��a�����Z���Ȃ�������Ă���
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}

const bool Collision::IntersectSphereVsCapsule(const DirectX::XMFLOAT3& spherePos, const float sphereRadius, const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, const float length, const float capsuleRadius)
{
    DirectX::XMFLOAT3 vec = spherePos - capsuleStart;

    //�����̕����Ƌt�����ɋ����������ꍇ�͏������Ȃ�
    if (XMFloat3Dot(XMFloat3Normalize(vec), capsuleDirection) < 0)return false;

    //�����̒����������܂ł̋��������������ꍇ�������Ȃ�
    if (XMFloat3Length(vec) > length + capsuleRadius)return false;

    //�������狅�܂ł̍ŒZ����
    float projectionLength = XMFloat3Dot(vec, capsuleDirection);

    const DirectX::XMFLOAT3 p1 = capsuleStart + capsuleDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    //���̋��������̔��a�����Z���Ȃ�������Ă���
    if (nearestLength <= sphereRadius + capsuleRadius)
    {
        return true;
    }

    return false;
}
