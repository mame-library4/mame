#pragma once
#include "Enemy.h"

class EnemyDragon : public Enemy
{
public:
    EnemyDragon();
    ~EnemyDragon() override {};

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void DebugRender(DebugRenderer* debugRenderer);

private:
    // ---------- BehaviorTree ----------
    void RegisterBehaviorNode();    // behavior登録
    bool CheckStatusChange() override;

    // ---------- Collision ----------
    void RegisterCollisionData();   // collisionData登録
    void UpdateCollisions(const float& elapsedTime) override;

    // ---------- 攻撃判定 ----------
    void ResetAllAttackActiveFlag()                             override;
    void SetSlamAttackActiveFlag(const bool& flag = true)       override;
    void SetTurnAttackActiveFlag(const bool& flag = true)       override;
    void SetTackleAttackActiveFlag(const bool& flag = true)     override;
    void SetFlyAttackActiveFlag(const bool& flag = true)        override;
    void SetComboSlamAttackActiveFlag(const bool& flag = true)  override;
    void SetKnockBackAttackActiveFalg(const bool& flag = true)  override;

    // ---------- ジャスト回避判定 ----------
    void ResetAllJustDodgeActiveFlag() override;
    void SetJustDodgeActiveFlag(const AttackAction& type, const bool& flag) override;

    // ---------- 押し出し判定 ----------
    void SetDownCollisionActiveFlag(const bool& flag = true) override;    

    // ---------- 部位ごとのダメージ処理 ----------
    void AddDamagePart(const float& damage, const int& dataIndex) override;

    void SetAttackPower();

private:
    Object circle_;

    // ----- DebugRenderer表示用フラグ -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    //bool isAttackSphere_    = false;
    //bool isDamageSphere_    = true;
    bool isDamageSphere_    = false;

    //bool isUpdateAnimation_ = false;
    bool isUpdateAnimation_ = true;
    

    float oldHealth_ = 0.0f;

    // テスト用
    bool isJustDodgeDetectionY_ = true;
    //bool isJustDodgeDetectionY_ = false;

    float distanceToPlayer_ = 0.0f;

public:
    enum CollisionData
    {
        DownStart   = 0,
        DownEnd     = 25,
    };
    enum AttackData
    {
        SlamAttackStart         = 0,
        SlamAttackEnd           = 1,
        TrunAttackStart         = 2,
        TrunAttackEnd           = 8,

        TackleAttackStart       = 9,
        TackleAttackEnd         = 13,
        FlyAttackStart          = 14,
        FlyAttackEnd            = 15,
        ComboSlamAttackStart    = 16,
        ComboSlamAttackEnd      = 18,
        KnockBackAttackStart    = 19,
        KnockBackAttackEnd      = 32,
    };
    enum class DamageData
    {
        Head    = 0,  // 頭
        Chest   = 1,  // 胸
        Body    = 2,  // 体
        FrontLeg     = 3,  // 前脚
        FrontLegEnd  = 6,  // 前脚の最後
        BackLeg      = 7,  // 後ろ脚
        BackLegEnd   = 16, // 後ろ脚の最後
        Tail = 17,
        TailEnd = 21,
        Wings = 22,
        WingsEnd = 47,
    };
    enum class JustDodgeData
    {
        SlamAttackStart = 0,
        SlamAttackEnd   = 5,
        TurnAttackStart = 6,
        TurnAttackEnd   = 11,
    };
};

