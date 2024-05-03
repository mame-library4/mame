#include "ActionTamamo.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- 死亡行動 -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
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

// ----- ひるみ行動 -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // ひるみアニメーション再生
        //owner_->PlayAnimation(Enemy::TamamoAnimation::Filnch, false, 1.0f);


        owner_->SetStep(1);
        break;
    case 1:
        break;
    }

    return ActionBase::State::Run;
}

#pragma region 非戦闘
// ----- 非戦闘待機行動 -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    // アニメーション再生 ( Idle )
    owner_->PlayAnimation(Enemy::TamamoAnimation::Idle, true);

    return ActionBase::State::Run;
}

// ----- 非戦闘歩き行動 -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{
    // アニメーション再生
    owner_->PlayAnimation(Enemy::TamamoAnimation::Walk, true);

    DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();

    switch (owner_->GetStep())
    {
    case 0:
    {
        DirectX::XMFLOAT3 movePos = { ownerPos.x + (rand() % 10 - 5), ownerPos.y, ownerPos.z + (rand() % 10 - 5) };
        movePos = owner_->SetTargetPosition(movePos);

        owner_->SetMovePosition(movePos);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

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
        direction = direction * elapsedTime * 1.0f;

        owner_->GetTransform()->AddPosition(direction);

        // 旋回処理
        owner_->Turn(elapsedTime, owner_->GetMovePosition());

    }
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion 非戦闘

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
        // プレイヤーが右にいる
        if (cross > 0)
        {
            // アニメーション再生 ( 歩き左 )
            //owner_->PlayAnimation(Enemy::TamamoAnimation::WalkLeft, true);
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::WalkLeft, true);
        }
        // プレイヤーが左にいる
        else
        {
            // アニメーション再生 ( 歩き右 )
            //owner_->PlayAnimation(Enemy::TamamoAnimation::WalkRight, true);
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::WalkRight, true);
        }

        // 初期化
        actionTimer_ = 3.0f;
        owner_->SetWeight(0.0f);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

        // 外積でプレイヤーが左右どっちにいるか判定
        float cross = XMFloat2Cross(ownerFront, vec);
        vec = XMFloat2Normalize(vec);
        // プレイヤーが右にいる
        if (cross > 0)
        {
            // 左ベクトル算出
            vec = { -vec.y, vec.x };
        }
        // プレイヤーが左にいる
        else
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
            owner_->SetStep(0);
            return ActionBase::State::Failed;
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::Bite, false);

        owner_->SetStep(1);

        break;
    case 1:
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::Slash, false);

        owner_->SetStep(1);

        break;
    case 1:
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::TailSwipe, false);

        owner_->SetStep(1);

        break;
    case 1:
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

#pragma endregion 攻撃系

#pragma region 叫ぶ系
// ----- 咆哮行動 -----
const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        //owner_->PlayAnimation(Enemy::TamamoAnimation::Roar, false);
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


