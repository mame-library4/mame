#pragma once
#include "../../../AI/BehaviorTree/ActionBase.h"
#include "Enemy.h"

#if 0
// collisionの有効化,無効化を制御する構造体
struct CollisionState
{
    float   startFrame_ = 0.0f;
    float   endFrame_ = 0.0f;   
    bool    isStart_ = false;
    bool    isEnd_ = false;

    void Initialize(const float& startFrame, const float& endFrame, const bool& isStart = false, const bool& isEnd = false);
};



#pragma region 戦闘

#pragma region 待機系
// 歩き行動
class WalkAction : public ActionBase
{
public:
    WalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

    enum class Side
    {
        Right,  // 右
        Left,   // 左
    };

private:
    float   actionTimer_ = 0.0f;
    Side    playerSide_ = Side::Right; // プレイヤーが左右どっちにいるか
};

class StepAction : public ActionBase
{
public:
    StepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion 待機系

#pragma region 攻撃系
// 噛みつき行動
class BiteAction : public ActionBase
{
public:
    BiteAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 2.0f;     // 攻撃判定無効フレーム
    bool    isAttackCollisionEnd_       = false;    // 制御フラグ

    // ----- 前進するための変数 -----
    float addForceFrame_ = 1.345f;
    bool isAddForce_ = false;

};

// ひっかき行動
class SlashAction : public ActionBase
{
public:
    SlashAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 1.5f;     // 攻撃判定無効になるアニメーションフレーム
    bool    isAttackCollisionEnd_       = false;    // 複数回処理しないようにする制御フラグ
};

// 尻尾行動
class TailSwipeAction : public ActionBase
{
public:
    TailSwipeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// たたきつけ行動
class SlamAction : public ActionBase
{
public:
    SlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    enum class Step
    {
        Initialize, // 初期化
        Search,     // 狙い定める
        Ascend,     // とびかかる
        Attack,     // 攻撃
        Wait, // 攻撃終了後の時間
    };
    void SetStep(const Step& step) { owner_->SetStep(static_cast<int>(step)); }

    void UpdateAttackCollision();

private:
    // ---------- 判定フラグ ---------------
    CollisionState collisionDetection_;
    CollisionState attackDetection_;
    float   disableCollisionPushStartFrame_ = 1.6f; // 押し出し判定無効フレーム


    float               stateChangeFrame_   = 1.3f; // ステートを切り替えるアニメーションのフレーム
    float               ascendEndFrame_     = 1.9f; // 上昇終了フレーム

    float attackEndFrame_ = 2.2f; // たたきつけ終了フレーム

    DirectX::XMFLOAT3   targetPosition_     = {};   // とびかかる位置

    float maxAscend_ = 2.0f; // 最大上昇値

    float easingTimer_ = 0.0f;

    float length_ = 0.0f;

};

#pragma endregion 攻撃系

#pragma region 叫ぶ系
// 咆哮行動
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   blurStartFrame_ = 0.0f; // ブラー開始フレーム
    float   blurEndFrame_   = 0.0f; // ブラー終了フレーム
    float   maxBlurPower_   = 0.0f; // ブラーの強さ
    float   blurTimer_      = 0.0f; // ブラーのEasing用タイマー
    float   maxBlurTime_    = 0.0f; // ブラー用

    bool    isVibration_    = false;// コントローラー振動
};

// 威嚇行動
class IntimidateAction : public ActionBase
{
public:
    IntimidateAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion 叫ぶ系

#pragma endregion 戦闘

#endif