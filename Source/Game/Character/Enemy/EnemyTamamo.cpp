#include "EnemyTamamo.h"
#include "JudgmentTamamo.h"
#include "ActionTamamo.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
EnemyTamamo::EnemyTamamo()
    : Enemy("./Resources/Model/Character/Fox.glb")
{
    // BehaviorTree設定
#pragma region BehaviorTree設定
    behaviorData_ = std::make_unique<BehaviorData>();       // BehaviorData生成
    behaviorTree_ = std::make_unique<BehaviorTree>(this);   // BehaviorTree生成

    // Behavior Node追加
    behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    // --------------- 死亡 ---------------
#pragma region 死亡
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new DeathAction(this));

#pragma endregion 死亡

    // --------------- 怯み ---------------
#pragma region ひるみ
    behaviorTree_->AddNode("Root", "Flinch", 1, BehaviorTree::SelectRule::None, new FlinchJudgment(this), new FlinchAction(this));

#pragma endregion ひるみ

    // --------------- 非戦闘 ---------------
#pragma region 非戦闘
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new NonBattleIdleAction(this)); // 非戦闘時待機
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleWalkJudgment(this), new NonBattleWalkAction(this)); // 非戦闘時歩き

#pragma endregion 非戦闘

    // --------------- 戦闘 ---------------
#pragma region 戦闘
    behaviorTree_->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, nullptr, nullptr); // 戦闘 (中間ノード)

    // ---------- 待機系 ----------
    behaviorTree_->AddNode("Battle", "Idle", 1, BehaviorTree::SelectRule::Priority, nullptr, nullptr); // 待機 (中間ノード)
    
    behaviorTree_->AddNode("Idle", "Walk", 0, BehaviorTree::SelectRule::None, new WalkJudgment(this), new WalkAction(this)); // 歩き
    behaviorTree_->AddNode("Idle", "Step", 1, BehaviorTree::SelectRule::None, nullptr, new StepAction(this)); // ステップ

    // ---------- 攻撃系 ----------
    behaviorTree_->AddNode("Battle", "Attack", 0, BehaviorTree::SelectRule::Priority, new AttackJudgment(this), nullptr); // 攻撃 (中間ノード)
    
    // ----- 距離近い -----
    behaviorTree_->AddNode("Attack", "Near", 1, BehaviorTree::SelectRule::Random, new NearAttackJudgment(this), nullptr); // 近距離 (中間ノード)
    
    behaviorTree_->AddNode("Near", "Bite",   0, BehaviorTree::SelectRule::None, nullptr, new BiteAction(this)); // 嚙みつき
    behaviorTree_->AddNode("Near", "Slash",  0, BehaviorTree::SelectRule::None, nullptr, new SlashAction(this)); // ひっかき
    behaviorTree_->AddNode("Near", "Tail",   0, BehaviorTree::SelectRule::None, nullptr, new TailSwipeAction(this)); // 尻尾
    // behaviorTree_->AddNode("Near", "Spin",   1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 回転
    // behaviorTree_->AddNode("Near", "Pounce", 1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 飛びつき
    
    // ----- 距離遠い -----
    behaviorTree_->AddNode("Attack", "Far", 1, BehaviorTree::SelectRule::Random, new FarAttackJudgment(this), nullptr); // 遠距離 (中間ノード)
    
    behaviorTree_->AddNode("Far", "Spine",  0, BehaviorTree::SelectRule::None, nullptr, nullptr); // 棘
    behaviorTree_->AddNode("Far", "Tackle", 0, BehaviorTree::SelectRule::None, nullptr, nullptr); // 突進
    behaviorTree_->AddNode("Far", "Pounce", 0, BehaviorTree::SelectRule::None, nullptr, nullptr); // 飛びつき

    // --- 叫ぶ系 -----
    behaviorTree_->AddNode("Attack", "Shout", 0, BehaviorTree::SelectRule::Priority, new ShoutJudgment(this), nullptr); // 叫ぶ系判定 (中間ノード)

    behaviorTree_->AddNode("Shout", "Roar",       0, BehaviorTree::SelectRule::None, new RoarJudgment(this), new RoarAction(this)); // 咆哮
    behaviorTree_->AddNode("Shout", "Intimidate", 1, BehaviorTree::SelectRule::None, new IntimidateJudgment(this), new IntimidateAction(this)); // 威嚇


#pragma endregion 戦闘

#pragma endregion BehaviorTree設定
}

// ----- デストラクタ -----
EnemyTamamo::~EnemyTamamo()
{
}

// ----- 初期化 -----
void EnemyTamamo::Initialize()
{
    GetTransform()->SetPositionZ(25);

    CollisionCylinderData data[] =
    {
        { "R:C_Spine_2", 0.7f, 1.5f, { 0, -1, 0 } }, // おなか
        { "R:C_Neck_1", 0.4f, 1.5f, { 0, -1, 0 } }, // くび前
        { "R:C_Spine_5", 0.4f, 1.5f, { 0, -1, 0 } }, // くび後ろ
        { "R:C_Tail_1", 0.4f, 1.5f, { 0, -1.5, 0 } }, // おしり
        { "R:R_Arm_2", 0.3f, 1.5f, { 0, -1, 0 } },  // 右前足
        { "R:L_Arm_2", 0.3f, 1.5f, { 0, -1, 0 } },  // 左前足
        { "R:R_Leg_2", 0.3f, 1.5f, { 0, -1, 0 } },  // 右後足
        { "R:L_Leg_2", 0.3f, 1.5f, { 0, -1, 0 } },  // 左後足
    };

    for (int i = 0; i < _countof(data); ++i)
    {
        RegisterCollisionCylinderData(data[i]);
    }

}

// ----- 終了化 -----
void EnemyTamamo::Finalize()
{
}

// ----- 更新処理 -----
void EnemyTamamo::Update(const float& elapsedTime)
{
    // 円柱判定用データ更新
    UpdateCollisionCylinderData(0.01f);

    // behaviorTree更新
    UpdateNode(elapsedTime);

    // アニメーション更新
    Object::Update(elapsedTime);
}

// ----- 描画 -----
void EnemyTamamo::Render()
{
    Object::Render(0.01f);
}

// ----- ImGui用 -----
void EnemyTamamo::DrawDebug()
{
    if (ImGui::TreeNode("Tamamo"))
    {
        Character::DrawDebug();

        if (ImGui::BeginMenu("Battle,Attack,Radius"))
        {
            ImGui::DragFloat("BattleRadius", &battleRadius_);
            ImGui::DragFloat("NearAttackRadius", &nearAttackRadius_);
            ImGui::DragFloat("FarAttackRadius", &farAttackRadius_);
            ImGui::EndMenu();
        }     

        std::string str = (activeNode_ != nullptr) ? activeNode_->GetName() : u8"なし";
        ImGui::Text(u8"Behavior：%s", str.c_str());
        if (ImGui::Button("addAttackCount")) AddAttackComboCount();
        ImGui::DragInt("attackComboCount", &attackComboCount_);

        Object::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- デバッグ用 -----
void EnemyTamamo::DebugRender(DebugRenderer* debugRenderer)
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition();

    for (auto& data : GetCollisionCylinderData())
    {
        debugRenderer->DrawCylinder(data.GetPosition(), data.GetRadius(), data.GetHeight(), data.GetColor());
    }

    // 戦闘範囲
    debugRenderer->DrawCylinder(position, battleRadius_, 0.5f, { 1,1,1,1 });

    // 攻撃範囲
    debugRenderer->DrawCylinder(position, nearAttackRadius_, 0.5f, { 1,0,0,1 });
    debugRenderer->DrawCylinder(position, farAttackRadius_, 0.5f, { 1,0,1,1 });

    debugRenderer->DrawSphere(tailPosition, 2, { 0,0,1,1 });

}