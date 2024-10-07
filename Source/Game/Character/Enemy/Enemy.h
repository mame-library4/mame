#pragma once
#include <memory>
#include "../Character.h"
#include "Graphics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorData.h"
#include "BehaviorTree/NodeBase.h"

class Enemy : public Character
{
public:
    enum class DragonAnimation
    {
        Idle0,          // 待機0
        Idle1,          // 待機１
        Walk,           // 歩き
        Run,            // 走り
        
        // ----- ダメージ食らい -----
        GetHitStart,    // 食らい始め    
        GetHitLoop,     // 食らいループ
        GetHitEnd,      // 食らい終わり

        // ----- 死亡 -----
        Death,          // 死亡
        DeathLoop,      // 死亡ループ

        Roar,           // 咆哮
        
        AttackSlam0,
        AttackSlam1,

        AttackStep,

        // ----- 突進攻撃 -----
        AttackTackle0,  // 予備動作
        AttackTackle1,  // 突進始まり
        AttackTackle2,  // 突進
        AttackTackle3,  // 後隙

        BackStep,       // バックステップ

        // ----- 上昇攻撃 -----
        AttackRise,     // 上昇
        AttackRiseLoop, // 上昇ループ
        AttackRiseEnd,  // 攻撃

        // ----- 空中からたたきつけ攻撃 -----
        AttackFly0,     // 空中へ飛ぶ
        AttackFly1,     // 空中待機
        AttackFly2,     // たたきつけ攻撃

        FireBreathFront,

        BackStepRoar,   // バックステップ後,咆哮


        // ----- 吹き飛ばし攻撃 -----
        AttackKnockBackStart,
        AttackKnockBackLoop,
        AttackKnockBackEnd0,
        AttackKnockBackEnd1,

        SleepEnd,

        // ----- クリティカル -----
        CriticalStart,
        CriticalLoop,
        CriticalEnd,

        // ----- コンボたたきつけ攻撃 -----
        AttackComboSlam0,
        AttackComboSlam1,
        AttackComboSlamEnd,

        // ----- 叫びコンボ -----
        ComboRoarStart,
        ComboRoarLoop,
        ComboRoarEnd0,
        ComboRoarEnd1,

        BackStepAttack,

        // ----- コンボ溜め攻撃 -----
        AttackComboCharge0,
        AttackComboCharge1,
        AttackComboCharge2,
        AttackComboCharge3,
        AttackComboCharge4,
        AttackComboCharge5,

        // ----- 回転攻撃 -----
        AttackTurnStart,
        AttackTurn,
        AttackTurnEnd,

        AttackMove0,
        AttackMove1,
        AttackMove2,
        
        FireBreathLeft,
        FireBreathRight,

        Idle2,
        Damage,
        Fly,

        Nova1,
        FlyTurn,
        Nova2,
    };

    enum class PartName { Head, Chest, Body, Leg, Tail, Wings, Max };

public:
    Enemy(const std::string& filename, const float& scaleFactor);
    ~Enemy() override {}

    virtual void Initialize()                       = 0;
    virtual void Finalize()                         = 0;
    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug()                        = 0;
    virtual void DebugRender(DebugRenderer* debugRenderer) = 0;

    void Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos);

    [[nodiscard]] const bool SearchPlayer();

    // ---------- BehaviorTree ----------
    void UpdateNode(const float& elapsedTime);
    virtual bool CheckStatusChange() = 0;

    // ---------- アニメーション関連 ----------
    void PlayAnimation(const DragonAnimation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const DragonAnimation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }

    // ---------- プレイヤーまでの距離を算出 ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- 自分自身からプレイヤーへのベクトル ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

    // ---------- ダメージ ----------
    void AddDamage(const float& damage, const int& dataIndex);

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ----- BehaviorTree用 -----
    [[nodiscard]] const int GetStep() const { return step_; }
    void SetStep(const int& step) { step_ = step; }

    // ----- ひるみ判定 -----
    [[nodiscard]] const bool GetIsFlinch() const { return isFlinch_; }
    void SetIsFlinch(const bool& flinch) { isFlinch_ = flinch; }

    // ----- 戦闘開始範囲 -----
    [[nodiscard]] const float GetBattleRadius() const { return battleRadius_; }
    void SetBattleRadius(const float& radius) { battleRadius_ = radius; }

    // ----- 攻撃開始範囲 -----
    [[nodiscard]] const float GetNearAttackRadius() const { return nearAttackRadius_; }
    void SetNearAttackRadius(const float& radius) { nearAttackRadius_ = radius; }

    [[nodiscard]] const float GetComboFlyAttackRadius() const { return comboFlyAttackRadius_; }
    void SetComboFlyAttackRadius(const float& radius) { comboFlyAttackRadius_ = radius; }

    // ----- 移動先位置 -----
    [[nodiscard]] const DirectX::XMFLOAT3 GetMovePosition() const { return movePosition_; }
    void SetMovePosition(const DirectX::XMFLOAT3& movePos) { movePosition_ = movePos; }

    // ----- 連続攻撃回数 -----
    [[nodiscard]] const int GetAttackComboCount() const { return attackComboCount_; }
    void SetAttackComboCount(const int& count) { attackComboCount_ = count; }
    void AddAttackComboCount() { ++attackComboCount_; }

    // ----- 歩行速度 -----
    [[nodiscard]] const float GetWalkSpeed() const { return walkSpeed_; }
    void SetWalkSpeed(const float& speed) { walkSpeed_ = speed; }

    // ----- 攻撃判定が有効か -----
    [[nodiscard]] const bool GetIsAttackActive() const { return isAttackActive_; }
    void SetIsAttackActive(const bool& flag) { isAttackActive_ = flag; }

    [[nodiscard]] const bool GetIsRoar() const { return isRoar_; }
    void SetIsRoar(const bool& flag) { isRoar_ = flag; }

    // ----- 攻撃ダメージ -----
    [[nodiscard]] const float GetAttackDamage() const { return attackDamage_; }
    void SetAttackDamage(const float& damage) { attackDamage_ = damage; }

#pragma endregion [Get, Set] Function

    [[nodiscard]] const std::string GetActiveNodeName() const { return (activeNode_ != nullptr) ? activeNode_->GetName() : ""; }

    // ---------- 攻撃判定 ----------
    virtual void ResetAllAttackActiveFlag()                             = 0; // 全攻撃判定無効化
    virtual void SetTurnAttackActiveFlag(const bool& flag = true)       = 0; // 回転攻撃
    virtual void SetTackleAttackActiveFlag(const bool& flag = true)     = 0; // 突進攻撃
    virtual void SetFlyAttackActiveFlag(const bool& flag = true)        = 0; // 上昇攻撃
    virtual void SetComboSlamAttackActiveFlag(const bool& flag = true)  = 0; // たたきつけ攻撃
    virtual void SetKnockBackAttackActiveFalg(const bool& flag = true)  = 0; // 吹き飛ばし攻撃

    // ---------- 押し出し判定 ----------
    virtual void SetDownCollisionActiveFlag(const bool& flag = true) = 0;

    [[nodiscard]] const bool GetIsStageCollisionJudgement() const { return isStageCollisionJudgement_; }
    void SetIsStageCollisionJudgement(const bool& flag) { isStageCollisionJudgement_ = flag; }

    // ---------- 怯み判定用データ ----------
    [[nodiscard]] const int GetFlinchDetectionDataCount() const { return flinchDetectionData_.size(); }
    std::vector<AttackDetectionData> GetFlinchDetectionData() { return flinchDetectionData_; }
    AttackDetectionData& GetFlinchDetectionData(const int& index) { return flinchDetectionData_.at(index); }

    [[nodiscard]] const bool GetIsPartDestruction(const PartName& partName) const { return isPartDestruction_[static_cast<int>(partName)]; }

private:
    // ---------- 部位破壊 ----------
    virtual void AddDamagePart(const float& damage, const int& dataIndex) = 0;
    void CheckPartDestruction(); // 部位破壊判定


protected:
    std::unique_ptr<BehaviorTree>   behaviorTree_;
    std::unique_ptr<BehaviorData>   behaviorData_;
    NodeBase*                       activeNode_ = nullptr;

    std::vector<AttackDetectionData> flinchDetectionData_; // 怯み判定用データ

    int     step_       = 0;        // 行動ステップ
    bool    isFlinch_   = false;    // ひるみフラグ

    // ---------- 戦闘判定範囲変数 ----------
    float               battleRadius_       = 20.0f;// 戦闘開始範囲
    
    float               nearAttackRadius_       = 15.0f; // 近距離攻撃開始範囲
    float               comboFlyAttackRadius_   = 8.5f;  // ３連続たたきつけ攻撃   

    float attackDamage_ = 0.0f; // 攻撃ダメージ

    DirectX::XMFLOAT3 movePosition_ = {}; // 移動先位置

    int     attackComboCount_ = 0; // 連続攻撃回数

    float walkSpeed_ = 0.0f; // 歩行速度

    // ---------- 状況管理フラグ ----------
    bool isAttackActive_    = false; // 攻撃判定が有効か
    bool isRoar_            = false; // 咆哮したか

    bool isStageCollisionJudgement_ = false; // ステージとの判定をするか

    // ---------- 部位破壊 ----------
    float partHealth_[static_cast<int>(PartName::Max)]        = {}; // 部位ごとの体力
    bool  isPartDestruction_[static_cast<int>(PartName::Max)] = {}; // 部位破壊判定フラグ

};

