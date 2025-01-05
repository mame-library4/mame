#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

#include "Camera.h"

#include "UI/UIHuntComplete.h"

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
    SetCollisionRadius(4.5f);

    // 回転速度設定
    SetRotateSpeed(5.0f);

    // 歩行速度設定
    SetWalkSpeed(15.0f);

    // 体力設定
    SetMaxHealth(6000.0f);
    SetHealth(GetMaxHealth());

    PlayAnimation(Enemy::DragonAnimation::Idle0, true);

    // 攻撃判定 無効化
    ResetAllAttackActiveFlag();

    // ジャスト回避判定　無効化
    ResetAllJustDodgeActiveFlag();    

    // 押し出し判定
    SetDownCollisionActiveFlag(false);

    // 部位ごとに怯み値を決める
    staggerValue_[static_cast<int>(PartName::Head)]  = 500.0f;
    staggerValue_[static_cast<int>(PartName::Chest)] = 500.0f;
    staggerValue_[static_cast<int>(PartName::Body)]  = 500.0f;
    staggerValue_[static_cast<int>(PartName::Leg)]   = 500.0f;
    staggerValue_[static_cast<int>(PartName::Tail)]  = 500.0f;
    staggerValue_[static_cast<int>(PartName::Wings)] = 500.0f;

    // 部位ごとの怯み値カウンターをリセットする
    for (int i = 0; i < static_cast<int>(PartName::Max); ++i)
    {
        staggerValueCounter_[i] = 0.0f;
    }

    // 部位破壊フラグを設定
    for (int partIndex = 0; partIndex < static_cast<int>(PartName::Max); ++partIndex)
    {
        isPartDestruction_[partIndex] = false;
    }

    // 攻撃力設定
    SetAttackPower();

    // アウトラインを使用する
    SetIsOutlineActive(true);
    SetOutlineColor({ 0.7f, 0.0f, 0.0f, 1.0f });
}

// ----- 終了化 -----
void EnemyDragon::Finalize()
{
}

// ----- 更新 -----
void EnemyDragon::Update(const float& elapsedTime)
{
    if (isHitStopActive_)
    {
        ++currentHitStopFrame_;

        if (currentHitStopFrame_ >= hitStopFrame_) isHitStopActive_ = false;

        // ヒットストップ中なのでここで終了
        return;
    }

    // behaviorTree更新
    UpdateNode(elapsedTime);

    if(isUpdateAnimation_) Character::Update(elapsedTime);

    // RootMotion
    RootMotion();

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
        ImGui::DragFloat("DebugRadius", &debugRadius_, 0.01f);

        if (ImGui::TreeNodeEx("BehaviorTree", ImGuiTreeNodeFlags_Framed))
        {
            std::string nodeName = (activeNode_ != nullptr) ? activeNode_->GetName() : u8"なし";
            ImGui::Text(u8"Behavior:%s", nodeName.c_str());

            behaviorTree_->DrawDebug();

            ImGui::TreePop();
        }

        ImGui::DragFloat("SuperNovaRadius", &superNovaRadius_, 0.01f, 0.0f, 20.0f);
        ImGui::DragFloat("SuperNovaDamage", &superNovaDamage_, 0.01f, 0.0f, 2.0f);

        if (ImGui::TreeNodeEx("Judgment", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("LongRangeRadius", &longRangeRadius_);

            ImGui::TreePop();
        }

        distanceToPlayer_ = CalcDistanceToPlayerNoConsiderationY();
        ImGui::DragFloat("DistanceToPlayer", &distanceToPlayer_);

        ImGui::Checkbox("EffekSeerEffect", &useEffekseerEffect_);

        if (ImGui::CollapsingHeader("AttackPower"))
        {
            ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 100), ImGuiWindowFlags_NoTitleBar);
            const char* name[] =
            { 
                "SlamAttack",
                "TurnAttack",
                "GuardAttack",
                "TackleAttack",
                "SuperNova",
                "Roar",
                "StompAttack",
                //"FireBreath",
            };

            for (int i = 0; i < static_cast<int>(AttackAction::Max); ++i)
            {
                ImGui::DragFloat(name[i], &attackPower_[i]);
            }

            ImGui::EndChild();
        }

        if (ImGui::TreeNode("JustDodgeDetectionData"))
        {
            ImGui::Checkbox("IsJustDodgeDetectionY", &isJustDodgeDetectionY_);

            for (JustDodgeDetectionData& data : justDodgeDetectionData_)
            {
                data.DrawDebug();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("StaggerValue", ImGuiTreeNodeFlags_Framed))
        {
            std::string name[] =
            {
               "Head", "Chest", "Body", "Leg", "Tail", "Wings"
            };
            std::string tenten = "----------";

            ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 400), ImGuiWindowFlags_NoTitleBar);
            
            for (int i = 0; i < static_cast<int>(PartName::Max); ++i)
            {
                std::string nodeName = tenten + name[i] + tenten;

                if (ImGui::TreeNodeEx(nodeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat("StaggerValue", &staggerValue_[i]);
                    ImGui::DragFloat("Counter", &staggerValueCounter_[i]);

                    ImGui::TreePop();
                }
            }

            ImGui::EndChild();
            ImGui::TreePop();
        }

        if (ImGui::Button("Roar")) SetIsRoar(false);

        ImGui::Checkbox("useAnimation", &isUpdateAnimation_);
        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

        ImGui::DragFloat("WalkSpeed", &walkSpeed_);

        float playerLength = CalcDistanceToPlayer();
        ImGui::DragFloat("PlayerLength", &playerLength);


        

        Character::DrawDebug();
        Object::DrawDebug();

        ImGui::EndMenu();
    }
}

// ----- DebugRebderer -----
void EnemyDragon::DebugRender(DebugRenderer* debugRenderer)
{
    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), debugRadius_, 1.0f, { 0,0.5f,1,1 });
    
    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), superNovaRadius_, 3.0f, {1,0,0,1});

    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), longRangeRadius_, 1.0f, { 0, 1, 0, 1 });

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

    // ジャスト回避判定用
    for (auto& data : GetJustDodgeDetectionData())
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
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new ActionDragon::DeathJudgment(this), new ActionDragon::DeathAction(this));

    // --------------- ダウン ---------------
    behaviorTree_->AddNode("Root", "Down", 1, BehaviorTree::SelectRule::Priority, new ActionDragon::DownJudgment(this), nullptr);
    behaviorTree_->AddNode("Down", "KnockDown",  0, BehaviorTree::SelectRule::None, new ActionDragon::KnockDownJudgment(this), new ActionDragon::KnockDownAction(this));
    behaviorTree_->AddNode("Down", "NormalDown", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::DownAction(this));
    
    // --------------- 攻撃 ---------------
#if 1

#if 1
    behaviorTree_->AddNode("Root", "Attack", 2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    behaviorTree_->AddNode("Attack", "PowerAttack", 0, BehaviorTree::SelectRule::Priority, new ActionDragon::PowerAttackJudgment(this), nullptr);
    behaviorTree_->AddNode("Attack", "NormalAttack", 1, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    behaviorTree_->AddNode("PowerAttack", "SuperNova", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SuperNovaAction(this));

    behaviorTree_->AddNode("NormalAttack", "LongRangeAttack", 0, BehaviorTree::SelectRule::Random, new ActionDragon::LongRangeJudgment(this), nullptr);
    behaviorTree_->AddNode("NormalAttack", "CloseRangeAttack", 0, BehaviorTree::SelectRule::Random, nullptr, nullptr);

    behaviorTree_->AddNode("LongRangeAttack", "Walk", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::WalkAction(this));
    behaviorTree_->AddNode("LongRangeAttack", "TackleAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));


    //behaviorTree_->AddNode("CloseRangeAttack", "SlamCombo", 0, BehaviorTree::SelectRule::Sequence, new ActionDragon::SlamComboAttackJudgment(this), nullptr);
    //behaviorTree_->AddNode("SlamCombo", "SlamAttack0", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));
    //behaviorTree_->AddNode("SlamCombo", "SlamAttack1", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));
    //behaviorTree_->AddNode("SlamCombo", "SlamAttack2", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));

    behaviorTree_->AddNode("CloseRangeAttack", "Turn", 0, BehaviorTree::SelectRule::Sequence, new ActionDragon::TurnAttackJudgment(this), nullptr);
    behaviorTree_->AddNode("Turn", "Turn0", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    behaviorTree_->AddNode("Turn", "Turn1", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::IdleAction(this));

    //behaviorTree_->AddNode("CloseRangeAttack", "SlamAttack",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "TurnAttack",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "Guard",        0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::GuardAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "Walk",         0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::WalkAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "StompAttack",  0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::StompAttackAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "TackleAttack", 0, BehaviorTree::SelectRule::None, new ActionDragon::TackleAttackJudgment(this), new ActionDragon::TackleAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "SuperNova", 0, BehaviorTree::SelectRule::None, new ActionDragon::SuperNovaJudgment(this), new ActionDragon::SuperNovaAction(this));
    //behaviorTree_->AddNode("CloseRangeAttack", "Roar", 0, BehaviorTree::SelectRule::None, new ActionDragon::RoarJudgment(this), new ActionDragon::RoarAction(this));]
#else

    behaviorTree_->AddNode("Root", "Attack", 2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    behaviorTree_->AddNode("Attack", "PowerAttack",  0, BehaviorTree::SelectRule::Priority, new ActionDragon::PowerAttackJudgment(this), nullptr);
    behaviorTree_->AddNode("Attack", "NormalAttack", 1, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    
    behaviorTree_->AddNode("PowerAttack", "SuperNova", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SuperNovaAction(this));
    
    behaviorTree_->AddNode("NormalAttack", "LongRangeAttack", 0, BehaviorTree::SelectRule::Random, new ActionDragon::LongRangeJudgment(this), nullptr);
    behaviorTree_->AddNode("NormalAttack", "CloseRangeAttack", 0, BehaviorTree::SelectRule::Random, nullptr, nullptr);
    
    behaviorTree_->AddNode("LongRangeAttack", "Walk",         0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::WalkAction(this));
    behaviorTree_->AddNode("LongRangeAttack", "TackleAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));

    behaviorTree_->AddNode("CloseRangeAttack", "SlamAttack",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "TurnAttack",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "Guard",        0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::GuardAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "Walk",         0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::WalkAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "StompAttack",  0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::StompAttackAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "TackleAttack", 0, BehaviorTree::SelectRule::None, new ActionDragon::TackleAttackJudgment(this), new ActionDragon::TackleAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "SuperNova",    0, BehaviorTree::SelectRule::None, new ActionDragon::SuperNovaJudgment(this),    new ActionDragon::SuperNovaAction(this));
    behaviorTree_->AddNode("CloseRangeAttack", "Roar",         0, BehaviorTree::SelectRule::None, new ActionDragon::RoarJudgment(this),         new ActionDragon::RoarAction(this));
#endif

#else
    behaviorTree_->AddNode("Root", "Attack", 2, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    behaviorTree_->AddNode("Attack", "Roar", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RoarAction(this));
    behaviorTree_->AddNode("Attack", "SuperNova", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SuperNovaAction(this));
    behaviorTree_->AddNode("Attack", "StompAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::StompAttackAction(this));
    behaviorTree_->AddNode("Attack", "SlamAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAttackAction(this));
    behaviorTree_->AddNode("Attack", "TurnAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    behaviorTree_->AddNode("Attack", "TackleAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));

#endif
}

bool EnemyDragon::CheckStatusChange()
{
    // --------------------
    //      死亡判定
    // --------------------
    // HPがなくなった
    if (GetHealth() <= 0.0f)
    {
        // 死亡フラグを立てる
        SetIsDead(true);

        // リセット
        SetStep(0);

        // 死亡時カメラを使用する
        Camera::Instance().UseDragonDeathCamera();

        // 討伐達成UIを出す
        UIHuntComplete* uIHuntComplete = new UIHuntComplete();

        return true;
    }

    // --------------------
    //      怯み判定
    // --------------------
    for (int i = 0; i < static_cast<int>(Enemy::PartName::Max); ++i)
    {
        if (staggerValueCounter_[i] > staggerValue_[i])
        {
            staggerValueCounter_[i] = 0;
            ++staggerCount_[i];

            // 怯んだ部位を保存
            staggerPartIndex_ = i;

            // 怯みフラグを立てる
            SetIsStagger(true);

            // リセット
            SetStep(0);
            ResetAllAttackActiveFlag();
            SetUseRootMotion(false);


            return true;
        }
    }

    return false;
}

// ----- CollisionData登録 -----
void EnemyDragon::RegisterCollisionData()
{

#pragma region ---------- ダウン時の押し出し判定 ----------
    RegisterCollisionDetectionData({ "Down_Dragon15_head",          0.6f,  true,  {},                       "Dragon15_head"         });
    RegisterCollisionDetectionData({ "Down_Dragon15_neck_2",        1.0f,  true,  { -0.4f, 0.0f, 0.0f },    "Dragon15_neck_2"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_neck_1",        1.0f,  true,  { -0.4f, 0.0f, 0.0f },    "Dragon15_neck_1"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_spine2",        1.05f, true,  {},                       "Dragon15_spine2"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_spine0",        0.85f, true,  {},                       "Dragon15_spine0"       });    
    RegisterCollisionDetectionData({ "Down_Dragon15_l_upperarm1",   0.7f,  false, { 0.2f, -0.15f, 0.0f },   "Dragon15_l_upperarm1"  });
    RegisterCollisionDetectionData({ "Down_Dragon15_r_upperarm1",   0.7f,  false, { 0.2f, 0.15f, 0.0f },    "Dragon15_r_upperarm1"  });
    RegisterCollisionDetectionData({ "Down_Dragon15_l_forearm",     0.55f, false, {},                       "Dragon15_l_forearm"    });
    RegisterCollisionDetectionData({ "Down_Dragon15_r_forearm",     0.55f, false, {},                       "Dragon15_r_forearm"    });    
    RegisterCollisionDetectionData({ "Down_Dragon15_l_calf",        0.55f, true,  { -0.1f, 0.5f, 0.0f },    "Dragon15_l_calf"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_r_calf",        0.55f, true,  { -0.1f, -0.5f, 0.0f },   "Dragon15_r_calf"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_l_horselink",   0.55f, true,  {},                       "Dragon15_l_horselink"  });
    RegisterCollisionDetectionData({ "Down_Dragon15_r_horselink",   0.55f, true,  {},                       "Dragon15_r_horselink"  });
    RegisterCollisionDetectionData({ "Down_Dragon15_l_foot",        0.35f, true,  { 0.05f, -1.0f, 0.1f },   "Dragon15_l_foot"       });
    RegisterCollisionDetectionData({ "Down_Dragon15_r_foot",        0.35f, true,  { 0.1f, 0.0f, -0.1f },    "Dragon15_r_foot"       });
    
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_00",       0.70f, true,  {},                       "Dragon15_tail_00"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_01",       0.55f, true,  { 0.20f, 0.0f,  0.00f },  "Dragon15_tail_01"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_02",       0.42f, true,  { 0.01f, 0.0f,  0.03f },  "Dragon15_tail_02"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_03",       0.35f, true,  { -0.10f, 0.0f, -0.05f }, "Dragon15_tail_03"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_04",       0.25f, true,  {},                       "Dragon15_tail_04"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_05",       0.20f, true,  {},                       "Dragon15_tail_05"      });

    RegisterCollisionDetectionData({ "Down_Dragon15_tail_add_0",    0.30f, true,  { 0.40f, 0.0f,  0.00f },  "Dragon15_tail_03"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_add_1",    0.20f, true,  { 0.40f, 0.0f,  0.03f },  "Dragon15_tail_04"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_add_2",    0.20f, true,  { 0.30f, 0.0f, -0.05f },  "Dragon15_tail_05"      });
    RegisterCollisionDetectionData({ "Down_Dragon15_tail_add_3",    0.20f, true,  { 0.60f, 0.0f, -0.13f },  "Dragon15_tail_05"      });
#pragma endregion ---------- ダウン時の押し出し判定 ----------

#pragma region ---------- 押し出し判定登録 ----------
    RegisterCollisionDetectionData({ "Dragon15_head",        0.4f,  false, {} });
    RegisterCollisionDetectionData({ "Dragon15_neck_1",      1.0f,  false, { -0.4f, 0.0f, 0.0f } });
    RegisterCollisionDetectionData({ "Dragon15_spine2",      1.05f, false, {} });    
    RegisterCollisionDetectionData({ "Dragon15_spine0",      0.85f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_r_hand",      0.3f,  false, {} });
    RegisterCollisionDetectionData({ "Dragon15_r_forearm",   0.32,  false, {} });
    RegisterCollisionDetectionData({ "Dragon15_l_hand",      0.3f,  false, {} });
    RegisterCollisionDetectionData({ "Dragon15_l_forearm",   0.32f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_r_foot",      0.32f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_r_horselink", 0.3f,  false, { 0.06f, 0.01f, -0.09f } });
    RegisterCollisionDetectionData({ "Dragon15_r_calf",      0.35f, false, { 0.03f, 0.0f, 0.15f } });
    RegisterCollisionDetectionData({ "Dragon15_l_foot",      0.32f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_l_horselink", 0.3f,  false, { 0.06f, 0.01f, -0.09f } });
    RegisterCollisionDetectionData({ "Dragon15_l_calf",      0.35f, false, { 0.03f, 0.0f, 0.15f } });
    RegisterCollisionDetectionData({ "Dragon15_tail_00",     0.70f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_tail_01",     0.55f, false, {  0.20f, 0.0f,  0.00f } });
    RegisterCollisionDetectionData({ "Dragon15_tail_02",     0.42f, false, {  0.01f, 0.0f,  0.03f } });
    RegisterCollisionDetectionData({ "Dragon15_tail_03",     0.35f, false, { -0.10f, 0.0f, -0.05f } });
    RegisterCollisionDetectionData({ "Dragon15_tail_04",     0.25f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_tail_05",     0.20f, false, {} });
    RegisterCollisionDetectionData({ "Dragon15_tail_add_0",  0.30f, false, {  0.40f, 0.0f,  0.00f }, "Dragon15_tail_03" });
    RegisterCollisionDetectionData({ "Dragon15_tail_add_1",  0.20f, false, {  0.40f, 0.0f,  0.03f }, "Dragon15_tail_04" });
    RegisterCollisionDetectionData({ "Dragon15_tail_add_2",  0.2f,  false, {  0.30f, 0.0f, -0.05f }, "Dragon15_tail_05" });
    RegisterCollisionDetectionData({ "Dragon15_tail_add_3",  0.2f,  false, {  0.60f, 0.0f, -0.13f }, "Dragon15_tail_05" });

#pragma endregion ---------- 押し出し判定登録 ----------

#pragma region ---------- くらい判定登録 ----------
    // { name, radius, damage倍率, offset, updateName }
    // ---------- 頭 ----------
    RegisterDamageDetectionData({ "Dragon15_head",          1.2f,   2.0f, {} }); // 0
    
    // ---------- 胸 ----------
    RegisterDamageDetectionData({ "Dragon15_neck_1",        1.4f,   1.2f, {} }); // 1

    // ---------- 胴体 ----------
    RegisterDamageDetectionData({ "Dragon15_spine1",        1.3f,   1.2f, {} }); // 2

    // ---------- 前足 ----------
    RegisterDamageDetectionData({ "Dragon15_r_hand",        1.0f,   1.5f, {} }); // 3
    RegisterDamageDetectionData({ "Dragon15_r_forearm",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_hand",        1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_forearm",     1.0f,   1.5f, {} }); // 6
    
    // ---------- 後ろ足 ----------
    RegisterDamageDetectionData({ "Dragon15_r_thigh",       0.65f,  1.5f, { 0.15f, 0.0f, 0.0f } }); // 7
    RegisterDamageDetectionData({ "Dragon15_r_calf",        0.6f,   1.5f, { 0.0f, 0.0f, 0.2f } });
    RegisterDamageDetectionData({ "Dragon15_r_horselink",   0.5f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_foot",        0.45f,  1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_toe11",       0.4f,   1.5f, { 0.06f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_thigh",       0.65f,  1.5f, { 0.15f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_calf",        0.6f,   1.5f, { 0.0f, 0.0f, 0.2f } });
    RegisterDamageDetectionData({ "Dragon15_l_horselink",   0.5f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_foot",        0.45f,  1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_toe11",       0.4f,   1.5f, { 0.06f, 0.0f, 0.0f } }); // 16

    // ---------- 尻尾 ----------
    RegisterDamageDetectionData({ "Dragon15_tail_00",       1.10f,  1.7f, {} });                    // 17
    RegisterDamageDetectionData({ "Dragon15_tail_01",       1.00f,  1.7f, { 0.30f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_tail_03",       0.90f,  1.7f, {} });
    RegisterDamageDetectionData({ "Dragon15_tail_04",       0.80f,  1.7f, { 0.07f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_tail_05",       0.75f,  1.7f, { 0.30f, 0.0f, 0.0f } }); // 21
    
    // ---------- 翼 ----------
    RegisterDamageDetectionData({ "Dragon15_l_wing_01",     1.0f,   1.5f, {} });                    // 22
    RegisterDamageDetectionData({ "Dragon15_l_wing_03",     1.3f,   1.5f, { 0.2f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_wing_04",     1.2f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_06",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_07",     1.25f,  1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_08",     1.25f,  1.5f, { 0.0f, -0.3f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_wing_09",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_10",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_12",     1.0f,   1.5f, {} });                    
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_0",  1.1f,   1.5f, { 1.6f, 0.0f, 0.0f } , "Dragon15_l_wing_04" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_1",  1.0f,   1.5f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_l_wing_07" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_2",  1.0f,   1.5f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_l_wing_10" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_3",  1.0f,   1.5f, { 1.25f, 0.0f, 0.0f } , "Dragon15_l_wing_12" });    
    RegisterDamageDetectionData({ "Dragon15_r_wing_01",     1.0f,   1.5f, {} }); 
    RegisterDamageDetectionData({ "Dragon15_r_wing_03",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_04",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_06",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_07",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_08",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_09",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_10",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_12",     1.0f,   1.5f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_0",  1.1f,   1.5f, { 1.6f, 0.0f, 0.0f }  , "Dragon15_r_wing_04" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_1",  1.0f,   1.5f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_r_wing_07" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_2",  1.0f,   1.5f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_r_wing_10" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_3",  1.0f,   1.5f, { 1.25f, 0.0f, 0.0f } , "Dragon15_r_wing_12" });  //47


    //RegisterDamageDetectionData({ "Dragon15_neck_2",    1.1f, 35.0f, {} }); // 首
#pragma endregion ---------- くらい判定登録 ----------

#pragma region ---------- 攻撃判定登録 ----------
    // ----- たたきつけ攻撃 -----
    RegisterAttackDetectionData({ "SlamAttack_0", 1.5f, {}, "Dragon15_l_hand" });     // 0
    RegisterAttackDetectionData({ "SlamAttack_1", 1.5f, {}, "Dragon15_l_forearm" });  // 1

    // ----- 回転攻撃用 -----
    RegisterAttackDetectionData({ "TurnAttack_0", 1.0f, {}, "Dragon15_tail_00" }); // 2
    RegisterAttackDetectionData({ "TurnAttack_1", 1.0f, {}, "Dragon15_tail_01" });
    RegisterAttackDetectionData({ "TurnAttack_2", 1.0f, {}, "Dragon15_tail_02" });
    RegisterAttackDetectionData({ "TurnAttack_3", 1.0f, {}, "Dragon15_tail_03" });
    RegisterAttackDetectionData({ "TurnAttack_4", 1.0f, {}, "Dragon15_tail_04" });
    RegisterAttackDetectionData({ "TurnAttack_5", 1.0f, {}, "Dragon15_tail_05" });
    RegisterAttackDetectionData({ "TurnAttack_6", 1.0f, { 1.0f,  0.0f, 0.0f }, "Dragon15_tail_05" }); // 8

    // ----- ガード攻撃 -----
    RegisterAttackDetectionData({ "GuardAttack_0",  1.5f, { 2.5f, 0.0f, 0.0f },   "Dragon15_r_wing_12" }); // 9
    RegisterAttackDetectionData({ "GuardAttack_1",  1.5f, { 0.0f, -0.3f, 0.7f },   "Dragon15_r_wing_10" });
    RegisterAttackDetectionData({ "GuardAttack_2",  1.5f, { 0.0f, -0.3f, -0.5f },  "Dragon15_r_wing_04" }); // 11

    // ----- 突進攻撃 -----
    RegisterAttackDetectionData({ "TackleAttack_0",  2.0f, { 1.0f, 0.0f, 0.0f },   "Dragon15_neck_1" });    // 12
    RegisterAttackDetectionData({ "TackleAttack_1",  2.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine0" });    // 13

    // ----- 大技(SuperNova) -----
    RegisterAttackDetectionData({ "SuperNova_0",  13.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" });    // 14 

    // ----- 咆哮 -----
    RegisterAttackDetectionData({ "Roar_0",  20.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" });    // 15

    // ----- 踏みつけ -----
    RegisterAttackDetectionData({ "StompAttack_0",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_l_hand" }); // 16
    RegisterAttackDetectionData({ "StompAttack_1",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_r_hand" });
    RegisterAttackDetectionData({ "StompAttack_2",  2.5f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" });
    RegisterAttackDetectionData({ "StompAttack_3",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_l_foot" });
    RegisterAttackDetectionData({ "StompAttack_4",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_r_foot" }); // 20


    // ----- 叩き付けコンボ攻撃 -----

#pragma endregion ---------- 攻撃判定登録 ----------

#pragma region ---------- ジャスト回避判定登録 ----------
    // ----- たたきつけ攻撃 -----
    RegisterJustDodgeDetectionData({ "SlamAttack_0", 2.0f, { 1.34f, 1.3f, 0.0f }, "Dragon15_spine2" }); // 0
    RegisterJustDodgeDetectionData({ "SlamAttack_1", 2.0f, { 1.7f, 0.5f, 0.0f },  "Dragon15_spine2" }); // 1

    // ----- 回転攻撃 -----
    RegisterJustDodgeDetectionData({ "TurnAttack_0", 2.0f, { 0.0f, 1.0f, 0.0f },  "Dragon15_tail_01" }); // 2
    RegisterJustDodgeDetectionData({ "TurnAttack_1", 2.0f, { 0.0f, -1.0f, 0.0f }, "Dragon15_tail_01" }); 
    RegisterJustDodgeDetectionData({ "TurnAttack_2", 2.0f, { 0.0f, 1.0f, 0.0f },  "Dragon15_tail_03" }); 
    RegisterJustDodgeDetectionData({ "TurnAttack_3", 2.0f, { 0.0f, -1.0f, 0.0f }, "Dragon15_tail_03" }); 
    RegisterJustDodgeDetectionData({ "TurnAttack_4", 2.0f, { 0.0f, 1.0f, 0.0f },  "Dragon15_tail_05" }); 
    RegisterJustDodgeDetectionData({ "TurnAttack_5", 2.0f, { 0.0f, -1.0f, 0.0f }, "Dragon15_tail_05" }); // 7

    // ----- ガード攻撃 -----
    RegisterJustDodgeDetectionData({ "GuardAttack_0", 2.5f, { 2.5f, 0.0f, 0.0f }, "Dragon15_r_wing_12" }); // 8
    RegisterJustDodgeDetectionData({ "GuardAttack_1", 2.5f, { 0.0f, -0.3f, 0.7f }, "Dragon15_r_wing_10" });
    RegisterJustDodgeDetectionData({ "GuardAttack_2", 2.5f, { 0.0f, -0.3f, -0.5f }, "Dragon15_r_wing_04" }); // 10

    // ----- 突進攻撃 -----
    RegisterJustDodgeDetectionData({ "TackleAttack_0", 3.0f, { 1.5f, 0.0f, 0.0f }, "Dragon15_neck_1" }); // 11
    RegisterJustDodgeDetectionData({ "TackleAttack_1", 3.0f, { 0.0f, 0.0f, 0.0f }, "Dragon15_spine0" }); // 12
    RegisterJustDodgeDetectionData({ "TackleAttack_2", 3.0f, { -2.0f, 0.0f, 0.0f }, "Dragon15_spine0" }); // 13

    // ----- 大技(SuperNova) -----
    RegisterJustDodgeDetectionData({ "SuperNova_0",  15.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" }); // 14

    // ----- 咆哮 (ダミー) -----
    RegisterJustDodgeDetectionData({ "Roar_0",  0.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" });    // 15

    // ----- 踏みつけ -----
    RegisterJustDodgeDetectionData({ "StompAttack_0",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_l_hand" }); // 16
    RegisterJustDodgeDetectionData({ "StompAttack_1",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_r_hand" });
    RegisterJustDodgeDetectionData({ "StompAttack_2",  2.5f, { 0.0f, 0.0f, 0.0f },   "Dragon15_spine2" });
    RegisterJustDodgeDetectionData({ "StompAttack_3",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_l_foot" });
    RegisterJustDodgeDetectionData({ "StompAttack_4",  1.0f, { 0.0f, 0.0f, 0.0f },   "Dragon15_r_foot" }); // 20

#pragma endregion ---------- ジャスト回避判定登録 ----------
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

    for(int i = AttackData::TrunAttackStart; i <= AttackData::TackleAttackEnd; ++i)
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

    // ジャスト回避判定更新
    for (JustDodgeDetectionData& data : justDodgeDetectionData_)
    {
        DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition());
        
        // Yは全て0.0fにする
        if(isJustDodgeDetectionY_) pos.y = 0.0f;

        data.SetJointPosition(pos);
    }
}


#pragma region ---------- 攻撃判定 ----------
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

// ----- 攻撃判定設定 -----
void EnemyDragon::SetAttackActiveFlag(const AttackAction& type, const bool& flag)
{
    // 攻撃判定を有効化するときに、有効化にする許可がないとできない
    if (flag == true && isAttackActivationAllowed_ == false)
    {
        return;
    }

    // 攻撃判定を設定
    SetIsAttackActive(flag);

    int dataList[][2] =
    {
        { static_cast<int>(AttackData::SlamAttackStart),   static_cast<int>(AttackData::SlamAttackEnd) },
        { static_cast<int>(AttackData::TrunAttackStart),   static_cast<int>(AttackData::TrunAttackEnd) },
        { static_cast<int>(AttackData::GuardAttackStart),  static_cast<int>(AttackData::GuardAttackEnd) },
        { static_cast<int>(AttackData::TackleAttackStart), static_cast<int>(AttackData::TackleAttackEnd) },
        { static_cast<int>(AttackData::SuperNovaStart),    static_cast<int>(AttackData::SuperNovaEnd) },
        { static_cast<int>(AttackData::RoarStart),         static_cast<int>(AttackData::RoarEnd) },
        { static_cast<int>(AttackData::StompAttackStart),  static_cast<int>(AttackData::StompAttackEnd) },
    };
    const int start = dataList[static_cast<int>(type)][0];
    const int end = dataList[static_cast<int>(type)][1];

    for (int i = start; i <= end; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

#pragma endregion ---------- 攻撃判定 ----------

#pragma region ---------- ジャスト回避判定 ----------
// ----- 全ジャスト回避判定無効化 -----
void EnemyDragon::ResetAllJustDodgeActiveFlag()
{
    for (JustDodgeDetectionData& data : justDodgeDetectionData_)
    {
        data.SetIsActive(false);
    }
}

// ----- ジャスト回避判定設定 -----
void EnemyDragon::SetJustDodgeActiveFlag(const AttackAction& type, const bool& flag)
{
    int dataList[][2] =
    {
        { static_cast<int>(JustDodgeData::SlamAttackStart),   static_cast<int>(JustDodgeData::SlamAttackEnd) },
        { static_cast<int>(JustDodgeData::TurnAttackStart),   static_cast<int>(JustDodgeData::TurnAttackEnd) },
        { static_cast<int>(JustDodgeData::GuardAttackStart),  static_cast<int>(JustDodgeData::GuardAttackEnd) },
        { static_cast<int>(JustDodgeData::TackleAttackStart), static_cast<int>(JustDodgeData::TackleAttackEnd) },
        { static_cast<int>(JustDodgeData::SuperNovaStart),    static_cast<int>(JustDodgeData::SuperNovaEnd) },
        { static_cast<int>(JustDodgeData::RoarStart),         static_cast<int>(JustDodgeData::RoarEnd) },
        { static_cast<int>(JustDodgeData::StompAttackStart),  static_cast<int>(JustDodgeData::StompAttackEnd) },
    };
    const int start = dataList[static_cast<int>(type)][0];
    const int end = dataList[static_cast<int>(type)][1];

    for (int i = start; i <= end; ++i)
    {
        GetJustDodgeDetectionData(i).SetIsActive(flag);
    }
}

#pragma endregion ---------- ジャスト回避判定 ----------

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

// ----- 部位ダメージ処理 -----
void EnemyDragon::AddDamagePart(const float& damage, const int& dataIndex)
{
    const DamageData partIndex = static_cast<DamageData>(dataIndex);

    // 頭
    if (partIndex == DamageData::Head)
    {
        staggerValueCounter_[static_cast<int>(PartName::Head)] += damage;
    }
    // 胸
    else if (partIndex == DamageData::Chest)
    {
        staggerValueCounter_[static_cast<int>(PartName::Chest)] += damage;
    }
    // 体
    else if (partIndex == DamageData::Body)
    {
        staggerValueCounter_[static_cast<int>(PartName::Body)] += damage;
    }
    // 前足
    else if (partIndex >= DamageData::FrontLeg && partIndex <= DamageData::FrontLegEnd)
    {
        staggerValueCounter_[static_cast<int>(PartName::Leg)] += damage;
    }
    // 後ろ足
    else if (partIndex >= DamageData::BackLeg && partIndex <= DamageData::BackLegEnd)
    {
        staggerValueCounter_[static_cast<int>(PartName::Leg)] += damage;
    }
    // 尻尾
    else if (partIndex >= DamageData::Tail && partIndex <= DamageData::TailEnd)
    {
        staggerValueCounter_[static_cast<int>(PartName::Tail)] += damage;
    }
    // 翼
    else if (partIndex >= DamageData::Wings && partIndex <= DamageData::WingsEnd)
    {
        staggerValueCounter_[static_cast<int>(PartName::Wings)] += damage;
    }
}

void EnemyDragon::SetAttackPower()
{
    attackPower_[static_cast<int>(AttackAction::SlamAttack)]    = 50.0f;
    attackPower_[static_cast<int>(AttackAction::TurnAttack)]    = 50.0f;
    attackPower_[static_cast<int>(AttackAction::GuardAttack)]   = 50.0f;
    attackPower_[static_cast<int>(AttackAction::TackleAttack)]  = 50.0f;
    attackPower_[static_cast<int>(AttackAction::SuperNova)]     = 200.0f;
    attackPower_[static_cast<int>(AttackAction::StompAttack)]   = 50.0f;
}
