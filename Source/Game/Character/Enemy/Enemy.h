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

        AttackFront,

        BackStepRoar,   // バックステップ後,咆哮

        MoveRoar, // 移動しながら咆哮

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
        AttackTurn0,
        AttackTurn1,
        AttackTurnEnd,

        AttackMove0,
        AttackMove1,
        AttackMove2,
        AttackMove3,
        AttackMove4,
    };

public:
    Enemy(std::string filename);
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

    // ---------- アニメーション関連 ----------


    // ---------- プレイヤーまでの距離を算出 ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- 自分自身からプレイヤーへのベクトル ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

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

