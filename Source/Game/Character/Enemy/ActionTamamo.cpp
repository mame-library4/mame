#include "ActionTamamo.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"
#include "../Player/PlayerManager.h"
#include "Camera.h"

// ----- 死亡行動 -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // アニメーション再生

        timer_ = 0.0f;

        owner_->SetStep(1);
        break;
    case 1:
        timer_ += elapsedTime;

        if (timer_ > 1.0f)
        {
            owner_->isWin_ = true;
            owner_->SetStep(2);
        }

        break;
    case 2:
        // 死亡なのでこのまま放置

        float posY = owner_->GetTransform()->GetPositionY();

        posY -= elapsedTime * 5.0f;
        float max = -100.0f;
        posY = std::max(max, posY);

        owner_->GetTransform()->SetPositionY(posY);

        break;
    }

    return ActionBase::State::Run;
}

// ----- ひるみ行動 -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // ひるみアニメーション再生
        owner_->PlayAnimation(Enemy::TamamoAnimation::Filnch, false, 1.0f);

        owner_->SetStep(1);
        break;
    case 1:
        // アニメーション再生終了
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

#pragma region 非戦闘
// ----- 非戦闘待機行動 -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    // アニメーション再生 ( Idle )
    //owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::Idle, true);

    return ActionBase::State::Run;
}

// ----- 非戦闘歩き行動 -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{

    DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();

    switch (owner_->GetStep())
    {
    case 0:
    {
        // アニメーション再生
        if (owner_->GetCurrentBlendAnimationIndex() < 0)
        {
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Idle, Enemy::TamamoAnimation::Walk, true);
            owner_->SetWeight(1.0f);
        }
        else
        {
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, true);
            owner_->SetWeight(0.0f);
        }

        DirectX::XMFLOAT3 movePos = owner_->SetTargetPosition();

        owner_->SetMovePosition(movePos);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        // 戦闘状態になる
        if (owner_->SearchPlayer())
        {
            owner_->SetStep(0);
            //return ActionBase::State::Complete;
            return ActionBase::State::Failed;
        }

        owner_->CollisionCharacterVsStage();

        owner_->AddWeight(elapsedTime * 2.0f);

        DirectX::XMFLOAT3 movePos = owner_->GetMovePosition();

        // 進行方向を求める
        DirectX::XMFLOAT3 direction = movePos - ownerPos;

        // 目的地に着いたか判定
        float length = XMFloat3Length(direction);
        if (length <= 1.0f)
        {
            // 目的地に着いたので終了
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        // 進む速度を計算
        direction = XMFloat3Normalize(direction);
        float speed = owner_->GetWalkSpeed() * elapsedTime;
        direction = direction * speed;

        owner_->GetTransform()->AddPosition(direction);

        // 旋回処理
        owner_->Turn(elapsedTime, owner_->GetMovePosition());

    }
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion 非戦闘

#pragma region 戦闘

#pragma region 待機系 
// ----- 歩き行動 -----
const ActionBase::State WalkAction::Run(const float& elapsedTime)
{
    DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 vec = playerPos - ownerPos;

    switch (owner_->GetStep())
    {
    case 0:
    {
        // 外積でプレイヤーが左右どっちにいるか判定
        float cross = XMFloat2Cross(ownerFront, vec);
        // 現在の方向を保存する
        playerSide_ = (cross > 0) ? Side::Right : Side::Left;

        // 左右で再生するアニメーションを切り替える
        if (playerSide_ == Side::Left)
        {
            // 今回設定するアニメーションが前回と同じでなければ初期化する
            if (owner_->PlayBlendAnimation(Enemy::TamamoAnimation::WalkRight, true))
            {
                // ウェイト値初期化
                owner_->SetWeight(0.0f);
            }
        }
        else
        {
            // 今回設定するアニメーションが前回と同じでなければ初期化する
            if (owner_->PlayBlendAnimation(Enemy::TamamoAnimation::WalkLeft, true))
            {
                // ウェイト値初期化
                owner_->SetWeight(0.0f);
            }
        }

        // タイマー初期化
        actionTimer_ = 2.0f;

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

        vec = XMFloat2Normalize(vec);
        // プレイヤーが右にいる
        if (playerSide_ == Side::Right)
        {
            // 左ベクトル算出
            vec = { -vec.y, vec.x };
        }
        // プレイヤーが左にいる
        if (playerSide_ == Side::Left)
        {
            // 右ベクトル算出
            vec = { vec.y, -vec.x };
        }

        owner_->GetTransform()->AddPositionX(vec.x * elapsedTime * 2.0f);
        owner_->GetTransform()->AddPositionZ(vec.y * elapsedTime * 2.0f);
    }


        // 旋回処理
        owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());

        owner_->AddWeight(elapsedTime * 2.0f);
        actionTimer_ -= elapsedTime;
        if (actionTimer_ <= 0.0f)
        {
            // プイレイヤーが範囲外に出た時
            if (owner_->SearchPlayer() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            else
            {
                owner_->SetStep(0);
                return ActionBase::State::Failed;
            }
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- ステップ行動 -----
const ActionBase::State StepAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        break;
    case 1:
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion 待機系

#pragma region 攻撃系
// ----- 噛みつき行動 -----
const ActionBase::State BiteAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // アニメーション設定 ( 噛みつき )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Bite, false);

        // 噛みつき攻撃判定有効化
        owner_->SetBiteAttackFlag();

        // 変数初期化
        isAttackCollisionEnd_ = false;
        isAddForce_ = false;    

        owner_->SetWeight(0.0f);

        owner_->SetStep(1);
        break;
    case 1:
        owner_->AddWeight(2.0f * elapsedTime);

        // アニメーションに合わせて攻撃判定を無効化する
        if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
            isAttackCollisionEnd_ == false)
        {
            // 一度だけ処理するように制御する
            isAttackCollisionEnd_ = true;

            // 噛みつき攻撃判定無効化
            owner_->SetBiteAttackFlag(false);
        }

        // アニメーションに合わせて少し前進する
        if (owner_->GetBlendAnimationSeconds() > addForceFrame_ &&
            isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(owner_->GetTransform()->CalcForward(), 0.3f);
        }

        // アニメーションが終わったら終了
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- ひっかき行動 -----
const ActionBase::State SlashAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // アニメーション設定 ( ひっかき )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Slash, false);

        // ひっかき攻撃判定設定
        owner_->SetSlashAttackFlag();

        // 変数初期化
        isAttackCollisionEnd_ = false;

        owner_->SetStep(1);

        break;
    case 1:

        // アニメーションに合わせて攻撃判定を無効化する
        if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
            isAttackCollisionEnd_ == false)
        {            
            // 一度だけしか処理しないように制御する
            isAttackCollisionEnd_ = true;

            // ひっかき攻撃判定を無効化する
            owner_->SetSlashAttackFlag(false);
        }


        // アニメーションが終わったら終了
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- 尻尾行動 -----
const ActionBase::State TailSwipeAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // アニメーション設定 ( 尻尾 )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::TailSwipe, false);

        // 尻尾攻撃判定設定
        owner_->SetTailSwipeAttackFlag();

        owner_->SetStep(1);
        break;
    case 1:


        // アニメーションが終わったら終了
        if (owner_->IsPlayAnimation() == false)
        {
            // 攻撃判定リセット
            owner_->SetTailSwipeAttackFlag(false);

            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- たたきつけ行動 -----
const ActionBase::State SlamAction::Run(const float& elapsedTime)
{
    DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 ownerPosition = owner_->GetTransform()->GetPosition();
    DirectX::XMFLOAT2 ownerPos = { ownerPosition.x, ownerPosition.z };
    DirectX::XMFLOAT2 targetPos = { targetPosition_.x, targetPosition_.z };

    // アニメーションに合わせて攻撃判定を有効化、無効化の切り替え
    UpdateAttackCollision();

    owner_->AddWeight(2.0f * elapsedTime);

    switch (static_cast<Step>(owner_->GetStep()))
    {
    case Step::Initialize:// 初期設定
        // アニメーション再生 ( たたきつけ )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Slam, false);
        owner_->SetWeight(0.0f);
        
        // 変数初期化
        isAttackCollisionStart_ = false;
        isAttackCollisionEnd_ = false;

        SetStep(Step::Search);
        break;
    case Step::Search:// 狙いを定める

        // 回転処理
        owner_->Turn(elapsedTime, playerPosition);

        // 指定しているアニメーションのフレームを超えると次の状態に移る
        if (owner_->GetBlendAnimationSeconds() > stateChangeFrame_)
        {
            // とびかかる位置を設定する
            targetPosition_ = playerPosition;

            targetPos = { targetPosition_.x, targetPosition_.z };

            // 自分自身とターゲットの距離を算出
            length_ = XMFloat2Length(targetPos - ownerPos);

            SetStep(Step::Ascend);
        }        

        break;
    case Step::Ascend:// 上昇する
    {
        // Y 方向移動
        easingTimer_ += elapsedTime;
        ownerPosition.y = Easing::InCirc(easingTimer_, ascendEndFrame_ - stateChangeFrame_, maxAscend_, 0.0f);
        owner_->GetTransform()->SetPositionY(ownerPosition.y);
                
        // XZ 方向移動
        DirectX::XMFLOAT2 moveVec = XMFloat2Normalize(targetPos - ownerPos);
        float moveLength = (length_ / 3) / (ascendEndFrame_ - stateChangeFrame_) * elapsedTime;
        moveVec = moveVec * moveLength;
        owner_->GetTransform()->AddPositionX(moveVec.x);
        owner_->GetTransform()->AddPositionZ(moveVec.y);

        // 回転処理
        owner_->Turn(elapsedTime, targetPosition_);

        // 指定しているアニメーションのフレームを超えると次の状態に移る
        if (owner_->GetBlendAnimationSeconds() > ascendEndFrame_)
        {
            // タイマーリセット
            easingTimer_ = 0.0f;

            SetStep(Step::Attack);
        }
    }
        break;
    case Step::Attack:// 実際にたたきつけ攻撃に入る
    {
        easingTimer_ += elapsedTime;
        ownerPosition.y = Easing::InCubic(easingTimer_, attackEndFrame_ - ascendEndFrame_, 0.0f, maxAscend_);
        owner_->GetTransform()->SetPositionY(ownerPosition.y);

        // XZ 方向移動
        DirectX::XMFLOAT2 moveVec = XMFloat2Normalize(targetPos - ownerPos);
        float moveLength = (length_ / 3)  / (attackEndFrame_ - ascendEndFrame_) * elapsedTime;
        moveVec = moveVec * moveLength;
        owner_->GetTransform()->AddPositionX(moveVec.x);
        owner_->GetTransform()->AddPositionZ(moveVec.y);

        // 回転処理
        owner_->Turn(elapsedTime, targetPosition_);

        if (owner_->GetBlendAnimationSeconds() > attackEndFrame_)
        {
            // カメラシェイクを入れる
            Camera::Instance().ScreenVibrate(0.1f, 1.0f);

            owner_->InitializeStones();

            // 位置を０に設定
            owner_->GetTransform()->SetPositionY(0.0f);
            
            // タイマーリセット
            easingTimer_ = 0.0f;

            SetStep(Step::Wait);
        }

    }
        break;
    case Step::Wait:

        // アニメーションが終わったら終了
        if (owner_->IsPlayAnimation() == false)
        {
            SetStep(Step::Initialize);

            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State();
}

// ----- アニメーションに合わせて攻撃判定を有効化、無効化の切り替え -----
void SlamAction::UpdateAttackCollision()
{
    // アニメーションに合わせて攻撃判定を有効化する
    if (owner_->GetBlendAnimationSeconds() > attackCollisionStartFrame_ &&
        isAttackCollisionStart_ == false)
    {
        // 制御フラグを立てる
        isAttackCollisionStart_ = true;

        // たたきつけ攻撃判定を有効化する
        owner_->SetSlamAttackFlag();
    }
    // アニメーションに合わせて攻撃判定を無効化する
    else if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
        isAttackCollisionEnd_ == false)
    {
        // 制御フラグを立てる
        isAttackCollisionEnd_ = true;

        // たたきつけ攻撃判定を無効化する
        owner_->SetSlamAttackFlag(false);
    }
}

#pragma endregion 攻撃系

#pragma region 叫ぶ系
// ----- 咆哮行動 -----
const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        //owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Roar, false);
        owner_->SetStep(1);
        break;
    case 1:

        if (!owner_->IsPlayAnimation())
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- 威嚇行動 -----
const ActionBase::State IntimidateAction::Run(const float& elapsedTime)
{
    return ActionBase::State::Run;
}

#pragma endregion 叫ぶ系

#pragma endregion 戦闘


