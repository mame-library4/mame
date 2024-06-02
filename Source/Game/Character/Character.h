#pragma once
#include "../Object.h"
#include "../Other/MathHelper.h"
#include "Collision/CollisionData.h"

class Character : public Object
{
public:
    Character(std::string filename);
    virtual ~Character() {}

    virtual void Update(const float& elapsedTime);
    virtual void Render(ID3D11PixelShader* psShader) = 0;
    virtual void DrawDebug();

public:
    // ----- Collision更新 -----
    virtual void UpdateCollisions(const float& elapsedTime, const float& scaleFactor);

    void CollisionCharacterVsStage();
    DirectX::XMFLOAT3 SetTargetPosition();
    

    // ----- 吹っ飛ばす処理 -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);
    void AddDamage(const float& damage) { health_ -= damage; }

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- 速度 ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    [[nodiscard]] const float GetAcceleration() const { return acceleration_; }
    [[nodiscard]] const float GetDeceleration() const { return deceleration_; }
    [[nodiscard]] const float GetMaxSpeed() const { return maxSpeed_; }
    
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { velocity_ = velocity; }
    void SetAcceleration(const float& speed) { acceleration_ = speed; }
    void SetDeceleration(const float& speed) { deceleration_ = speed; }
    void SetMaxSpeed(const float& maxSpeed) { maxSpeed_ = maxSpeed; }

    // ---------- 回転速度 ----------
    [[nodiscard]] const float GetRotateSpeed() const { return rotateSpeed_; }
    void SetRotateSpeed(const float& speed) { rotateSpeed_ = speed; }

    // ---------- 体力 ----------
    [[nodiscard]] const float GetMaxHealth() const { return maxHealth_; }
    void SetMaxHealth(const float& health) { maxHealth_ = health; }
    [[nodiscard]] const float GetHealth() const { return health_; }
    void SetHealth(const float& health) { health_ = health; }
    
    // -------------------- Collision用 --------------------
    [[nodiscard]] const float GetCollisionRadius() const { return collisionRadius_; }
    void SetCollisionRadius(const float& radius) { collisionRadius_ = radius; }

    // ---------- くらい判定 ----------
    void RegisterDamageDetectionData(const DamageDetectionData& data);
    [[nodiscard]] const int GetDamageDetectionDataCount() const { return damageDetectionData_.size(); }
    std::vector<DamageDetectionData> GetDamageDetectionData() { return damageDetectionData_; }
    DamageDetectionData& GetDamageDetectionData(const std::string& name);
    DamageDetectionData& GetDamageDetectionData(const int& index);

    // ---------- 攻撃判定 ----------
    void RegisterAttackDetectionData(const AttackDetectionData& data);
    [[nodiscard]] const int GetAttackDetectionDataCount() const { return attackDetectionData_.size(); }
    std::vector<AttackDetectionData> GetAttackDetectionData() { return attackDetectionData_; }
    AttackDetectionData& GetAttackDetectionData(const std::string& name);
    AttackDetectionData& GetAttackDetectionData(const int& index);

    // ---------- 押し出し判定 ----------
    void RegisterCollisionDetectionData(const CollisionDetectionData& data);
    [[nodiscard]] const int GetCollisionDetectionDataCount() const { return collisionDetectionData_.size(); }
    std::vector<CollisionDetectionData> GetCollisionDetectionData() { return collisionDetectionData_; }
    CollisionDetectionData& GetCollisionDetectionData(const std::string& name);
    CollisionDetectionData& GetCollisionDetectionData(const int& index);

#pragma endregion [Get, Set] Function

protected:
    // ---------- Collision用 ----------
    std::vector<DamageDetectionData>        damageDetectionData_;   // くらい判定
    std::vector<AttackDetectionData>        attackDetectionData_;   // 攻撃判定
    std::vector<CollisionDetectionData>     collisionDetectionData_;// 押し出し判定

private:
    DirectX::XMFLOAT3   velocity_       = {};   // 速度
    float               acceleration_   = 0.0f; // 速度
    float               deceleration_   = 0.0f; // 減速
    float               maxSpeed_       = 0.0f; // 最大速度

    float               rotateSpeed_    = 1.0f; // 回転速度

    float               maxHealth_      = 1.0f;
    float               health_         = 1.0f; // 体力


    float collisionRadius_ = 0.0f;

    // ---------- 吹っ飛び --------------------
    DirectX::XMFLOAT3   blowDirection_  = {};
    float               blowPower_      = 0.0f;
};

