#pragma once
#include <DirectXMath.h>
#include <string>
#include "MathHelper.h"

// ----- �~������p�f�[�^ -----
struct CollisionCylinderData
{
public:
    // ---------- �R���X�g���N�^ ----------
    CollisionCylinderData(const std::string& name, const float& radius, const float& height,
        const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = {})
        : name_(name), radius_(radius), height_(height), offsetPosition_(offsetPosition), color_(color)
    {}
    CollisionCylinderData() = default;

    // ---------- ImGui�p ----------
    void DrawDebug();

    // ---------- ���O ----------
    [[nodiscard]] const std::string GetName() const { return name_; }

    // ---------- ���a ----------
    [[nodiscard]] const float GetRadius() const { return radius_; }

    // ---------- ���� ----------
    [[nodiscard]] const float GetHeight() const { return height_; }

    // ---------- �ʒu ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return jointPosition_ + offsetPosition_; }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

    // ---------- �F ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }

private:
    std::string         name_ = "";   // ���O
    float               radius_ = 0.0f; // ���a
    float               height_ = 0.0f; // ����
    DirectX::XMFLOAT3   jointPosition_ = {};   // �W���C���g�ʒu
    DirectX::XMFLOAT3   offsetPosition_ = {};   // �I�t�Z�b�g�ʒu
    DirectX::XMFLOAT4   color_ = {};   // �F
};

// ----- ������p�f�[�^ -----
struct CollisionSphereData
{
public:
    // ---------- �R���X�g���N�^ ----------
    CollisionSphereData(const std::string& name, const float& radius,
        const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = { 1,1,1,1 })
        : name_(name), radius_(radius), offsetPosition_(offsetPosition), color_(color)
    {}
    CollisionSphereData() = default;

    // ---------- ImGui�p ----------
    void DrawDebug();

    // ---------- ���O ----------
    [[nodiscard]] const std::string GetName() const { return name_; }

    // ---------- ���a ----------
    [[nodiscard]] const float GetRadius() const { return radius_; }

    // ---------- �ʒu ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return jointPosition_; }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

    // ---------- �F ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }
    void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }

private:
    std::string         name_ = "";   // ���O
    float               radius_ = 0.0f; // ���a
    DirectX::XMFLOAT3   jointPosition_ = {};   // �W���C���g�ʒu
    DirectX::XMFLOAT3   offsetPosition_ = {};   // �I�t�Z�b�g�ʒu
    DirectX::XMFLOAT4   color_ = {};   // �F
};

// ----- ���ꔻ�� -----
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

    // ---------- ImGui�p ----------
    void DrawDebug();

    // ---------- ���O ----------
    [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

    // ---------- ���a ----------
    [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

    // ---------- �ʒu ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }
    void SetPosition(const DirectX::XMFLOAT3& jointPosition) { collisionSphereData_.SetJointPosition(jointPosition + collisionSphereData_.GetOffsetPosition()); }

    // ---------- �F ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }
    void SetColor(const DirectX::XMFLOAT4& color) { collisionSphereData_.SetColor(color); }

    // ---------- �X�V�p���O ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

    // ---------- �_���[�W�� ----------
    [[nodiscard]] const float GetDamage() const { return damage_; }

    // ----- ����������̊Ǘ��p -----
    void Update(const float& elapsedTime);

    [[nodiscard]] const bool GetIsHit() const { return isHit_; }
    void SetIsHit(const bool& hit) { isHit_ = hit; }
    void SetHitTimer(const float& time) { hitTimer_ = time; }

private:
    CollisionSphereData collisionSphereData_;
    std::string         updateName_;            // �X�V�p�̖��O
    float               damage_ = 0.0f;         // �_���[�W��

    // ----- ����������̊Ǘ��p -----
    bool                isHit_ = false; 
    float               hitTimer_ = 0.0f;
};

// ----- �U������ -----
struct AttackDetectionData
{
    AttackDetectionData(const std::string& name, const float& radius,
        const DirectX::XMFLOAT3& offsetPosition = {}, const std::string& updateName = "")
        : collisionSphereData_(name, radius, offsetPosition, { 1, 0, 0, 1 }),
        updateName_((updateName == "") ? name : updateName)
    {}
    AttackDetectionData() = default;

    // ---------- ImGui�p ----------
    void DrawDebug();

    // ---------- ���O ----------
    [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

    // ---------- ���a ----------
    [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

    // ---------- �ʒu ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }

    // ---------- �F ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

    // ---------- �L��(�g�p)�t���O ----------
    [[nodiscard]] const bool GetIsActive() const { return isActive_; }
    void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    // ---------- �X�V�p���O ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

private:
    CollisionSphereData collisionSphereData_;   // ���̃f�[�^
    std::string         updateName_;            // �X�V�p�̖��O
    bool                isActive_ = true;       // ���ݗL����
};

// ----- �����o������ -----
struct CollisionDetectionData
{
    CollisionDetectionData(const std::string& name, const float& radius, 
        const DirectX::XMFLOAT3& offsetPosition = {}, const std::string& updateName = "")
        : collisionSphereData_(name, radius, offsetPosition, { 0, 1, 1, 1 }),
        updateName_((updateName == "") ? name : updateName)
    {}
    CollisionDetectionData() = default;

    // ---------- ImGui�p ----------
    void DrawDebug();

    // ---------- ���O ----------
    [[nodiscard]] const std::string GetName() const { return collisionSphereData_.GetName(); }

    // ---------- ���a ----------
    [[nodiscard]] const float GetRadius() const { return collisionSphereData_.GetRadius(); }

    // ---------- �ʒu ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return collisionSphereData_.GetOffsetPosition(); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const { return collisionSphereData_.GetPosition(); }
    //[[nodiscard]] const DirectX::XMFLOAT3 GetCollisionPosition() const { return collisionSphereData_.GetPosition() + collisionSphereData_.GetOffsetPosition(); }
    void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }
    void SetPosition(const DirectX::XMFLOAT3& jointPosition) { collisionSphereData_.SetJointPosition(jointPosition + collisionSphereData_.GetOffsetPosition()); }

    // ---------- �F ----------
    [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

    // ---------- �L��(�g�p)�t���O ----------
    [[nodiscard]] const bool GetIsActive() const { return isActive_; }
    void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    // ---------- �X�V�p���O ----------
    [[nodiscard]] const std::string GetUpdateName() const { return updateName_; }

private:
    CollisionSphereData collisionSphereData_;
    bool                isActive_ = true;      // ���ݗL����

    std::string updateName_;
};