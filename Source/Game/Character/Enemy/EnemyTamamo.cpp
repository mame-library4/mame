#include "EnemyTamamo.h"
#include "JudgmentTamamo.h"
#include "ActionTamamo.h"
#include "../Other/MathHelper.h"

// ----- コンストラクタ -----
EnemyTamamo::EnemyTamamo()
    : Enemy("./Resources/Model/Character/Fox.glb")
{
    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i] = std::make_unique<Stone>("./Resources/Model/stone.glb");
    }

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
    behaviorTree_->AddNode("Attack", "Near", 1, BehaviorTree::SelectRule::Priority, new NearAttackJudgment(this), nullptr); // 近距離 (中間ノード)
    
    behaviorTree_->AddNode("Near", "Bite",   1, BehaviorTree::SelectRule::None, new BiteJudgment(this), new BiteAction(this)); // 嚙みつき
    behaviorTree_->AddNode("Near", "Slash",  1, BehaviorTree::SelectRule::None, nullptr, new SlashAction(this)); // ひっかき
    behaviorTree_->AddNode("Near", "Tail",   0, BehaviorTree::SelectRule::None, new TailSwipeJudgment(this), new TailSwipeAction(this)); // 尻尾
    // behaviorTree_->AddNode("Near", "Spin",   1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 回転
    // behaviorTree_->AddNode("Near", "Pounce", 1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 飛びつき
    
    // ----- 距離遠い -----
    behaviorTree_->AddNode("Attack", "Far", 2, BehaviorTree::SelectRule::Priority, new FarAttackJudgment(this), nullptr); // 遠距離 (中間ノード)
    
    behaviorTree_->AddNode("Far", "Slam",  0, BehaviorTree::SelectRule::None, nullptr, new SlamAction(this)); // たたきつけ
    behaviorTree_->AddNode("Far", "Spine",  1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 棘
    behaviorTree_->AddNode("Far", "Tackle", 1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 突進
    behaviorTree_->AddNode("Far", "Pounce", 1, BehaviorTree::SelectRule::None, nullptr, nullptr); // 飛びつき

    // --- 叫ぶ系 -----
    behaviorTree_->AddNode("Attack", "Shout", 0, BehaviorTree::SelectRule::Priority, new ShoutJudgment(this), nullptr); // 叫ぶ系判定 (中間ノード)

    behaviorTree_->AddNode("Shout", "Roar",       0, BehaviorTree::SelectRule::None, new RoarJudgment(this), new RoarAction(this)); // 咆哮
    behaviorTree_->AddNode("Shout", "Intimidate", 1, BehaviorTree::SelectRule::None, new IntimidateJudgment(this), new IntimidateAction(this)); // 威嚇


#pragma endregion 戦闘

#pragma endregion BehaviorTree設定

    // collisionData登録
    RegisterCollisionData();
}

// ----- デストラクタ -----
EnemyTamamo::~EnemyTamamo()
{
}

// ----- 初期化 -----
void EnemyTamamo::Initialize()
{
    // サイズを設定
    GetTransform()->SetScaleFactor(1.5f);

    GetTransform()->SetPositionZ(25);

    // 回転速度設定
    SetRotateSpeed(5.0f);

    // 歩行速度設定
    SetWalkSpeed(3.0f);

    // 体力設定
    SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());
}

// ----- 終了化 -----
void EnemyTamamo::Finalize()
{
}

// ----- 更新処理 -----
void EnemyTamamo::Update(const float& elapsedTime)
{
    Character::Update(elapsedTime);

    UpdateStones(elapsedTime);

    // Collisionデータ更新
    UpdateCollisions(elapsedTime, 0.01f);

    // behaviorTree更新
    UpdateNode(elapsedTime);
}

// ----- 描画 -----
void EnemyTamamo::Render()
{
    Object::Render(0.01f);

    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Render(1.0f);
    }
}

// ----- ImGui用 -----
void EnemyTamamo::DrawDebug()
{
    if (ImGui::Begin("Tamamo"))
    {
        stones_[0]->DrawDebug();

        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

        Character::DrawDebug();

        if (ImGui::TreeNode("Move"))
        {
            ImGui::DragFloat("WalkSpeed", &walkSpeed_);
            ImGui::TreePop();
        }

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

        ImGui::End();
    }
}

// ----- デバッグ用 -----
void EnemyTamamo::DebugRender(DebugRenderer* debugRenderer)
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 forwardVec = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rightVec = GetTransform()->CalcRight();

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
    if (isAttackSphere_)
    {
        for (auto& data : GetAttackDetectionData())
        {
            // 現在アクティブではないでの表示しない
            if (data.GetIsActive() == false) continue;

            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }

    // 戦闘範囲
    debugRenderer->DrawCylinder(position, battleRadius_, 0.5f, { 1,1,1,1 });

    debugRenderer->DrawCylinder(position + forwardVec * battleRadius_, 0.5f, 2.0f, { 0,1,0,1 });
    debugRenderer->DrawCylinder(position + rightVec * battleRadius_, 0.5f, 2.0f, { 0,1,0,1 });
    debugRenderer->DrawCylinder(position + rightVec * -battleRadius_, 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + forwardVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + rightVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + forwardVec + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + (rightVec * -1) + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    

    // 攻撃範囲
    debugRenderer->DrawCylinder(position, nearAttackRadius_, 0.5f, { 1,0,0,1 });
    debugRenderer->DrawCylinder(position, farAttackRadius_, 0.5f, { 1,0,1,1 });

}

void EnemyTamamo::InitializeStones()
{
    // 生成位置を前足元に設定する
    DirectX::XMFLOAT3 pos = GetTransform()->GetPosition() + GetTransform()->CalcForward() * 4;

    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Initialize(pos);
    }

}

void EnemyTamamo::UpdateStones(const float& elapsedTime)
{
    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Update(elapsedTime);
    }
}

// ----- 全攻撃判定設定 -----
void EnemyTamamo::SetAllAttackFlag(const bool& activeFlag)
{
    for (int i = 0; i < GetAttackDetectionDataCount(); ++i)
    {
        GetAttackDetectionData(i).SetIsActive(activeFlag);
    }
}

// ----- 噛みつき攻撃判定設定 -----
void EnemyTamamo::SetBiteAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:C_Tongue_1").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Head_1").SetIsActive(activeFlag);
    //GetAttackDetectionData("R:C_Spine_6").SetIsActive(activeFlag);
}

// ----- ひっかき攻撃判定設定 -----
void EnemyTamamo::SetSlashAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:R_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:R_Middle_Finger_2").SetIsActive(activeFlag);
}

// ----- 尻尾攻撃判定設定 -----
void EnemyTamamo::SetTailSwipeAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:C_Tail_3").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Tail_5").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Tail_8").SetIsActive(activeFlag);
}

// ----- たたきつけ攻撃判定設定 -----
void EnemyTamamo::SetSlamAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:R_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:R_Middle_Finger_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:L_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:L_Middle_Finger_2").SetIsActive(activeFlag);
}

// ----- collisionData登録 -----
void EnemyTamamo::RegisterCollisionData()
{
    // 押し出し判定
    CollisionDetectionData collisionDetectionData[] =
    {
        { "R:C_Tongue_1",   0.4f,   { 0, 0, 15 },   }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.5f,   {},             }, // 顔
        { "R:C_Spine_6",    1.1f,   { 0, -10, -20 } }, // くび
        { "R:C_Hip",        1.0f,   { 0, -20, 10 }  }, // おしり

        { "R:R_Arm_2",      0.25f,  { 0, 30, 0 }    }, // 右ひじ
        { "R:R_Arm_3",      0.25f,  { 0, -15, 0 }   }, // 右手首
        { "R:R_Hand",       0.3f,   { 0, 0, -20 }   }, // 右手

        { "R:L_Arm_2",      0.25f,  { 0, -30, 0 }   }, // 左ひじ
        { "R:L_Arm_3",      0.25f,  { 0, 15, 0 }    }, // 左手首
        { "R:L_Hand",       0.3f,   { 0, 0, 20 }    }, // 左手         

        { "R:R_Leg_2",      0.25f,  {0,45,20},      }, // 右足付け根
        { "R:R_Leg_3",      0.25f,  {0,20,-10},     }, // 右ひざ
        { "R:R_Foot",       0.3f,   {0,0,-15},      }, // 右足

        { "R:L_Leg_2",      0.25f,  {0,-45,-20},    }, // 左足付け根
        { "R:L_Leg_3",      0.25f,  {0,-20,10},     }, // 左ひざ
        { "R:L_foot",       0.3f,   {0,0,15},       }, // 左足

        { "R:C_Tail_3",     0.65f,  {},             }, // 尻尾
        { "R:C_Tail_5",     1.25f,  {},             }, // 尻尾
        { "R:C_Tail_8",     2.5f,   {},             }, // 尻尾

    };
    // くらい判定
    DamageDetectionData damageDetectionData[] =
    {// { 名前, 半径, ダメージ量, offsetPosition }
        { "R:C_Tongue_1",   0.66f, 10, { 0, 0, 15 }    }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.7f,  10, {}              }, // 顔
        { "R:C_Spine_6",    1.3f,  10, { 0, -10, -20 } }, // くび
        { "R:C_Hip",        1.0f,  10, { 0, -20, 10 }  }, // おしり

        { "R:R_Arm_2",      0.4f,  10, { 0, 30, 0 }    }, // 右ひじ
        { "R:R_Arm_3",      0.4f,  10, { 0, -15, 0 }   }, // 右手首
        { "R:R_Hand",       0.4f,  10, { 0, 0, -20 }   }, // 右手
        
        { "R:L_Arm_2",      0.4f,  10, { 0, -30, 0 }   }, // 左ひじ
        { "R:L_Arm_3",      0.4f,  10, { 0, 15, 0 }    }, // 左手首
        { "R:L_Hand",       0.4f,  10, { 0, 0, 20 }    }, // 左手         
        
        { "R:R_Leg_2",      0.4f,  10, {0,45,20},      }, // 右足付け根
        { "R:R_Leg_3",      0.4f,  10, {0,20,-10},     }, // 右ひざ
        { "R:R_Foot",       0.4f,  10, {0,0,-15},      }, // 右足

        { "R:L_Leg_2",      0.4f,  10, {0,-45,-20},    }, // 左足付け根
        { "R:L_Leg_3",      0.4f,  10, {0,-20,10},     }, // 左ひざ
        { "R:L_foot",       0.4f,  10, {0,0,15},       }, // 左足
        
        { "R:C_Tail_3",     1.0f,  10, {},             }, // 尻尾
        { "R:C_Tail_5",     1.8f,  10, {},             }, // 尻尾
        { "R:C_Tail_8",     3.0f,  10, {},             }, // 尻尾
    };
    // 攻撃判定
    AttackDetectionData attackDetectionData[] =
    {
        { "R:C_Tongue_1",   0.66f,  { 0, 0, 15 }    }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.90f,  {},             }, // 顔
        { "R:C_Spine_6",    1.3f,   { 0, -10, -20 } }, // くび

        { "R:R_Arm_2",              0.5f,   { 0, 40, 0 },   }, // 右腕
        { "R:R_Middle_Finger_2",    0.5f,   { -15, 0, 0 },  }, // 右手

        { "R:L_Arm_2",              0.5f,   { 0, -40, 0 },  }, // 左腕
        { "R:L_Middle_Finger_2",    0.5f,   { -15, 0, 0 },  }, // 左手

        { "R:C_Tail_3",     1.0f,   {},             }, // 尻尾
        { "R:C_Tail_5",     1.8f,   {},             }, // 尻尾
        { "R:C_Tail_8",     3.0f,   {},             }, // 尻尾
    };

    for (int i = 0; i < _countof(collisionDetectionData); ++i)
    {
        RegisterCollisionDetectionData(collisionDetectionData[i]);
    }
    for (int i = 0; i < _countof(damageDetectionData); ++i)
    {
        RegisterDamageDetectionData(damageDetectionData[i]);
    }
    for (int i = 0; i < _countof(attackDetectionData); ++i)
    {
        RegisterAttackDetectionData(attackDetectionData[i]);
    }
}
