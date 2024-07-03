#pragma once
#include <DirectXMath.h>
#include <string>
#include "MathHelper.h"

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
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return jointPosition_ + offsetPosition_; }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

    // ---------- 色 ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }

private:
    std::string         name_ = "";   // 名前
    float               radius_ = 0.0f; // 半径
    float               height_ = 0.0f; // 高さ
    DirectX::XMFLOAT3   jointPosition_ = {};   // ジョイント位置
    DirectX::XMFLOAT3   offsetPosition_ = {};   // オフセット位置
    DirectX::XMFLOAT4   color_ = {};   // 色
};

// ----- 球判定用データ -----
struct CollisionSphereData
{
public:
    // ---------- コンストラクタ ----------
    CollisionSphereData(const std::string& name, const float& radius,
        const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = { 1,1,1,1 })
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
    void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }

private:
    std::string         name_ = "";   // 名前
    float               radius_ = 0.0f; // 半径
    DirectX::XMFLOAT3   jointPosition_ = {};   // ジョイント位置
    DirectX::XMFLOAT3   offsetPosition_ = {};   // オフセット位置
    DirectX::XMFLOAT4   color_ = {};   // 色
};

// ----- やられ判定 -----
struct DamageDetectionData
{
public:
    DamageDetectionData(const std::string& name, const float& radius, const float& damage,
        const DirectX::XMFLOAT3& offsetPosition = {}, const std::string& updateName = "")
        : collisionSphereData_(name, radius, offsetPosition, { 0, 0, 1, 1 }),
        updateName_((updateName == "") ? name : updateName),
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
    void SetColor(const DirectX::XMFLOAT4& color) { collisionSphereData_.SetColor(color); }

    // ---------- 更新用名前 ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

    // ---------- ダメージ量 ----------
    [[nodiscard]] const float GetDamage() const { return damage_; }

    // ----- くらった時の管理用 -----
    void Update(const float& elapsedTime);

    [[nodiscard]] const bool GetIsHit() const { return isHit_; }
    void SetIsHit(const bool& hit) { isHit_ = hit; }
    void SetHitTimer(const float& time) { hitTimer_ = time; }

private:
    CollisionSphereData collisionSphereData_;
    std::string         updateName_;            // 更新用の名前
    float               damage_ = 0.0f;         // ダメージ量

    // ----- くらった時の管理用 -----
    bool                isHit_ = false; 
    float               hitTimer_ = 0.0f;
};

// ----- 攻撃判定 -----
struct AttackDetectionData
{
    AttackDetectionData(const std::string& name, const float& radius,
        const DirectX::XMFLOAT3& offsetPosition = {}, const std::string& updateName = "")
        : collisionSphereData_(name, radius, offsetPosition, { 1, 0, 0, 1 }),
        updateName_((updateName == "") ? name : updateName)
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

    // ---------- 更新用名前 ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

private:
    CollisionSphereData collisionSphereData_;   // 球体データ
    std::string         updateName_;            // 更新用の名前
    bool                isActive_ = true;       // 現在有効か
};

// ----- 押し出し判定 -----
struct CollisionDetectionData
{
    CollisionDetectionData(const std::string& name, const float& radius, 
        const DirectX::XMFLOAT3& offsetPosition = {}, const std::string& updateName = "")
        : collisionSphereData_(name, radius, offsetPosition, { 0, 1, 1, 1 }),
        updateName_((updateName == "") ? name : updateName)
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
    //[[nodiscard]] const DirectX::XMFLOAT3 GetCollisionPosition() const { return collisionSphereData_.GetPosition() + collisionSphereData_.GetOffsetPosition(); }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }
    void SetPosition(const DirectX::XMFLOAT3& jointPosition) { collisionSphereData_.SetJointPosition(jointPosition + collisionSphereData_.GetOffsetPosition()); }

    // ---------- 色 ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

    // ---------- 有効(使用)フラグ ----------
    [[nodiscard]] const bool GetIsActive() const { return isActive_; }
    void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    // ---------- 更新用名前 ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

private:
    CollisionSphereData collisionSphereData_;
    bool                isActive_ = true;      // 現在有効か

    std::string updateName_;
};