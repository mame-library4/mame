#include "Player.h"
#include "PlayerState.h"
#include "../../../Graphics/Graphics.h"

// ----- コンストラクタ -----
Player::Player()
    : Character("./Resources/Model/Character/Player.glb")
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
    RegisterCollisionDetectionData({ "collide", 0.25f });

    // 当たり判定用設定
    RegisterDamageDetectionData({ "head", 0.15f, 10, { 0, 1.4f, 0 } });
    RegisterDamageDetectionData({ "body", 0.25f, 10, { 0, 1.0f, 0 } });
    RegisterDamageDetectionData({ "foot", 0.25f, 10, { 0, 0.5f, 0 } });

    // 体力設定
    SetMaxHealth(100.0f);
    SetHealth(GetMaxHealth());

    Object::PlayAnimation(animationIndex_, true, speed_);
}

void Player::Finalize()
{
}

// ----- 更新 -----
void Player::Update(const float& elapsedTime)
{
    GetCollisionDetectionData("collide").SetJointPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("head").SetPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("body").SetPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("foot").SetPosition(GetTransform()->GetPosition());

    Character::Update(elapsedTime);

    Camera::Instance().SetTarget(GetTransform()->GetPosition() + offset_);

    GetStateMachine()->Update(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();

    //GetTransform()->SetPositionY(0.0f);
}

// ----- 描画 -----
void Player::Render()
{
    Object::Render(0.01f);
}

// ----- ImGui用 -----
void Player::DrawDebug()
{
    if (ImGui::TreeNode("Player"))
    {
        ImGui::DragFloat3("offset", &offset_.x);

        ImGui::Checkbox("Collision", &isCollisionSphere_);
        ImGui::Checkbox("Damage", &isDamageSphere_);

        Character::DrawDebug();
        Object::DrawDebug();

        ImGui::DragFloat("speed", &speed_);
        ImGui::DragInt("animationIndex_", &animationIndex_);
        if (ImGui::Button("BT"))
        {
            Object::PlayAnimation(animationIndex_, true, speed_);
        }

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
                ChangeState(Player::STATE::StrongAttack0);
            }
            break;
            // Y
        case Player::STATE::StrongAttack0:
        case Player::STATE::StrongAttack1:
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
