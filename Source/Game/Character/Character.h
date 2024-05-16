#pragma once
#include "../Object.h"
#include "../Other/MathHelper.h"

class Character : public Object
{
public:
    Character(std::string filename);
    virtual ~Character() {}

    virtual void Update(const float& elapsedTime);
    virtual void Render()                           = 0;
    virtual void DrawDebug();

public:
#pragma region [Collision] struct
    // ----- 円柱判定用データ -----
    struct CollisionCylinderData
    {
    public:
        // ---------- コンストラクタ ----------
        CollisionCylinderData(const std::string& name, const float& radius, const float& height,
            const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = {})
            : name_(name), radius_(radius), height_(height), offsetPosition_(offsetPosition), color_(color)
        {}
        CollisionCylinderData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return name_; }
        
        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return radius_; }
        
        // ---------- 高さ ----------
        [[nodiscard]] const float GetHeight() const { return height_; }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const{ return jointPosition_ + offsetPosition_; }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }

    private:
        std::string         name_           = "";   // 名前
        float               radius_         = 0.0f; // 半径
        float               height_         = 0.0f; // 高さ
        DirectX::XMFLOAT3   jointPosition_  = {};   // ジョイント位置
        DirectX::XMFLOAT3   offsetPosition_ = {};   // オフセット位置
        DirectX::XMFLOAT4   color_          = {};   // 色
    };

    // ----- 球判定用データ -----
    struct CollisionSphereData
    {
    public:
        // ---------- コンストラクタ ----------
        CollisionSphereData(const std::string& name, const float& radius,
            const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = {1,1,1,1})
            : name_(name), radius_(radius), offsetPosition_(offsetPosition), color_(color)
        {}
        CollisionSphereData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return name_; }

        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return radius_; }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return jointPosition_; }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }

    private:
        std::string         name_           = "";   // 名前
        float               radius_         = 0.0f; // 半径
        DirectX::XMFLOAT3   jointPosition_  = {};   // ジョイント位置
        DirectX::XMFLOAT3   offsetPosition_ = {};   // オフセット位置
        DirectX::XMFLOAT4   color_          = {};   // 色
    };

    // ----- やられ判定 -----
    struct DamageDetectionData
    {
    public:
        DamageDetectionData(const std::string& name, const float& radius, const float& damage, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 0, 0, 1, 1 }),
            damage_(damage)
        {}
        DamageDetectionData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }
        void SetPosition(const DirectX::XMFLOAT3& jointPosition) { collisionSphereData_.SetJointPosition(jointPosition + collisionSphereData_.GetOffsetPosition()); }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

    private:
        CollisionSphereData collisionSphereData_;
        float               damage_ = 0.0f; // ダメージ量
    };

    // ----- 攻撃判定 -----
    struct AttackDetectionData
    {
        AttackDetectionData(const std::string& name, const float& radius, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 1, 0, 0, 1 })
        {}
        AttackDetectionData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

        // ---------- 有効(使用)フラグ ----------
        [[nodiscard]] const bool GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    private:
        CollisionSphereData collisionSphereData_;   // 球体データ
        bool                isActive_ = false;      // 現在有効か
    };

    // ----- 押し出し判定 -----
    struct CollisionDetectionData
    {
        CollisionDetectionData(const std::string& name, const float& radius, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 0, 0, 0, 1 })
        {}
        CollisionDetectionData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

        // ---------- 有効(使用)フラグ ----------
        [[nodiscard]] const bool GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    private:
        CollisionSphereData collisionSphereData_;
        bool                isActive_ = false;      // 現在有効か
    };

#pragma endregion [Collision] struct

    // ----- Collision更新 -----
    void UpdateCollisions(const float& scaleFactor);

    void CollisionCharacterVsStage();
    DirectX::XMFLOAT3 SetTargetPosition();
    

    // ----- 吹っ飛ばす処理 -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- 速度 ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    [[nodiscard]] const float GetMaxSpeed() const { return maxSpeed_; }
    
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { velocity_ = velocity; }
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

private:
    DirectX::XMFLOAT3   velocity_   = {};   // 速度
    float               maxSpeed_   = 0.0f; // 最大速度

    float               rotateSpeed_ = 1.0f; // 回転速度

    float               maxHealth_  = 1.0f;
    float               health_     = 1.0f; // 体力

    // ---------- Collision用 ----------
    std::vector<DamageDetectionData>        damageDetectionData_;   // くらい判定
    std::vector<AttackDetectionData>        attackDetectionData_;   // 攻撃判定
    std::vector<CollisionDetectionData>     collisionDetectionData_;// 押し出し判定

    // ---------- 吹っ飛び --------------------
    DirectX::XMFLOAT3   blowDirection_  = {};
    float               blowPower_      = 0.0f;
};

