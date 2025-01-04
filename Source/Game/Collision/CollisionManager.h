#pragma once
#include <DirectXMath.h>
#include <string>
#include "Effect/EffectManager.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

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
    // ========== Player VS ZZ ==========
#pragma region ========== Player VS ZZ ==========
    void UpdatePlayerVs();
    // ----- ƒWƒƒƒXƒg‰ñ”ğ”»’è -----
    void UpdatePlayerJustDodge();               // PlayerJustDodge
    void PlayerJustDodgeVsEnemyAttack();        // Player VS Enemy
    void PlayerJustDodgeVsProjectileAttack();   // Player VS Projectile
    // ----- UŒ‚”»’è -----
    void UpdatePlayerAttack();              // PlayerAttack
    void PlayerAttackVsEnemyDamage();       // Player VS Enemy
    // ----- ƒJƒEƒ“ƒ^[”»’è -----
    void UpdatePlayerCounter();             // PlayerCounter
    void PlayerCounterVsEnemy();            // Player VS Enemy
    void PlayerGuardCounterVsEnemy();       // Player VS Enemy
    void PlayerCounterVsProjectile();       // Player VS Projectile
    void PlayerGuardCounterVsProjectile();  // Player VS Projectile
    // ----- ‚­‚ç‚¢”»’è -----
    void UpdatePlayerDamage();              // PlayerDamage
    void PlayerFlinchVsEnemyAttack();       // Player VS Enemy
    void PlayerDamageVsEnemyAttack();       // Player VS Enemy
    void PlayerDamageVsProjectileAttack();  // Player VS Projectile
    // ----- ‰Ÿ‚µo‚µ”»’è -----
    void UpdatePlayerCollision();           // PlayerCollision
    void PlayerVsEnemy();                   // Player VS Enemy
#pragma endregion ========== Player VS ZZ ==========

    // ========== Enemy VS ZZ ==========
#pragma region ========== Enemy VS ZZ ==========
    void UpdateEnemyVs();
    // ----- ‚­‚ç‚¢”»’è -----
    void UpdateEnemyDamage();               // EnemyDamage
    void EnemyDamageVsProjectileAttack();   // Enemy VS Proejctile

#pragma endregion ========== Enemy VS ZZ ==========
    
    // ========== Projectile VS ZZ ==========
#pragma region ========== Projectile VS ZZ ==========
    void UpdateProjectileVs();

#pragma endregion ========== Projectile VS ZZ ==========


    void UpdateItemVs();       // ƒAƒCƒeƒ€‚Æ‚Ìˆ—ŒÄ‚Ño‚µ
    void UpdateItemVsAttack(); // ƒAƒCƒeƒ€‚ÆUŒ‚”»’è‚Æ‚Ì”»’è
    void UpdateItemVsDamage(); // ƒAƒCƒeƒ€‚Æƒ_ƒ[ƒW”»’è
    void UpdateItemVsItem();   // ƒAƒCƒeƒ€“¯m‚Ì”»’è

private:// ”»’è—pŠÖ”
#pragma region ---------- ”»’è—pŠÖ” ----------
    // ---------- ‹…‚Æ‹…‚ÌŒğ·”»’è ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB);

    // ---------- ‹…‚Æ‹…‚ÌŒğ·”»’è ‰Ÿ‚µo‚µ—L ----------
    [[nodiscard]] const bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- ‹…‚Æ‹…‚ÌŒğ·”»’è YÀ•W‚Ì‰Ÿ‚µo‚µ‚ğl—¶‚µ‚È‚¢ ----------
    [[nodiscard]] const bool IntersectSphereVsSphereNotConsiderY(
        const DirectX::XMFLOAT3& positionA, const float radiusA,
        const DirectX::XMFLOAT3& positionB, const float radiusB,
        DirectX::XMFLOAT3& outPositionB);

    // ---------- ‰~’Œ‚Æ‰~’Œ‚ÌŒğ·”»’è ----------
    [[nodiscard]] const bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA,
        const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB,
        DirectX::XMFLOAT3& outPosition);
    
    // ---------- ‹…‚Æ’¼ü‚ÌŒğ·”»’è ----------
    [[nodiscard]] const bool IntersectSphereVsRay(
        const DirectX::XMFLOAT3& spherePos, const float radius,
        const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length);
    
    // ---------- ‹…‚Æ’¼ü‚ÌŒğ·”»’è ----------
    [[nodiscard]] const bool IntersectSphereVsCapsule(
        const DirectX::XMFLOAT3& spherePos, const float sphereRadius,
        const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection,
        const float length, const float capsuleRadius);

#pragma endregion ---------- ”»’è—pŠÖ” ----------

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

    ComputeParticleEmitter computeParticleEmitter_ = {};

    
    // ----- HitEffect -----
    float hitEffectDefaultSize_     = 0.3f;
    float hitEffectWeakPointSize_   = 0.4f;
    float hitEffectDefaultSpeed_    = 1.0f;
    float hitEffectSlowSpeed_       = 0.01f;
};

