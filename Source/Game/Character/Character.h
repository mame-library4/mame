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
        [[nodiscard]] const DirectX::XMFLOAT3 GetPosition() const{ return jointPosition_ + offsetPosition_; }
        void SetJointPosition(const DirectX::XMFLOAT3& position) { jointPosition_ = position; }

        // ---------- �F ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }

    private:
        std::string         name_           = "";   // ���O
        float               radius_         = 0.0f; // ���a
        float               height_         = 0.0f; // ����
        DirectX::XMFLOAT3   jointPosition_  = {};   // �W���C���g�ʒu
        DirectX::XMFLOAT3   offsetPosition_ = {};   // �I�t�Z�b�g�ʒu
        DirectX::XMFLOAT4   color_          = {};   // �F
    };

    // ----- ������p�f�[�^ -----
    struct CollisionSphereData
    {
    public:
        // ---------- �R���X�g���N�^ ----------
        CollisionSphereData(const std::string& name, const float& radius,
            const DirectX::XMFLOAT3& offsetPosition = {}, const DirectX::XMFLOAT4& color = {1,1,1,1})
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

    private:
        std::string         name_           = "";   // ���O
        float               radius_         = 0.0f; // ���a
        DirectX::XMFLOAT3   jointPosition_  = {};   // �W���C���g�ʒu
        DirectX::XMFLOAT3   offsetPosition_ = {};   // �I�t�Z�b�g�ʒu
        DirectX::XMFLOAT4   color_          = {};   // �F
    };

    // ----- ���ꔻ�� -----
    struct DamageDetectionData
    {
    public:
        DamageDetectionData(const std::string& name, const float& radius, const float& damage, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 0, 0, 1, 1 }),
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

    private:
        CollisionSphereData collisionSphereData_;
        float               damage_ = 0.0f; // �_���[�W��
    };

    // ----- �U������ -----
    struct AttackDetectionData
    {
        AttackDetectionData(const std::string& name, const float& radius, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 1, 0, 0, 1 })
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

    private:
        CollisionSphereData collisionSphereData_;   // ���̃f�[�^
        bool                isActive_ = false;      // ���ݗL����
    };

    // ----- �����o������ -----
    struct CollisionDetectionData
    {
        CollisionDetectionData(const std::string& name, const float& radius, const DirectX::XMFLOAT3& offsetPosition = {})
            : collisionSphereData_(name, radius, offsetPosition, { 0, 0, 0, 1 })
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
        void SetJointPosition(const DirectX::XMFLOAT3& position) { collisionSphereData_.SetJointPosition(position); }

        // ---------- �F ----------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return collisionSphereData_.GetColor(); }

        // ---------- �L��(�g�p)�t���O ----------
        [[nodiscard]] const bool GetIsActive() const { return isActive_; }
        void SetIsActive(const bool& isActive) { isActive_ = isActive; }

    private:
        CollisionSphereData collisionSphereData_;
        bool                isActive_ = false;      // ���ݗL����
    };

#pragma endregion [Collision] struct

    // ----- Collision�X�V -----
    void UpdateCollisions(const float& scaleFactor);

    void CollisionCharacterVsStage();
    DirectX::XMFLOAT3 SetTargetPosition();
    

    // ----- ������΂����� -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- ���x ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetVelocity() const { return velocity_; }
    [[nodiscard]] const float GetMaxSpeed() const { return maxSpeed_; }
    
    void SetVelocity(const DirectX::XMFLOAT3& velocity) { velocity_ = velocity; }
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

private:
    DirectX::XMFLOAT3   velocity_   = {};   // ���x
    float               maxSpeed_   = 0.0f; // �ő呬�x

    float               rotateSpeed_ = 1.0f; // ��]���x

    float               maxHealth_  = 1.0f;
    float               health_     = 1.0f; // �̗�

    // ---------- Collision�p ----------
    std::vector<DamageDetectionData>        damageDetectionData_;   // ���炢����
    std::vector<AttackDetectionData>        attackDetectionData_;   // �U������
    std::vector<CollisionDetectionData>     collisionDetectionData_;// �����o������

    // ---------- ������� --------------------
    DirectX::XMFLOAT3   blowDirection_  = {};
    float               blowPower_      = 0.0f;
};

