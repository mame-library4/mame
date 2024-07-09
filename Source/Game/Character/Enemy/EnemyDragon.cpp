#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

// ----- コンストラクタ -----
EnemyDragon::EnemyDragon()
    : Enemy("./Resources/Model/Character/Enemy/Dragon.gltf", 1.0f)
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

    PlayAnimation(Enemy::DragonAnimation::Idle0, true);



    //SetTurnAttackActiveFlag(false);
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
    if(GetUseRootMotion()) UpdateRootMotion();

    // Collisionデータ更新
    UpdateCollisions(elapsedTime);


    // ステージの外に出ないようにする
    CollisionCharacterVsStage();
}

// ----- 描画 -----
void EnemyDragon::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
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
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new ActionDragon::DeathAction(this));

    // --------------- 怯み ---------------
    behaviorTree_->AddNode("Root", "Flinch", 1, BehaviorTree::SelectRule::None, new FlinchJudgment(this), new ActionDragon::FlinchAction(this));

    // --------------- 非戦闘 ---------------
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new ActionDragon::NonBattleIdleAction(this));
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::NonBattleWalkAction(this));

    // --------------- 戦闘 ---------------
    behaviorTree_->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    behaviorTree_->AddNode("Battle", "Shout", 0, BehaviorTree::SelectRule::Priority, new ShoutJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Near",  1, BehaviorTree::SelectRule::Priority, new NearJudgment(this), nullptr);
    behaviorTree_->AddNode("Battle", "Far",   2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    behaviorTree_->AddNode("Shout", "Roar",         0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RoarAction(this));
    behaviorTree_->AddNode("Shout", "BackStepRoar", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::BackStepRoarAction(this));

    behaviorTree_->AddNode("Near", "FlyAttack",   1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FlyAttackAction(this));
    
    behaviorTree_->AddNode("Near", "TurnAttack",  0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    behaviorTree_->AddNode("Near", "KnockBack",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::KnockBackAction(this));
    
    behaviorTree_->AddNode("Near", "ComboSlam",   2, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboSlamAction(this));

    
    behaviorTree_->AddNode("Near", "BackStep",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::BackStepAction(this));
    behaviorTree_->AddNode("Near", "Slam",        1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAction(this));
    behaviorTree_->AddNode("Near", "FrontAttack", 1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FrontAttackAction(this));
    behaviorTree_->AddNode("Near", "ComboCharge", 1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboChargeAction(this));

    behaviorTree_->AddNode("Far", "Tackle",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));
    behaviorTree_->AddNode("Far", "RiseAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RiseAttackAction(this));
    behaviorTree_->AddNode("Far", "MoveTurn",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveTurnAction(this));
    behaviorTree_->AddNode("Far", "MoveAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveAttackAction(this));
    

}

// ----- CollisionData登録 -----
void EnemyDragon::RegisterCollisionData()
{
    // 押し出し判定登録
    CollisionDetectionData collisionDetectionData[] =
    {
        { "Dragon15_head", 0.4f, {} }, // 頭

        { "Dragon15_neck_1",    1.0f,  { -0.4f, 0.0f, 0.0f } },
        { "Dragon15_spine2",    1.05f, {} },
        { "Dragon15_spine0",    0.85f, {} },
        { "Dragon15_pelvis",    0.85f, {} },

        { "Dragon15_r_hand",    0.3f, {} },
        { "Dragon15_r_forearm", 0.32, {} },

        { "Dragon15_l_hand",    0.3f, {} },
        { "Dragon15_l_forearm", 0.32f, {} },

        { "Dragon15_r_foot",        0.32f, {} },
        { "Dragon15_r_horselink",   0.3f,  { 0.06f, 0.01f, -0.09f } },
        { "Dragon15_r_calf",        0.35f, { 0.03f, 0.0f, 0.15f } },

        { "Dragon15_l_foot",        0.32f, {} },
        { "Dragon15_l_horselink",   0.3f,  { 0.06f, 0.01f, -0.09f } },
        { "Dragon15_l_calf",        0.35f, { 0.03f, 0.0f, 0.15f } },

        // ---------- 尻尾 ----------
#pragma region ---------- 尻尾 ----------
        { "Dragon15_tail_00", 0.70f, {} },
        { "Dragon15_tail_01", 0.55f, {  0.20f, 0.0f,  0.00f } },
        { "Dragon15_tail_02", 0.42f, {  0.01f, 0.0f,  0.03f } },
        { "Dragon15_tail_03", 0.35f, { -0.10f, 0.0f, -0.05f } },
        { "Dragon15_tail_04", 0.25f,  {} },
        { "Dragon15_tail_05", 0.20f,  {} },
        
        { "Dragon15_tail_add_0", 0.30f, {  0.40f, 0.0f,  0.00f }, "Dragon15_tail_03" },
        { "Dragon15_tail_add_1", 0.20f, {  0.40f, 0.0f,  0.03f }, "Dragon15_tail_04" },
        { "Dragon15_tail_add_2", 0.2f,  {  0.30f, 0.0f, -0.05f }, "Dragon15_tail_05" },
        { "Dragon15_tail_add_3", 0.2f,  {  0.60f, 0.0f, -0.13f }, "Dragon15_tail_05" },
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
        { "Dragon15_tail_00", 1.0f, {} }, // 0
        { "Dragon15_tail_01", 1.0f, {} }, // 
        { "Dragon15_tail_02", 1.0f, {} }, // 
        { "Dragon15_tail_03", 1.0f, {} }, // 
        { "Dragon15_tail_04", 1.0f, {} }, // 
        { "Dragon15_tail_05", 1.0f, {} }, // 6
        { "Dragon15_tail_Add0", 1.0f, { 1.0f, 0.0f, 0.0f }, "Dragon15_tail_05"}, // 7


    };
    for (int i = 0; i < _countof(attackDetectionData); ++i)
    {
        RegisterAttackDetectionData(attackDetectionData[i]);
    }
}

void EnemyDragon::UpdateCollisions(const float& elapsedTime)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition()));
    }

    for (int i = AttackData::TrunAttackStart; i < AttackData::TrunAttackEnd; ++i)
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
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition()));
    }
}

void EnemyDragon::SetTurnAttackActiveFlag(const bool& flag)
{
    for (int i = AttackData::TrunAttackStart; i < AttackData::TrunAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}
