#pragma once
#include "../Object.h"
#include "../Other/MathHelper.h"

class Character : public Object
{
public:
    Character(std::string filename);
    virtual ~Character() {}

    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render()                           = 0;
    virtual void DrawDebug();

public:
    // ----- 円柱判定用データ -----
    struct CollisionCylinderData
    {
    public:
        // ---------- コンストラクタ ----------
        CollisionCylinderData(const std::string& name, const float& radius, const float& height)
            : name_(name), radius_(radius), height_(height) 
        {}
        CollisionCylinderData(const std::string& name, const float& radius, const float& height, const DirectX::XMFLOAT3& offsetPosition)
            : name_(name), radius_(radius), height_(height), offsetPosition_(offsetPosition)
        {}
        CollisionCylinderData() = default;

        // ---------- ImGui用 ----------
        void DrawDebug();

        // ---------- 名前 ----------
        [[nodiscard]] const std::string GetName() const { return name_; }
        
        // ---------- 半径 ----------
        [[nodiscard]] const float GetRadius() const { return radius_; }
        void SetRadius(const float& radius) { radius_ = radius; }
        
        // ---------- 高さ ----------
        [[nodiscard]] const float GetHeight() const { return height_; }
        void SetHeight(const float& height) { height_ = height; }

        // ---------- 位置 ----------
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return jointPosition_ + offsetPosition_; }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }
        void SetOffsetPosition(const DirectX::XMFLOAT3& position) { offsetPosition_ = position; }

        // ---------- 色 ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }
        void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }
        void SetColor(const float& r, const float& g, const float& b, const float& a) { color_ = { r, g, b, a }; }

    private:
        std::string         name_           = "";   // 名前
        float               radius_         = 0.0f; // 半径
        float               height_         = 0.0f; // 高さ
        DirectX::XMFLOAT3   jointPosition_  = {};   // ジョイント位置
        DirectX::XMFLOAT3   offsetPosition_ = {};   // オフセット位置
        DirectX::XMFLOAT4   color_          = {};   // 色
    };

    // ----- 円柱判定用データ更新 -----
    void UpdateCollisionCylinderData(const float& scaleFactor);


    void CollisionCharacterVsStage();
    DirectX::XMFLOAT3 SetTargetPosition(const DirectX::XMFLOAT3& pos);



public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- 速度 ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    [[nodiscard]] const float GetMaxSpeed() const { return maxSpeed_; }
    
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { velocity_ = velocity; }
    void SetMaxSpeed(const float& maxSpeed) { maxSpeed_ = maxSpeed; }

    // ---------- 体力 ----------
    [[nodiscard]] const float GetHealth() const { return health_; }
    void SetHealth(const float& health) { health_ = health; }

    // ---------- Collision用 ----------
    void RegisterCollisionCylinderData(const CollisionCylinderData& data);
    [[nodiscard]] const int GetCollisionCylinderDataCount() const { return collisionCylinderData_.size(); }
    CollisionCylinderData& GetCollisionCylinderData(const std::string& name);
    CollisionCylinderData& GetCollisionCylinderData(const int& index) { return collisionCylinderData_.at(index); }
    std::vector<CollisionCylinderData> GetCollisionCylinderData() { return collisionCylinderData_; }

#pragma endregion [Get, Set] Function

private:
    DirectX::XMFLOAT3   velocity_   = {};   // 速度
    float               maxSpeed_   = 0.0f; // 最大速度

    float               maxHealth_  = 1.0f;
    float               health_     = 1.0f; // 体力

    // ---------- Collision用 ----------
    std::vector<CollisionCylinderData> collisionCylinderData_;
};

