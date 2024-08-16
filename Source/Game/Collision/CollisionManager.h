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

private:// 判定用関数
#pragma region ---------- 判定用関数 ----------
    // ---------- 球と球の交差判定 ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB);

    // ---------- 球と球の交差判定 押し出し有 ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- 球と球の交差判定 Y座標の押し出しを考慮しない ----------
    [[nodiscard]] const bool IntersectSphereVsSphereNotConsiderY(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- 円柱と円柱の交差判定 ----------
    [[nodiscard]] const bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB,
        DirectX::XMFLOAT3& outPosition);
    
    // ---------- 球と直線の交差判定 ----------
    [[nodiscard]] const bool IntersectSphereVsRay(
        const DirectX::XMFLOAT3& spherePos, const float radius,
        const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length);
    
    // ---------- 球と直線の交差判定 ----------
    [[nodiscard]] const bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePos, const float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection,
        const float length, const float capsuleRadius);

#pragma endregion ---------- 判定用関数 ----------
};

