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
    // ----- Collision�X�V -----
    virtual void UpdateCollisions(const float& elapsedTime, const float& scaleFactor);

    void CollisionCharacterVsStage();
    DirectX::XMFLOAT3 SetTargetPosition();
    

    // ----- ������΂����� -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);
    void AddDamage(const float& damage) { health_ -= damage; }

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- ���x ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    [[nodiscard]] const float GetAcceleration() const { return acceleration_; }
    [[nodiscard]] const float GetDeceleration() const { return deceleration_; }
    [[nodiscard]] const float GetMaxSpeed() const { return maxSpeed_; }
    
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { velocity_ = velocity; }
    void SetAcceleration(const float& speed) { acceleration_ = speed; }
    void SetDeceleration(const float& speed) { deceleration_ = speed; }
    void SetMaxSpeed(const float& maxSpeed) { maxSpeed_ = maxSpeed; }

    // ---------- ��]���x ----------
    [[nodiscard]] const float GetRotateSpeed() const { return rotateSpeed_; }
    void SetRotateSpeed(const float& speed) { rotateSpeed_ = speed; }

    // ---------- �̗� ----------
    [[nodiscard]] const float GetMaxHealth() const { return maxHealth_; }
    void SetMaxHealth(const float& health) { maxHealth_ = health; }
    [[nodiscard]] const float GetHealth() const { return health_; }
    void SetHealth(const float& health) { health_ = health; }
    
    // -------------------- Collision�p --------------------
    [[nodiscard]] const float GetCollisionRadius() const { return collisionRadius_; }
    void SetCollisionRadius(const float& radius) { collisionRadius_ = radius; }

    // ---------- ���炢���� ----------
    void RegisterDamageDetectionData(const DamageDetectionData& data);
    [[nodiscard]] const int GetDamageDetectionDataCount() const { return damageDetectionData_.size(); }
    std::vector<DamageDetectionData> GetDamageDetectionData() { return damageDetectionData_; }
    DamageDetectionData& GetDamageDetectionData(const std::string& name);
    DamageDetectionData& GetDamageDetectionData(const int& index);

    // ---------- �U������ ----------
    void RegisterAttackDetectionData(const AttackDetectionData& data);
    [[nodiscard]] const int GetAttackDetectionDataCount() const { return attackDetectionData_.size(); }
    std::vector<AttackDetectionData> GetAttackDetectionData() { return attackDetectionData_; }
    AttackDetectionData& GetAttackDetectionData(const std::string& name);
    AttackDetectionData& GetAttackDetectionData(const int& index);

    // ---------- �����o������ ----------
    void RegisterCollisionDetectionData(const CollisionDetectionData& data);
    [[nodiscard]] const int GetCollisionDetectionDataCount() const { return collisionDetectionData_.size(); }
    std::vector<CollisionDetectionData> GetCollisionDetectionData() { return collisionDetectionData_; }
    CollisionDetectionData& GetCollisionDetectionData(const std::string& name);
    CollisionDetectionData& GetCollisionDetectionData(const int& index);

#pragma endregion [Get, Set] Function

protected:
    // ---------- Collision�p ----------
    std::vector<DamageDetectionData>        damageDetectionData_;   // ���炢����
    std::vector<AttackDetectionData>        attackDetectionData_;   // �U������
    std::vector<CollisionDetectionData>     collisionDetectionData_;// �����o������

private:
    DirectX::XMFLOAT3   velocity_       = {};   // ���x
    float               acceleration_   = 0.0f; // ���x
    float               deceleration_   = 0.0f; // ����
    float               maxSpeed_       = 0.0f; // �ő呬�x

    float               rotateSpeed_    = 1.0f; // ��]���x

    float               maxHealth_      = 1.0f;
    float               health_         = 1.0f; // �̗�


    float collisionRadius_ = 0.0f;

    // ---------- ������� --------------------
    DirectX::XMFLOAT3   blowDirection_  = {};
    float               blowPower_      = 0.0f;
};

