#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

// ----- コンストラクタ -----
EnemyDragon::EnemyDragon()
    : Enemy("./Resources/Model/Character/Enemy/Dragon.gltf", 1.0f),
    circle_("./Resources/Model/Circle/circle.gltf", 5.0f)
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
    //GetTransform()->SetPositionZ(-20);
    GetTransform()->SetPositionZ(-10);
    //GetTransform()->SetPositionZ(10);

    // サイズを設定
    GetTransform()->SetScaleFactor(1.5f);

    // ステージとの判定offset設定
    SetCollisionRadius(2.5f);

    // 回転速度設定
    SetRotateSpeed(5.0f);

    // 歩行速度設定
    SetWalkSpeed(3.0f);

    // 体力設定
    //SetMaxHealth(30.0f);
    //SetMaxHealth(100.0f);
    //SetMaxHealth(0.0f);
    SetMaxHealth(600.0f);
    //SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());

    PlayAnimation(Enemy::DragonAnimation::Idle0, true);


    // 攻撃判定 無効化
    SetTurnAttackActiveFlag(false);
    SetTackleAttackActiveFlag(false);
    SetFlyAttackActiveFlag(false);
    SetComboSlamAttackActiveFlag(false);

    // 押し出し判定
    SetDownCollisionActiveFlag(false);
}

// ----- 終了化 -----
void EnemyDragon::Finalize()
{
}

// ----- 更新 -----
void EnemyDragon::Update(const float& elapsedTime)
{
    if(isUpdateAnimation_) Character::Update(elapsedTime);

    // behaviorTree更新
    UpdateNode(elapsedTime);

    // RootMotion
    if(GetUseRootMotion()) RootMotion();

    // Collisionデータ更新
    UpdateCollisions(elapsedTime);


    // ステージの外に出ないようにする
    if(GetIsStageCollisionJudgement() == false) CollisionCharacterVsStage();

    DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();
    pos.y = 0.01f;
    circle_.GetTransform()->SetPosition(pos);
}

// ----- 描画 -----
void EnemyDragon::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);

    if(GetIsStageCollisionJudgement()) circle_.Render(psShader);
}

// ----- ImGui用 -----
void EnemyDragon::DrawDebug()
{
    if (ImGui::BeginMenu("Dragon"))
    {
        if (ImGui::Button("Roar")) SetIsRoar(false);

        ImGui::Checkbox("useAnimation", &isUpdateAnimation_);
        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

        if (ImGui::TreeNode("Judgment"))
        {
            ImGui::DragFloat("NearAttackRadius", &nearAttackRadius_);
            ImGui::DragFloat("ComboFlyAttackRadius", &comboFlyAttackRadius_);

            ImGui::TreePop();
        }

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
    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), nearAttackRadius_, 1.0f, { 0,1,0,1 });
    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), comboFlyAttackRadius_, 1.0f, { 0,1,1,1 });

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

    for (auto& data : GetFlinchDetectionData())
    {
        // 現在アクティブではないでの表示しない
        if (data.GetIsActive() == false) continue;

        debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), {1,0,1,1});
    }

}

// ----- Behavior登録 -----
void EnemyDragon::RegisterBehaviorNode()
{
    // Behavior Node追加
    behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    // --------------- 死亡 ---------------
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new ActionDragon::DeathAction(this));

    // --------------- 怯み ---------------
    behaviorTree_->AddNode("Root",   "Flinch",       1, BehaviorTree::SelectRule::Priority, new FlinchJudgment(this), nullptr);
    behaviorTree_->AddNode("Flinch", "NormalFlinch", 0, BehaviorTree::SelectRule::None, new NormalFlinchJudgment(this), new ActionDragon::FlinchAction(this));
    behaviorTree_->AddNode("Flinch", "FlyFlinch",    1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FlyFlinchAction(this));

    // --------------- 非戦闘 ---------------
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new ActionDragon::NonBattleIdleAction(this));
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::NonBattleWalkAction(this));

    // --------------- 戦闘 ---------------
    behaviorTree_->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    behaviorTree_->AddNode("Battle", "Shout", 0, BehaviorTree::SelectRule::Priority, new ShoutJudgment(this), nullptr);
#if 0
    behaviorTree_->AddNode("Battle", "Near", 1, BehaviorTree::SelectRule::Random, new NearJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Far", 2, BehaviorTree::SelectRule::Random, nullptr, nullptr);
#else
    behaviorTree_->AddNode("Battle", "Near",  1, BehaviorTree::SelectRule::Priority, new NearJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Far",   2, BehaviorTree::SelectRule::Random, nullptr, nullptr);
    //behaviorTree_->AddNode("Battle", "Far",   2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
#endif

    //behaviorTree_->AddNode("Shout", "Roar", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RoarAction(this));
    behaviorTree_->AddNode("Shout", "RoarLong",         0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RoarLongAction(this));

    
    behaviorTree_->AddNode("Near", "ComboFlySlam",  0, BehaviorTree::SelectRule::None, new ComboFlySlamJudgment(this), new ActionDragon::ComboFlySlamAction(this));    
    

    behaviorTree_->AddNode("Near", "MostNear",    0, BehaviorTree::SelectRule::Random, nullptr, nullptr);

    behaviorTree_->AddNode("MostNear", "TurnAttack",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    behaviorTree_->AddNode("MostNear", "FlyAttack",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FlyAttackAction(this));    
    behaviorTree_->AddNode("MostNear", "ComboSlam",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboSlamAction(this));    
    behaviorTree_->AddNode("MostNear", "KnockBack",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::KnockBackAction(this));
    

    
    //behaviorTree_->AddNode("Near", "BackStep",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::BackStepAction(this));
    //behaviorTree_->AddNode("Near", "Slam",        1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAction(this));
    //behaviorTree_->AddNode("Near", "ComboCharge", 1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboChargeAction(this));

    behaviorTree_->AddNode("Far", "FireBreathCombo",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreathCombo(this));
    behaviorTree_->AddNode("Far", "FireBreath",        0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreath(this));
    behaviorTree_->AddNode("Far", "Tackle",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));
    behaviorTree_->AddNode("Far", "RiseAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RiseAttackAction(this));
    //behaviorTree_->AddNode("Far", "MoveTurn",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveTurnAction(this));
    //behaviorTree_->AddNode("Far", "MoveAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveAttackAction(this));

}

bool EnemyDragon::CheckStatusChange()
{
    // --------------------
    //      死亡判定
    // --------------------
    // HPがなくなった
    if (GetHealth() <= 0.0f)
    {
        // リセット
        SetStep(0); 
        ResetAllAttackActiveFlag();
        return true;
    }

    // --------------------
    //      怯み判定
    // --------------------
    // 既に怯み中
    if (GetIsFlinch()) return false;
    // 初回時などに通る
    if (oldHealth_ <= 0.0f)
    {
        oldHealth_ = GetHealth();
        return false;
    }
    const float damage = oldHealth_ - GetHealth();
    oldHealth_ = GetHealth();

    // damageが 30 より大きければ怯み
    //if (damage > 30.0f)
    if (damage > 100.0f)
    {
        SetIsFlinch(true);
        // リセット
        SetStep(0); 
        ResetAllAttackActiveFlag();
        return true;
    }

    return false;
}

// ----- CollisionData登録 -----
void EnemyDragon::RegisterCollisionData()
{
    // 押し出し判定登録
    CollisionDetectionData collisionDetectionData[] =
    {
#pragma region ダウン時の押し出し判定
        // ----- ダウン時 -----
        { "Down_Dragon15_head",      0.6f,  true, {},                    "Dragon15_head"   },                          // 0
        { "Down_Dragon15_neck_2",    1.0f,  true, { -0.4f, 0.0f, 0.0f }, "Dragon15_neck_2" },
        { "Down_Dragon15_neck_1",    1.0f,  true, { -0.4f, 0.0f, 0.0f }, "Dragon15_neck_1" },
        { "Down_Dragon15_spine2",    1.05f, true, {},                    "Dragon15_spine2" },
        { "Down_Dragon15_spine0",    0.85f, true, {},                    "Dragon15_spine0" },
        
        { "Down_Dragon15_l_upperarm1",   0.7f,  false, { 0.2f, -0.15f, 0.0f }, "Dragon15_l_upperarm1" },
        { "Down_Dragon15_r_upperarm1",   0.7f,  false, { 0.2f, 0.15f, 0.0f }, "Dragon15_r_upperarm1"},
        { "Down_Dragon15_l_forearm",     0.55f, false, {}, "Dragon15_l_forearm"},
        { "Down_Dragon15_r_forearm",     0.55f, false, {}, "Dragon15_r_forearm" },

        { "Down_Dragon15_l_calf",        0.55f, true, { -0.1f, 0.5f, 0.0f },    "Dragon15_l_calf" },
        { "Down_Dragon15_r_calf",        0.55f, true, { -0.1f, -0.5f, 0.0f },   "Dragon15_r_calf" },
        { "Down_Dragon15_l_horselink",   0.55f,  true, {},                       "Dragon15_l_horselink" },
        { "Down_Dragon15_r_horselink",   0.55f,  true, {},                       "Dragon15_r_horselink" },
        { "Down_Dragon15_l_foot",        0.35f, true, { 0.05f, -1.0f, 0.1f },   "Dragon15_l_foot" },
        { "Down_Dragon15_r_foot",        0.35f, true, { 0.1f, 0.0f, -0.1f },    "Dragon15_r_foot" },

        #pragma region ---------- 尻尾 ----------
        { "Dragon15_tail_00", 0.70f, true, {} },
        { "Dragon15_tail_01", 0.55f, true, {  0.20f, 0.0f,  0.00f } },
        { "Dragon15_tail_02", 0.42f, true, {  0.01f, 0.0f,  0.03f } },
        { "Dragon15_tail_03", 0.35f, true, { -0.10f, 0.0f, -0.05f } },
        { "Dragon15_tail_04", 0.25f, true,  {} },
        { "Dragon15_tail_05", 0.20f, true,  {} },

        { "Dragon15_tail_add_0", 0.30f, true, {  0.40f, 0.0f,  0.00f }, "Dragon15_tail_03" },
        { "Dragon15_tail_add_1", 0.20f, true, {  0.40f, 0.0f,  0.03f }, "Dragon15_tail_04" },
        { "Dragon15_tail_add_2", 0.2f,  true, {  0.30f, 0.0f, -0.05f }, "Dragon15_tail_05" },
        { "Dragon15_tail_add_3", 0.2f,  true, {  0.60f, 0.0f, -0.13f }, "Dragon15_tail_05" },
#pragma endregion ---------- 尻尾 ----------
#pragma endregion ダウン時の押し出し判定

        { "Dragon15_head", 0.4f, false, {} }, // 頭

        { "Dragon15_neck_1",    1.0f,  false, { -0.4f, 0.0f, 0.0f } },
        { "Dragon15_spine2",    1.05f, false, {} },
        { "Dragon15_spine0",    0.85f, false, {} },
        { "root",    0.85f, false, {} },

        { "Dragon15_r_hand",    0.3f, false, {} },
        { "Dragon15_r_forearm", 0.32, false, {} },

        { "Dragon15_l_hand",    0.3f, false, {} },
        { "Dragon15_l_forearm", 0.32f, false,  {} },

        { "Dragon15_r_foot",        0.32f, false, {} },
        { "Dragon15_r_horselink",   0.3f,  false, { 0.06f, 0.01f, -0.09f } },
        { "Dragon15_r_calf",        0.35f, false, { 0.03f, 0.0f, 0.15f } },

        { "Dragon15_l_foot",        0.32f, false, {} },
        { "Dragon15_l_horselink",   0.3f,  false, { 0.06f, 0.01f, -0.09f } },
        { "Dragon15_l_calf",        0.35f, false, { 0.03f, 0.0f, 0.15f } },

        // ---------- 尻尾 ----------
#pragma region ---------- 尻尾 ----------
        { "Dragon15_tail_00", 0.70f, false, {} },
        { "Dragon15_tail_01", 0.55f, false, {  0.20f, 0.0f,  0.00f } },
        { "Dragon15_tail_02", 0.42f, false, {  0.01f, 0.0f,  0.03f } },
        { "Dragon15_tail_03", 0.35f, false, { -0.10f, 0.0f, -0.05f } },
        { "Dragon15_tail_04", 0.25f, false,  {} },
        { "Dragon15_tail_05", 0.20f, false,  {} },
        
        { "Dragon15_tail_add_0", 0.30f, false, {  0.40f, 0.0f,  0.00f }, "Dragon15_tail_03" },
        { "Dragon15_tail_add_1", 0.20f, false, {  0.40f, 0.0f,  0.03f }, "Dragon15_tail_04" },
        { "Dragon15_tail_add_2", 0.2f,  false, {  0.30f, 0.0f, -0.05f }, "Dragon15_tail_05" },
        { "Dragon15_tail_add_3", 0.2f,  false, {  0.60f, 0.0f, -0.13f }, "Dragon15_tail_05" },
#pragma endregion ---------- 尻尾 ----------
    };
    for (int i = 0; i < _countof(collisionDetectionData); ++i)
    {
        RegisterCollisionDetectionData(collisionDetectionData[i]);
    }

    // くらい判定登録
    DamageDetectionData damageDetectionData[] =
    {
        // { name, radius, damage, offset, updateName }

        { "Dragon15_head",      1.2f, 65.0f, {} }, // 頭
        { "Dragon15_neck_1",    1.4f, 27.0f, {} }, // 胴体
        { "Dragon15_neck_2",    1.1f, 35.0f, {} }, // 首
        { "Dragon15_spine1",    1.3f, 25.0f, {} }, // おなか


        { "Dragon15_r_hand",    1.0f, 35.0f, {} },
        { "Dragon15_r_forearm", 1.0f, 35.0f, {} },
                                      
        { "Dragon15_l_hand",    1.0f, 35.0f, {} },
        { "Dragon15_l_forearm", 1.0f, 35.0f, {} },

        // ---------- 足 ----------
#pragma region ---------- 足 ----------
        { "Dragon15_r_thigh",     0.65f, 35.0f, { 0.15f, 0.0f, 0.0f } },
        { "Dragon15_r_calf",      0.6f,  35.0f, { 0.0f, 0.0f, 0.2f } },
        { "Dragon15_r_horselink", 0.5f,  35.0f, {} },
        { "Dragon15_r_foot",      0.45f, 35.0f, {} },
        { "Dragon15_r_toe11",     0.4f,  35.0f, { 0.06f, 0.0f, 0.0f } },

        { "Dragon15_l_thigh",     0.65f, 35.0f, { 0.15f, 0.0f, 0.0f } },
        { "Dragon15_l_calf",      0.6f,  35.0f, { 0.0f, 0.0f, 0.2f } },
        { "Dragon15_l_horselink", 0.5f,  35.0f, {} },
        { "Dragon15_l_foot",      0.45f, 35.0f, {} },
        { "Dragon15_l_toe11",     0.4f,  35.0f, { 0.06f, 0.0f, 0.0f } },
#pragma endregion ---------- 足 ----------

        // ---------- 尻尾 ----------
#pragma region ---------- 尻尾 ----------
        { "Dragon15_tail_00", 1.10f, 45.0f, {} },
        { "Dragon15_tail_01", 1.00f, 45.0f, { 0.30f, 0.0f, 0.0f } },
        { "Dragon15_tail_03", 0.90f, 45.0f, {} },
        { "Dragon15_tail_04", 0.80f, 45.0f, { 0.07f, 0.0f, 0.0f } },
        { "Dragon15_tail_05", 0.75f, 45.0f, { 0.30f, 0.0f, 0.0f } },
#pragma endregion ---------- 尻尾 ----------

        // ---------- 翼 ----------
#pragma region ---------- 翼 ----------
        { "Dragon15_l_wing_01", 1.0f,  50.0f, {} },
        { "Dragon15_l_wing_03", 1.3f,  50.0f, { 0.2f, 0.0f, 0.0f } },
        { "Dragon15_l_wing_04", 1.2f,  50.0f, {} },
        { "Dragon15_l_wing_06", 1.0f,  50.0f, {} },
        { "Dragon15_l_wing_07", 1.25f, 50.0f, {} },
        { "Dragon15_l_wing_08", 1.25f, 50.0f, { 0.0f, -0.3f, 0.0f } },
        { "Dragon15_l_wing_09", 1.0f,  50.0f, {} },
        { "Dragon15_l_wing_10", 1.0f,  50.0f, {} },
        { "Dragon15_l_wing_12", 1.0f,  50.0f, {} },

        { "Dragon15_l_wing_add_0", 1.1f, 50.0f, { 1.6f, 0.0f, 0.0f } , "Dragon15_l_wing_04" },
        { "Dragon15_l_wing_add_1", 1.0f, 50.0f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_l_wing_07" },
        { "Dragon15_l_wing_add_2", 1.0f, 50.0f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_l_wing_10" },
        { "Dragon15_l_wing_add_3", 1.0f, 50.0f, { 1.25f, 0.0f, 0.0f } , "Dragon15_l_wing_12" },

        { "Dragon15_r_wing_01", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_03", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_04", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_06", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_07", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_08", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_09", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_10", 1.0f, 50.0f, {} },
        { "Dragon15_r_wing_12", 1.0f, 50.0f, {} },

        { "Dragon15_r_wing_add_0", 1.1f, 50.0f, { 1.6f, 0.0f, 0.0f }  , "Dragon15_r_wing_04" },
        { "Dragon15_r_wing_add_1", 1.0f, 50.0f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_r_wing_07" },
        { "Dragon15_r_wing_add_2", 1.0f, 50.0f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_r_wing_10" },
        { "Dragon15_r_wing_add_3", 1.0f, 50.0f, { 1.25f, 0.0f, 0.0f } , "Dragon15_r_wing_12" },

#pragma endregion ---------- 翼 ----------

    };
    for (int i = 0; i < _countof(damageDetectionData); ++i)
    {
        RegisterDamageDetectionData(damageDetectionData[i]);
    }

    // 攻撃判定登録
    AttackDetectionData attackDetectionData[] =
    {
        // ----- 回転攻撃用 -----
        { "TurnAttack_0", 1.0f, {}, "Dragon15_tail_00" }, // 0
        { "TurnAttack_1", 1.0f, {}, "Dragon15_tail_01" }, // 
        { "TurnAttack_2", 1.0f, {}, "Dragon15_tail_02" }, // 
        { "TurnAttack_3", 1.0f, {}, "Dragon15_tail_03" }, // 
        { "TurnAttack_4", 1.0f, {}, "Dragon15_tail_04" }, // 
        { "TurnAttack_5", 1.0f, {}, "Dragon15_tail_05" }, // 5

        // ----- 突進攻撃用 -----
        { "TackleAttack_0", 1.0f, {}, "Dragon15_neck_3" },  // 6
        { "TackleAttack_1", 1.0f, {}, "Dragon15_neck_1" },  // 
        { "TackleAttack_2", 1.0f, {}, "Dragon15_spine2" },  // 
        { "TackleAttack_3", 1.0f, {}, "Dragon15_spine0" },  // 
        { "TackleAttack_4", 1.0f, {}, "Dragon15_tail_00" }, // 10

        // ----- 空中からたたきつけ攻撃 -----
        { "FlyAttack_0", 1.0f, {}, "Dragon15_r_hand" },    // 11
        { "FlyAttack_1", 1.0f, {}, "Dragon15_l_hand" },    // 12

        // ----- コンボたたきつけ攻撃 -----
        { "ComboSlam_0", 0.8f, {}, "Dragon15_r_hand" },     // 13
        { "ComboSlam_1", 0.8f, {}, "Dragon15_r_forearm" },  // 
        { "ComboSlam_2", 0.7f, {}, "Dragon15_r_finger21" }, // 15

    };
    for (int i = 0; i < _countof(attackDetectionData); ++i)
    {
        RegisterAttackDetectionData(attackDetectionData[i]);
    }

    // 怯み判定登録
    AttackDetectionData flinchDetectionData[] =
    {
        //// ----- 回転攻撃用 -----
        //{ "TurnAttack_0", 1.0f, {}, "Dragon15_tail_00" }, // 0
        //{ "TurnAttack_1", 1.0f, {}, "Dragon15_tail_01" }, // 
        //{ "TurnAttack_2", 1.0f, {}, "Dragon15_tail_02" }, // 
        //{ "TurnAttack_3", 1.0f, {}, "Dragon15_tail_03" }, // 
        //{ "TurnAttack_4", 1.0f, {}, "Dragon15_tail_04" }, // 
        //{ "TurnAttack_5", 1.0f, {}, "Dragon15_tail_05" }, // 5

        //// ----- 突進攻撃用 -----
        //{ "TackleAttack_0", 1.0f, {}, "Dragon15_neck_3" },  // 6
        //{ "TackleAttack_1", 1.0f, {}, "Dragon15_neck_1" },  // 
        //{ "TackleAttack_2", 1.0f, {}, "Dragon15_spine2" },  // 
        //{ "TackleAttack_3", 1.0f, {}, "Dragon15_spine0" },  // 
        //{ "TackleAttack_4", 1.0f, {}, "Dragon15_tail_00" }, // 10

        //// ----- 空中からたたきつけ攻撃 -----
        //{ "FlyAttack_0", 1.0f, {}, "Dragon15_r_hand" },    // 11
        //{ "FlyAttack_1", 1.0f, {}, "Dragon15_l_hand" },    // 12

        // ----- コンボたたきつけ攻撃 -----
        { "ComboSlam_0", 1.0f, {}, "Dragon15_r_hand" },     // 13
        { "ComboSlam_1", 1.0f, {}, "Dragon15_r_forearm" },  // 
        { "ComboSlam_2", 1.0f, {}, "Dragon15_r_finger21" }, // 15
    };
    for (int i = 0; i < _countof(flinchDetectionData); ++i)
    {
        flinchDetectionData_.emplace_back(flinchDetectionData[i]);
    }
}

void EnemyDragon::UpdateCollisions(const float& elapsedTime)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }

    for (int i = AttackData::TrunAttackStart; i <= AttackData::TackleAttackEnd; ++i)
    //for (int i = AttackData::TrunAttackStart; i < AttackData::TrunAttackEnd; ++i)
    {
        AttackDetectionData& data = GetAttackDetectionData(i);
        DirectX::XMFLOAT3 pos = data.GetPosition();
        pos.y = 1.0f;
        data.SetJointPosition(pos);
    }

    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition());
        
        if(data.GetFixedY()) 
            pos.y = 0.0f;

        data.SetJointPosition(pos);
    }


    for (AttackDetectionData& data : flinchDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
}


#pragma region ----- 攻撃判定 -----
// ----- 全攻撃判定無効化 -----
void EnemyDragon::ResetAllAttackActiveFlag()
{
    // 攻撃判定フラグをセットする
    SetIsAttackActive(false);

    for (AttackDetectionData& data : attackDetectionData_)
    {
        data.SetIsActive(false);
    }
}

// ----- 回転攻撃判定設定 -----
void EnemyDragon::SetTurnAttackActiveFlag(const bool& flag)
{
    // 攻撃判定フラグをセットする
    SetIsAttackActive(flag);

    for (int i = AttackData::TrunAttackStart; i <= AttackData::TrunAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- 突進攻撃判定設定 -----
void EnemyDragon::SetTackleAttackActiveFlag(const bool& flag)
{
    // 攻撃判定フラグをセットする
    SetIsAttackActive(flag);

    for (int i = AttackData::TackleAttackStart; i <= AttackData::TackleAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- 上昇攻撃判定設定 -----
void EnemyDragon::SetFlyAttackActiveFlag(const bool& flag)
{
    // 攻撃判定フラグをセットする
    SetIsAttackActive(flag);

    for (int i = AttackData::FlyAttackStart; i <= AttackData::FlyAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- コンボたたきつけ攻撃判定設定 -----
void EnemyDragon::SetComboSlamAttackActiveFlag(const bool& flag)
{
    // 攻撃判定フラグをセットする
    SetIsAttackActive(flag);

    for (int i = AttackData::ComboSlamAttackStart; i <= AttackData::ComboSlamAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}
#pragma endregion ----- 攻撃判定 -----

// ----- 押し出し判定 -----
void EnemyDragon::SetDownCollisionActiveFlag(const bool& flag)
{
    // 全てのフラグをリセットする
    const bool resetFlag = !flag;
    //for (auto& data : GetCollisionDetectionData())
    for (int i = 0; i < collisionDetectionData_.size(); ++i)
    {
        GetCollisionDetectionData(i).SetIsActive(resetFlag);
    }
    // ダウン時の押し出し判定設定
    for (int i = CollisionData::DownStart; i < CollisionData::DownEnd; ++i)
    {
        GetCollisionDetectionData(i).SetIsActive(flag);
    }
}