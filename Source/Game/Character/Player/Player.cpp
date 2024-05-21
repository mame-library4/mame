#include "Player.h"
#include "PlayerState.h"
#include "../../../Graphics/Graphics.h"

// ----- コンストラクタ -----
Player::Player()
    : Character("./Resources/Model/Character/Player.glb"),
    swordModel_("./Resources/Model/Character/Sword.glb")
{
    // --- ステートマシン ---
    {
        stateMachine_.reset(new StateMachine<State<Player>>);

        // ステートを登録する
        GetStateMachine()->RegisterState(new PlayerState::IdleState(this));                 // 待機
        GetStateMachine()->RegisterState(new PlayerState::WalkState(this));                 // 歩き
        GetStateMachine()->RegisterState(new PlayerState::RunState(this));                  // 走り
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));            // 回避
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));              // カウンター
        GetStateMachine()->RegisterState(new PlayerState::CounterAttackState(this));        // カウンター攻撃
        GetStateMachine()->RegisterState(new PlayerState::LightAttack0State(this));         // 弱攻撃0
        GetStateMachine()->RegisterState(new PlayerState::LightAttack1State(this));         // 弱攻撃1
        GetStateMachine()->RegisterState(new PlayerState::LightAttack2State(this));         // 弱攻撃2
        GetStateMachine()->RegisterState(new PlayerState::StrongAttack0State(this));        // 強攻撃0
        GetStateMachine()->RegisterState(new PlayerState::StrongAttack1State(this));        // 強攻撃1       
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));               // ダメージ

        // 一番初めのステートを設定する
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }
}

// ----- デストラクタ -----
Player::~Player()
{
}

// ----- 初期化 -----
void Player::Initialize()
{
    // 押し出し判定用変数設定
    RegisterCollisionDetectionData({ "collide0", 0.25f, { 0, 1.3f, 0} });
    RegisterCollisionDetectionData({ "collide1", 0.25f, { 0, 0.8f, 0} });
    RegisterCollisionDetectionData({ "collide2", 0.25f, { 0, 0.3f, 0} });

    // くらい判定
    RegisterDamageDetectionData({ "R:R:j_Head_end", 0.15f,  10, { -8, 2, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Spine2",   0.2f,   10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Hips",     0.2f,   10, { 10, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Reg_R",    0.15f,  10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Leg_L",    0.15f,  10, { 0, 0, 0 } });

    // 攻撃判定
    RegisterAttackDetectionData({ "R1:R:j_Sword", 0.15f, { 0, 35, 0} });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -35, 0 } });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -5, 0 } });

    // 体力設定
    SetMaxHealth(100.0f);
    SetHealth(GetMaxHealth());
}

void Player::Finalize()
{
}

// ----- 更新 -----
void Player::Update(const float& elapsedTime)
{
    swordModel_.GetTransform()->SetPosition(GetTransform()->GetPosition());
    swordModel_.GetTransform()->SetRotation(GetTransform()->GetRotation());
    
    // Collisionデータ更新
    UpdateCollisions(elapsedTime, 0.01f);
    GetCollisionDetectionData("collide0").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide1").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide2").SetPosition(GetTransform()->GetPosition());

    // アニメーション更新
    Character::Update(elapsedTime);
    swordModel_.UpdateAnimation(elapsedTime);

    Camera::Instance().SetTarget(GetTransform()->GetPosition() + offset_);

    GetStateMachine()->Update(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();

    //GetTransform()->SetPositionY(0.0f);
}

// ----- 描画 -----
void Player::Render()
{
    const float scaleFactor = 0.01f;

    Object::Render(scaleFactor);
    swordModel_.Render(scaleFactor);
}

// ----- ImGui用 -----
void Player::DrawDebug()
{
    if (ImGui::TreeNode("Player"))
    {
        ImGui::DragFloat3("offset", &offset_.x);

        ImGui::Checkbox("Collision", &isCollisionSphere_);
        ImGui::Checkbox("Damage", &isDamageSphere_);
        ImGui::Checkbox("Attack", &isAttackSphere_);

        Character::DrawDebug();
        Object::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- デバッグ用 -----
void Player::DebugRender(DebugRenderer* debugRenderer)
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition();

    if (isCollisionSphere_)
    {
        for (auto& data : GetCollisionDetectionData())
        {
            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }
    if (isAttackSphere_)
    {
        for (auto& data : GetAttackDetectionData())
        {
            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }
    if (isDamageSphere_)
    {
        for (auto& data : GetDamageDetectionData())
        {
            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }
}

void Player::UpdateAttackState(const Player::STATE& state)
{
    // 先行入力受付
// TODO:先行入力の受付の制限を設ける。
    if (GetLightAttackKeyDown()) SetNextInput(Player::NextInput::LightAttack);
    if (GetStrongAttackKeyDown()) SetNextInput(Player::NextInput::StrongAttack);

    // アニメーション再生中
    if (IsPlayAnimation()) return;

    // 先行入力があった。
    if (GetNextInput())
    {
        switch (state)
        {
        case Player::STATE::LightAttack0:
        case Player::STATE::LightAttack1:
        case Player::STATE::LightAttack2:
            // Xボタンが押された場合
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(state);
            }
            // Yボタンが押された場合
            else
            {
                // TODO: アニメーションが来るまで重攻撃は放置
                break;
                ChangeState(Player::STATE::StrongAttack0);
            }
            break;
            // Y
        case Player::STATE::StrongAttack0:
        case Player::STATE::StrongAttack1:
            // TODO: アニメーションが来るまで重攻撃は放置
            break;

            // Xボタンが押された場合
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(Player::STATE::LightAttack0);
            }
            // Yボタンが押された場合
            else
            {
                ChangeState(state);
            }
            break;
        default:
            // Xボタンが押された場合
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(Player::STATE::LightAttack0);
            }
            // それ以外
            else
            {
                ChangeState(state);
            }
            break;
        }

        return;
    }

    // 先行入力がなかった。( 待機ステートへ遷移 )
    ChangeState(STATE::Idle);
    return;
}

void Player::ResetFlags()
{
    SetNextInput(Player::NextInput::None);  // 先行入力管理フラグ
    SetIsAvoidance(false);                  // 回避入力判定用フラグ
}

void Player::PlayBlendAnimation(const Animation& index1, const Animation& index2, const bool& loop, const float& speed)
{
    Object::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed);
    swordModel_.PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed);
}

void Player::PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed)
{
    const int currentAnimationIndex = GetCurrentBlendAnimationIndex();
    
    Object::PlayBlendAnimation(static_cast<int>(index), loop, speed);
    swordModel_.PlayBlendAnimation(static_cast<int>(index), loop, speed);
}

void Player::UpdateCollisions(const float& elapsedTime, const float& scaleFactor)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(swordModel_.GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
}

void Player::SetWeight(const float& weight)
{
    Object::SetWeight(weight);
    swordModel_.SetWeight(weight);
}

void Player::AddWeight(const float& weight)
{
    Object::AddWeight(weight);

    // weightは0~1の間に収める
    float w = swordModel_.GetWeight();
    w += weight;
    w = std::clamp(w, 0.0f, 1.0f);
    swordModel_.SetWeight(w);
}
