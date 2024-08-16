#pragma once
#include <DirectXMath.h>

class CollisionManager
{
private:
    CollisionManager() {}
    ~CollisionManager() {}

public:
    static CollisionManager& Instance()
    {
        static CollisionManager instance;
        return instance;
    }

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime);

private:
    void UpdatePlayerVsEnemy();
    void UpdatePlayerAttackVsEnemyDamage();
    void UpdatePlayerDamageVsEnemyAttack();
    void UpdatePlayerCollisionVsEnemyCollision();
    void UpdatePlayerCounterCollision() {}

    void UpdatePlayerVsProjectile();
    void UpdatePlayerDamageVsProjectileAttack();

private:// ����p�֐�
#pragma region ---------- ����p�֐� ----------
    // ---------- ���Ƌ��̌������� ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB);

    // ---------- ���Ƌ��̌������� �����o���L ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- ���Ƌ��̌������� Y���W�̉����o�����l�����Ȃ� ----------
    [[nodiscard]] const bool IntersectSphereVsSphereNotConsiderY(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- �~���Ɖ~���̌������� ----------
    [[nodiscard]] const bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB,
        DirectX::XMFLOAT3& outPosition);
    
    // ---------- ���ƒ����̌������� ----------
    [[nodiscard]] const bool IntersectSphereVsRay(
        const DirectX::XMFLOAT3& spherePos, const float radius,
        const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length);
    
    // ---------- ���ƒ����̌������� ----------
    [[nodiscard]] const bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePos, const float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection,
        const float length, const float capsuleRadius);

#pragma endregion ---------- ����p�֐� ----------
};

