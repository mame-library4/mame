#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

// ----- コンストラクタ -----
EnemyDragon::EnemyDragon()
    : Enemy("./Resources/Model/Character/Enemy/Dragon.gltf"),
    scaleFacter_(1.0f)
{
    // BehaviorTree設定
    behaviorData_ = std::make_unique<BehaviorData>();       // BehaviorData生成
    behaviorTree_ = std::make_unique<BehaviorTree>(this);   // BehaviorTree生成
    RegisterBehaviorNode();

    // CollisionData登録
    RegisterCollisionData();
}

// ----- 初期化 -----
void EnemyDragon::Initialize()
{
    // 位置設定
    GetTransform()->SetPositionZ(10);

    // サイズを設定
    GetTransform()->SetScaleFactor(1.5f);

    // ステージとの判定offset設定
    SetCollisionRadius(2.5f);

    // 回転速度設定
    SetRotateSpeed(5.0f);

    // 歩行速度設定
    SetWalkSpeed(3.0f);

    // 体力設定
    SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());
}

// ----- 終了化 -----
void EnemyDragon::Finalize()
{
}

// ----- 更新 -----
void EnemyDragon::Update(const float& elapsedTime)
{
    Character::Update(elapsedTime);

    // Collisionデータ更新
    UpdateCollisions(elapsedTime, scaleFacter_);

    // behaviorTree更新
    UpdateNode(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();
}

// ----- 描画 -----
void EnemyDragon::Render(ID3D11PixelShader* psShader)
{
    Object::Render(scaleFacter_, psShader);
}

// ----- ImGui用 -----
void EnemyDragon::DrawDebug()
{
    if (ImGui::BeginMenu("Dragon"))
    {
        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

        Character::DrawDebug();
        Object::DrawDebug();

        std::string nodeName = (activeNode_ != nullptr) ? activeNode_->GetName() : u8"なし";
        ImGui::Text(u8"Behavior:%s", nodeName.c_str());

        ImGui::EndMenu();
    }
}

// ----- DebugRebderer -----
void EnemyDragon::DebugRender(DebugRenderer* debugRenderer)
{
    if (isCollisionSphere_)
    {
        for (auto& data : GetCollisionDetectionData())
        {
            // 現在アクティブではないので表示しない
            if (data.GetIsActive() == false) continue;

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
    if (isAttackSphere_)
    {
        for (auto& data : GetAttackDetectionData())
        {
            // 現在アクティブではないでの表示しない
            if (data.GetIsActive() == false) continue;

            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }
}

// ----- Behavior登録 -----
void EnemyDragon::RegisterBehaviorNode()
{
    // Behavior Node追加
    behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    // --------------- 死亡 ---------------
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new DeathAction(this));

    // --------------- 怯み ---------------
    behaviorTree_->AddNode("Root", "Flinch", 1, BehaviorTree::SelectRule::None, new FlinchJudgment(this), new FlinchAction(this));

    // --------------- 非戦闘 ---------------
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new NonBattleIdleAction(this));
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, nullptr, new NonBattleWalkAction(this));

    // --------------- 戦闘 ---------------
    behaviorTree_->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    behaviorTree_->AddNode("Battle", "Shout", 0, BehaviorTree::SelectRule::Priority, new ShoutJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Near",  1, BehaviorTree::SelectRule::Priority, new NearJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Far",   2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    behaviorTree_->AddNode("Shout", "Roar",         0, BehaviorTree::SelectRule::None, nullptr, new RoarAction(this));
    behaviorTree_->AddNode("Shout", "BackStepRoar", 0, BehaviorTree::SelectRule::None, nullptr, new BackStepRoarAction(this));
    behaviorTree_->AddNode("Shout", "MoveRoar",     0, BehaviorTree::SelectRule::None, nullptr, new MoveRoarAction(this));

    behaviorTree_->AddNode("Near", "BackStep",    0, BehaviorTree::SelectRule::None, nullptr, new BackStepAction(this));
    behaviorTree_->AddNode("Near", "FlyAttack",   0, BehaviorTree::SelectRule::None, nullptr, new FlyAttackAction(this));
    behaviorTree_->AddNode("Near", "KnockBack",   0, BehaviorTree::SelectRule::None, nullptr, new KnockBackAction(this));
    behaviorTree_->AddNode("Near", "Slam",        0, BehaviorTree::SelectRule::None, nullptr, new SlamAction(this));
    behaviorTree_->AddNode("Near", "FrontAttack", 0, BehaviorTree::SelectRule::None, nullptr, new FrontAttackAction(this));
    behaviorTree_->AddNode("Near", "ComboSlam",   0, BehaviorTree::SelectRule::None, nullptr, new ComboSlamAction(this));
    behaviorTree_->AddNode("Near", "ComboCharge", 0, BehaviorTree::SelectRule::None, nullptr, new ComboChargeAction(this));
    behaviorTree_->AddNode("Near", "TurnAttack",  0, BehaviorTree::SelectRule::None, nullptr, new TurnAttackAction(this));

    behaviorTree_->AddNode("Far", "Tackle",     0, BehaviorTree::SelectRule::None, nullptr, new TackleAction(this));
    behaviorTree_->AddNode("Far", "RiseAttack", 0, BehaviorTree::SelectRule::None, nullptr, new RiseAttackAction(this));
    behaviorTree_->AddNode("Far", "MoveTurn",   0, BehaviorTree::SelectRule::None, nullptr, new MoveTurnAction(this));
    behaviorTree_->AddNode("Far", "MoveAttack", 0, BehaviorTree::SelectRule::None, nullptr, new MoveAttackAction(this));
    

}

// ----- CollisionData登録 -----
void EnemyDragon::RegisterCollisionData()
{
    // 押し出し判定登録
    CollisionDetectionData collisionDetectionData[] =
    {
        {},
    };

    // くらい判定登録
    DamageDetectionData damageDetectionData[] =
    {
        {},
    };

    // 攻撃判定登録
    AttackDetectionData attackDetectionData[] =
    {
        {},
    };
}
