#pragma once
#include <memory>
#include "../Character.h"
#include "../Graphics/Graphics.h"

#if 1
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorData.h"
#include "BehaviorTree/NodeBase.h"
#else
class BehaviorTree;
class BehaviorData;
class NodeBase;
#endif

class Enemy : public Character
{
public:
    enum class TamamoAnimation
    {
        Idle,       // 待機
        Walk,       // 歩き
        WalkRight,  // 斜め歩き (右)
        WalkLeft,   // 斜め歩き (左)
        Bite,       // 噛みつき
        Slash,      // ひっかき
        TailSwipe,  // 尻尾
        Roar,       // 咆哮
        Filnch,     // ひるみ
        Slam,       // たたきつけ

#if 0
        Bite,           // 噛みつき
        Slash,          // ひっかき
        TailSwipe,      // 尻尾
        SpinAttack,     // 回転
        SpineShot,      // 棘
        Tackle,         // 突進
        Pounce,         // とびかかる
        Roar,           // 咆哮
        Intimidate,     // 威嚇
        Filnch,         // ひるみ
        Walk,           // 歩き
        Step,           // ステップ
        Idle,           // 待機
#endif
    };

public:
    Enemy(std::string filename);
    ~Enemy() override {}

    virtual void Initialize()                       = 0;
    virtual void Finalize()                         = 0;
    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render()                           = 0;
    virtual void RenderUserInterface()              = 0;
    virtual void DrawDebug()                        = 0;
    virtual void DebugRender(DebugRenderer* debugRenderer) = 0;

    void Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos);

    [[nodiscard]] const bool SearchPlayer();

    // ---------- BehaviorTree ----------
    void UpdateNode(const float& elapsedTime);

    // ---------- アニメーション関連 ----------
    void PlayAnimation(const TamamoAnimation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const TamamoAnimation& index1, const TamamoAnimation& index2, const bool& loop, const float& speed = 1.0f) { Object::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed); }
    bool PlayBlendAnimation(const TamamoAnimation& index, const bool& loop, const float& speed = 1.0f);

    // ---------- プレイヤーまでの距離を算出 ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- 自分自身からプレイヤーへのベクトル ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

    // TODO:消す。これほんまにいらん
    bool isWin_ = false;

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
    [[nodiscard]] const float GetFarAttackRadius() const { return farAttackRadius_; }
    void SetFarAttackRadius(const float& radius) { farAttackRadius_ = radius; }

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

#pragma endregion [Get, Set] Function

#pragma region 九尾用 攻撃判定設定
    virtual void SetAllAttackFlag(const bool& activeFlag = false) {}
    virtual void SetBiteAttackFlag(const bool& activeFlag = true) {}
    virtual void SetSlashAttackFlag(const bool& activeFlag = true) {}
    virtual void SetTailSwipeAttackFlag(const bool& activeFlag = true) {}
    virtual void SetSlamAttackFlag(const bool& activeFlag = true) {}

    virtual void InitializeStones() {}

#pragma endregion 九尾用 攻撃判定設定

protected:
    std::unique_ptr<BehaviorTree>   behaviorTree_;
    std::unique_ptr<BehaviorData>   behaviorData_;
    NodeBase*                       activeNode_ = nullptr;

    int     step_       = 0;        // 行動ステップ
    bool    isFlinch_   = false;    // ひるみフラグ

    // ---------- 戦闘判定範囲変数 ----------
    float               battleRadius_       = 20.0f;// 戦闘開始範囲
    float               nearAttackRadius_   = 6.0f; // 近距離攻撃開始範囲
    float               farAttackRadius_    = 14.0f;// 遠距離攻撃開始範囲


    DirectX::XMFLOAT3 movePosition_ = {}; // 移動先位置

    int     attackComboCount_ = 0; // 連続攻撃回数

    float walkSpeed_ = 0.0f; // 歩行速度
};

