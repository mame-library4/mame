#pragma once
#include <DirectXMath.h>
#include <string>
#include "Effect/EffectManager.h"

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
    void DrawDebug();

private:
    void UpdatePlayerVsEnemy();
    void CheckJustDodgeCollision(); // ジャスト回避判定
    void UpdatePlayerAttackVsEnemyDamage();
    void UpdatePlayerDamageVsEnemyAttack();
    void UpdatePlayerCollisionVsEnemyCollision();
    void CounterCheckEnemyAttack();                 // 敵の攻撃に対するカウンター判定

    void UpdatePlayerVsProjectile();
    void UpdatePlayerDamageVsProjectileAttack();
    void CounterCheckProjectile();                  // 発射物に対するカウンター判定

    void UpdateItemVs();       // アイテムとの処理呼び出し
    void UpdateItemVsAttack(); // アイテムと攻撃判定との判定
    void UpdateItemVsDamage(); // アイテムとダメージ判定
    void UpdateItemVsItem();   // アイテム同士の判定

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

private:
    struct EffectHandle
    {
        Effekseer::Handle   effectHandle_ = {};
        std::string         name_;
    };
    static const int maxEffectHandle_ = 10;
    EffectHandle effectHandle_[maxEffectHandle_] = {};
    int handleCounter_ = 0;

    int hitEffectType_ = 0;

    float vibrationTime_ = 1.0f;
    float vibrationVolume_ = 1.0f;

    // --------------------

    DirectX::XMFLOAT4 damageUIColor_            = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 weakPointDamageUIColor_   = { 1.0f, 0.55f, 0.0f, 1.0f };

};

