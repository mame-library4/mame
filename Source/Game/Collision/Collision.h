#pragma once

#include <DirectXMath.h>

// �R���W����
class Collision
{
public:
    // ���Ƌ��̌�������
    static const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPosition
    );

    // �����o�������Ȃ��o�[�W����
    static const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB
    );

    // ---------- ���Ƌ��̌������� Player�p ( Y���W�̉����o�����l�����Ȃ� ) ----------
    static const bool IntersectSphereVsSphereNotConsiderY(
        const DirectX::XMFLOAT3& positionA, const float& radiusA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB,
        DirectX::XMFLOAT3& outPositionB
    );




    // ----- "�~��" �� "�~��" �̌������� -----
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB,
        DirectX::XMFLOAT3& outPosition);


    //���ƒ����̌�������
    static const bool IntersectSphereVsRay(
        const DirectX::XMFLOAT3& spherePos, const float radius,
        const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length
    );

    //���ƒ����̌�������
    static const bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePos, const float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, 
        const float length, const float capsuleRadius
    );
};

