#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

#include "Camera.h"

#include "UI/UIHuntComplete.h"

// ----- �R���X�g���N�^ -----
EnemyDragon::EnemyDragon()
    : Enemy("./Resources/Model/Character/Enemy/Dragon.gltf", 1.0f),
    circle_("./Resources/Model/Circle/circle.gltf", 5.0f)
{
    // BehaviorTree�ݒ�
    behaviorData_ = std::make_unique<BehaviorData>();       // BehaviorData����
    behaviorTree_ = std::make_unique<BehaviorTree>(this);   // BehaviorTree����
    RegisterBehaviorNode();

    // CollisionData�o�^
    RegisterCollisionData();
}

// ----- ������ -----
void EnemyDragon::Initialize()
{
    // �ʒu�ݒ�
    //GetTransform()->SetPositionZ(-20);
    GetTransform()->SetPositionZ(-10);
    //GetTransform()->SetPositionZ(10);

    // �T�C�Y��ݒ�
    GetTransform()->SetScaleFactor(1.5f);

    // �X�e�[�W�Ƃ̔���offset�ݒ�
    SetCollisionRadius(2.5f);

    // ��]���x�ݒ�
    SetRotateSpeed(5.0f);

    // ���s���x�ݒ�
    SetWalkSpeed(15.0f);

    // �̗͐ݒ�
    SetMaxHealth(60.0f);
    //SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());

    PlayAnimation(Enemy::DragonAnimation::Idle0, true);


    // �U������ ������
    ResetAllAttackActiveFlag();

    // �����o������
    SetDownCollisionActiveFlag(false);

    // ���ʂ��Ƃ̗̑͂�ݒ肷��
    partHealth_[static_cast<int>(PartName::Head)]  = 50.0f;
    partHealth_[static_cast<int>(PartName::Chest)] = 50.0f;
    partHealth_[static_cast<int>(PartName::Body)]  = 50.0f;
    partHealth_[static_cast<int>(PartName::Leg)]   = 50.0f;
    partHealth_[static_cast<int>(PartName::Tail)]  = 50.0f;
    partHealth_[static_cast<int>(PartName::Wings)] = 50.0f;

    // ���ʔj��t���O��ݒ�
    for (int partIndex = 0; partIndex < static_cast<int>(PartName::Max); ++partIndex)
    {
        isPartDestruction_[partIndex] = false;
    }
}

// ----- �I���� -----
void EnemyDragon::Finalize()
{
}

// ----- �X�V -----
void EnemyDragon::Update(const float& elapsedTime)
{
    // behaviorTree�X�V
    UpdateNode(elapsedTime);

    if(isUpdateAnimation_) Character::Update(elapsedTime);

    // RootMotion
    RootMotion();

    // Collision�f�[�^�X�V
    UpdateCollisions(elapsedTime);


    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    if(GetIsStageCollisionJudgement() == false) CollisionCharacterVsStage();

    DirectX::XMFLOAT3 pos = GetTransform()->GetPosition();
    pos.y = 0.01f;
    circle_.GetTransform()->SetPosition(pos);
}

// ----- �`�� -----
void EnemyDragon::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);

    if(GetIsStageCollisionJudgement()) circle_.Render(psShader);
}

// ----- ImGui�p -----
void EnemyDragon::DrawDebug()
{
    if (ImGui::BeginMenu("Dragon"))
    {
        if (ImGui::TreeNode("PartDestruction"))
        {
            ImGui::DragFloat("Head", &partHealth_[static_cast<int>(PartName::Head)]);
            ImGui::DragFloat("Chest", &partHealth_[static_cast<int>(PartName::Chest)]);
            ImGui::DragFloat("Body", &partHealth_[static_cast<int>(PartName::Body)]);
            ImGui::DragFloat("Leg", &partHealth_[static_cast<int>(PartName::Leg)]);
            ImGui::DragFloat("Tail", &partHealth_[static_cast<int>(PartName::Tail)]);
            ImGui::DragFloat("Wings", &partHealth_[static_cast<int>(PartName::Wings)]);

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

        if (ImGui::TreeNode("Judgment"))
        {
            ImGui::DragFloat("NearAttackRadius", &nearAttackRadius_);
            ImGui::DragFloat("ComboFlyAttackRadius", &comboFlyAttackRadius_);

            ImGui::TreePop();
        }

        Character::DrawDebug();
        Object::DrawDebug();

        std::string nodeName = (activeNode_ != nullptr) ? activeNode_->GetName() : u8"�Ȃ�";
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
            // ���݃A�N�e�B�u�ł͂Ȃ��̂ŕ\�����Ȃ�
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
            // ���݃A�N�e�B�u�ł͂Ȃ��ł̕\�����Ȃ�
            if (data.GetIsActive() == false) continue;

            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }

    for (auto& data : GetFlinchDetectionData())
    {
        // ���݃A�N�e�B�u�ł͂Ȃ��ł̕\�����Ȃ�
        if (data.GetIsActive() == false) continue;

        //debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), {1,0,1,1});
    }

}

// ----- Behavior�o�^ -----
void EnemyDragon::RegisterBehaviorNode()
{
    // Behavior Node�ǉ�
    behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    // --------------- ���S ---------------
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new ActionDragon::DeathAction(this));

    // --------------- ���� ---------------
    behaviorTree_->AddNode("Root",   "Flinch",       1, BehaviorTree::SelectRule::Priority, new FlinchJudgment(this), nullptr);
    behaviorTree_->AddNode("Flinch", "PartDestructionFlinch", 0, BehaviorTree::SelectRule::None, new PartDestructionFlinchJudgment(this), new ActionDragon::PartDestructionFlinchAction(this));
    behaviorTree_->AddNode("Flinch", "NormalFlinch",          1, BehaviorTree::SelectRule::None, new NormalFlinchJudgment(this), new ActionDragon::FlinchAction(this));
    behaviorTree_->AddNode("Flinch", "FlyFlinch",             2, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FlyFlinchAction(this));

    // --------------- ��퓬 ---------------
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new ActionDragon::NonBattleIdleAction(this));
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::NonBattleWalkAction(this));

    // --------------- �퓬 ---------------
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
    

    behaviorTree_->AddNode("Near", "MostNear",    0, BehaviorTree::SelectRule::SequentialLooping, nullptr, nullptr);
    //behaviorTree_->AddNode("Near", "MostNear",    0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);
    //behaviorTree_->AddNode("Near", "MostNear",    0, BehaviorTree::SelectRule::Random, nullptr, nullptr);

    
    behaviorTree_->AddNode("MostNear", "SuperNova",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SuperNovaAction(this));
    behaviorTree_->AddNode("MostNear", "TurnAttack",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TurnAttackAction(this));
    
    behaviorTree_->AddNode("MostNear", "ComboSlam",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboSlamAction(this));    
    behaviorTree_->AddNode("MostNear", "KnockBack",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::KnockBackAction(this));
    behaviorTree_->AddNode("MostNear", "FlyAttack",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FlyAttackAction(this));    

#if 1
    behaviorTree_->AddNode("MostNear", "ComboFlySlam", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboFlySlamAction(this));
    behaviorTree_->AddNode("MostNear", "FireBreath", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreath(this));
    behaviorTree_->AddNode("MostNear", "FireBreathCombo", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreathCombo(this));
    
    behaviorTree_->AddNode("MostNear", "Tackle", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));
#endif


    //behaviorTree_->AddNode("Near", "BackStep",    0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::BackStepAction(this));
    //behaviorTree_->AddNode("Near", "Slam",        1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::SlamAction(this));
    //behaviorTree_->AddNode("Near", "ComboCharge", 1, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::ComboChargeAction(this));

    //behaviorTree_->AddNode("Far", "RiseAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::RiseAttackAction(this));
    
    
    behaviorTree_->AddNode("Far", "Tackle",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::TackleAction(this));
    behaviorTree_->AddNode("Far", "FireBreath",        0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreath(this));
    behaviorTree_->AddNode("Far", "FireBreathCombo",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::FireBreathCombo(this));
    //behaviorTree_->AddNode("Far", "Move",     0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveAction(this));
    
    //behaviorTree_->AddNode("Far", "MoveTurn",   0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveTurnAction(this));
    //behaviorTree_->AddNode("Far", "MoveAttack", 0, BehaviorTree::SelectRule::None, nullptr, new ActionDragon::MoveAttackAction(this));

}

bool EnemyDragon::CheckStatusChange()
{
    // --------------------
    //      ���S����
    // --------------------
    // HP���Ȃ��Ȃ���
    if (GetHealth() <= 0.0f)
    {
        // ���Z�b�g
        SetStep(0); 
        ResetAllAttackActiveFlag();

        SetUseRootMotion(false);

        // ���S�t���O�𗧂Ă�
        SetIsDead(true);

        // ���S���J�������g�p����
        Camera::Instance().SetUseEnemyDeathCamera();

        // �����B��UI���o��
        UIHuntComplete* uIHuntComplete = new UIHuntComplete();

        return true;
    }

    // --------------------
    //      ���ݔ���
    // --------------------
    // ���ɋ��ݒ�
    if (GetIsFlinch()) return false;
    // ���񎞂Ȃǂɒʂ�
    if (oldHealth_ <= 0.0f)
    {
        oldHealth_ = GetHealth();
        return false;
    }
    const float damage = oldHealth_ - GetHealth();
    oldHealth_ = GetHealth();

    // damage�� 30 ���傫����΋���
    //if (damage > 30.0f)
    if (damage > 100.0f)
    {
        SetIsFlinch(true);
        // ���Z�b�g
        SetStep(0); 
        ResetAllAttackActiveFlag();

        SetUseRootMotion(false);

        return true;
    }

    return false;
}

// ----- CollisionData�o�^ -----
void EnemyDragon::RegisterCollisionData()
{

#pragma region ---------- �_�E�����̉����o������ ----------
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
#pragma endregion ---------- �_�E�����̉����o������ ----------

#pragma region ---------- �����o������o�^ ----------
    RegisterCollisionDetectionData({ "Dragon15_head",        0.4f,  false, {} });
    RegisterCollisionDetectionData({ "Dragon15_neck_1",      1.0f,  false, { -0.4f, 0.0f, 0.0f } });
    RegisterCollisionDetectionData({ "Dragon15_spine2",      1.05f, false, {} });    
    RegisterCollisionDetectionData({ "Dragon15_spine0",      0.85f, false, {} });
    RegisterCollisionDetectionData({ "root",                 0.85f, false, {} });
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

#pragma endregion ---------- �����o������o�^ ----------

#pragma region ---------- ���炢����o�^ ----------
    // { name, radius, damage, offset, updateName }
    // ---------- �� ----------
    RegisterDamageDetectionData({ "Dragon15_head",      1.2f, 65.0f, {} }); // 0
    
    // ---------- �� ----------
    RegisterDamageDetectionData({ "Dragon15_neck_1",    1.4f, 27.0f, {} }); // 1

    // ---------- ���� ----------
    RegisterDamageDetectionData({ "Dragon15_spine1",    1.3f, 25.0f, {} }); // 2

    // ---------- �O�� ----------
    RegisterDamageDetectionData({ "Dragon15_r_hand",    1.0f, 35.0f, {} }); // 3
    RegisterDamageDetectionData({ "Dragon15_r_forearm", 1.0f, 35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_hand",    1.0f, 35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_forearm", 1.0f, 35.0f, {} }); // 6
    
    // ---------- ��둫 ----------
    RegisterDamageDetectionData({ "Dragon15_r_thigh",     0.65f, 35.0f, { 0.15f, 0.0f, 0.0f } }); // 7
    RegisterDamageDetectionData({ "Dragon15_r_calf",      0.6f,  35.0f, { 0.0f, 0.0f, 0.2f } });
    RegisterDamageDetectionData({ "Dragon15_r_horselink", 0.5f,  35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_foot",      0.45f, 35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_toe11",     0.4f,  35.0f, { 0.06f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_thigh",     0.65f, 35.0f, { 0.15f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_calf",      0.6f,  35.0f, { 0.0f, 0.0f, 0.2f } });
    RegisterDamageDetectionData({ "Dragon15_l_horselink", 0.5f,  35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_foot",      0.45f, 35.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_toe11",     0.4f,  35.0f, { 0.06f, 0.0f, 0.0f } }); // 16

    // ---------- �K�� ----------
    RegisterDamageDetectionData({ "Dragon15_tail_00", 1.10f, 45.0f, {} });                    // 17
    RegisterDamageDetectionData({ "Dragon15_tail_01", 1.00f, 45.0f, { 0.30f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_tail_03", 0.90f, 45.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_tail_04", 0.80f, 45.0f, { 0.07f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_tail_05", 0.75f, 45.0f, { 0.30f, 0.0f, 0.0f } }); // 21
    
    // ---------- �� ----------
    RegisterDamageDetectionData({ "Dragon15_l_wing_01", 1.0f,  50.0f, {} });                    // 22
    RegisterDamageDetectionData({ "Dragon15_l_wing_03", 1.3f,  50.0f, { 0.2f, 0.0f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_wing_04", 1.2f,  50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_06", 1.0f,  50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_07", 1.25f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_08", 1.25f, 50.0f, { 0.0f, -0.3f, 0.0f } });
    RegisterDamageDetectionData({ "Dragon15_l_wing_09", 1.0f,  50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_10", 1.0f,  50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_l_wing_12", 1.0f,  50.0f, {} });                    
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_0", 1.1f, 50.0f, { 1.6f, 0.0f, 0.0f } , "Dragon15_l_wing_04" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_1", 1.0f, 50.0f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_l_wing_07" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_2", 1.0f, 50.0f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_l_wing_10" });
    RegisterDamageDetectionData({ "Dragon15_l_wing_add_3", 1.0f, 50.0f, { 1.25f, 0.0f, 0.0f } , "Dragon15_l_wing_12" });    
    RegisterDamageDetectionData({ "Dragon15_r_wing_01", 1.0f, 50.0f, {} }); 
    RegisterDamageDetectionData({ "Dragon15_r_wing_03", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_04", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_06", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_07", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_08", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_09", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_10", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_12", 1.0f, 50.0f, {} });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_0", 1.1f, 50.0f, { 1.6f, 0.0f, 0.0f }  , "Dragon15_r_wing_04" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_1", 1.0f, 50.0f, { 1.2f, 0.0f, 0.0f }  , "Dragon15_r_wing_07" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_2", 1.0f, 50.0f, { 1.0f, 0.0f, 0.0f }  , "Dragon15_r_wing_10" });
    RegisterDamageDetectionData({ "Dragon15_r_wing_add_3", 1.0f, 50.0f, { 1.25f, 0.0f, 0.0f } , "Dragon15_r_wing_12" });  //47


    //RegisterDamageDetectionData({ "Dragon15_neck_2",    1.1f, 35.0f, {} }); // ��
#pragma endregion ---------- ���炢����o�^ ----------

#pragma region ---------- �U������o�^ ----------
    // ----- ��]�U���p -----
    RegisterAttackDetectionData({ "TurnAttack_0", 1.0f, {}, "Dragon15_tail_00" }); // 0
    RegisterAttackDetectionData({ "TurnAttack_1", 1.0f, {}, "Dragon15_tail_01" });
    RegisterAttackDetectionData({ "TurnAttack_2", 1.0f, {}, "Dragon15_tail_02" });
    RegisterAttackDetectionData({ "TurnAttack_3", 1.0f, {}, "Dragon15_tail_03" });
    RegisterAttackDetectionData({ "TurnAttack_4", 1.0f, {}, "Dragon15_tail_04" });
    RegisterAttackDetectionData({ "TurnAttack_5", 1.0f, {}, "Dragon15_tail_05" }); // 5

    // ----- �ːi�U���p -----
    RegisterAttackDetectionData({ "TackleAttack_0", 1.0f, {}, "Dragon15_neck_3" }); // 6
    RegisterAttackDetectionData({ "TackleAttack_1", 1.0f, {}, "Dragon15_neck_1" }); 
    RegisterAttackDetectionData({ "TackleAttack_2", 1.0f, {}, "Dragon15_spine2" }); 
    RegisterAttackDetectionData({ "TackleAttack_3", 1.0f, {}, "Dragon15_spine0" }); 
    RegisterAttackDetectionData({ "TackleAttack_4", 1.0f, {}, "Dragon15_tail_00"}); // 10

    // ----- �󒆂��炽�������U�� -----
    RegisterAttackDetectionData({ "FlyAttack_0", 1.0f, {}, "Dragon15_r_hand" }); // 11
    RegisterAttackDetectionData({ "FlyAttack_1", 1.0f, {}, "Dragon15_l_hand" }); // 12
    
    // ----- �R���{���������U�� -----
    RegisterAttackDetectionData({ "ComboSlam_0", 0.8f, {}, "Dragon15_r_hand"     }); // 13
    RegisterAttackDetectionData({ "ComboSlam_1", 0.8f, {}, "Dragon15_r_forearm"  }); 
    RegisterAttackDetectionData({ "ComboSlam_2", 0.7f, {}, "Dragon15_r_finger21" }); // 15

    // ----- ������΂��U�� -----
    RegisterAttackDetectionData({ "KnockBack_0",  0.55f, { 1.9f,  0.15f, 0.0f },  "Dragon15_r_wing_04" }); // 16
    RegisterAttackDetectionData({ "KnockBack_1",  0.6f,  { 1.21f, 0.15f, 0.0f },  "Dragon15_r_wing_04" });
    RegisterAttackDetectionData({ "KnockBack_2",  0.65f, { 0.4f,  0.2f,  0.0f },  "Dragon15_r_wing_04" });
    RegisterAttackDetectionData({ "KnockBack_3",  0.45f, { 2.0f, 0.0f, 0.0f },    "Dragon15_r_wing_07" });
    RegisterAttackDetectionData({ "KnockBack_4",  0.55f, { 1.4f, 0.0f, 0.0f },    "Dragon15_r_wing_07" });
    RegisterAttackDetectionData({ "KnockBack_5",  0.6f,  { 0.7f, 0.0f, 0.0f },    "Dragon15_r_wing_07" });
    RegisterAttackDetectionData({ "KnockBack_6",  0.65f, {},                      "Dragon15_r_wing_07" });
    RegisterAttackDetectionData({ "KnockBack_7",  0.45f, { 1.85f, 0.0f, 0.0f },   "Dragon15_r_wing_10" });
    RegisterAttackDetectionData({ "KnockBack_8",  0.5f,  { 1.25f, 0.0f, 0.0f },   "Dragon15_r_wing_10" });
    RegisterAttackDetectionData({ "KnockBack_9",  0.55f, { 0.5f, 0.0f, 0.0f },    "Dragon15_r_wing_10" });
    RegisterAttackDetectionData({ "KnockBack_10", 0.6f,  { -0.3f, 0.0f, 0.0f },   "Dragon15_r_wing_10" });
    RegisterAttackDetectionData({ "KnockBack_11", 0.45f, { 2.25f, 0.0f, 0.0f },   "Dragon15_r_wing_12" });
    RegisterAttackDetectionData({ "KnockBack_12", 0.5f,  { 1.55f, 0.0f, 0.0f },   "Dragon15_r_wing_12" });
    RegisterAttackDetectionData({ "KnockBack_13", 0.6f,  { 0.8f, 0.0f, 0.0f },    "Dragon15_r_wing_12" }); // 29

#pragma endregion ---------- �U������o�^ ----------


    // ���ݔ���o�^
    AttackDetectionData flinchDetectionData[] =
    {
        //// ----- ��]�U���p -----
        //{ "TurnAttack_0", 1.0f, {}, "Dragon15_tail_00" }, // 0
        //{ "TurnAttack_1", 1.0f, {}, "Dragon15_tail_01" }, // 
        //{ "TurnAttack_2", 1.0f, {}, "Dragon15_tail_02" }, // 
        //{ "TurnAttack_3", 1.0f, {}, "Dragon15_tail_03" }, // 
        //{ "TurnAttack_4", 1.0f, {}, "Dragon15_tail_04" }, // 
        //{ "TurnAttack_5", 1.0f, {}, "Dragon15_tail_05" }, // 5

        //// ----- �ːi�U���p -----
        //{ "TackleAttack_0", 1.0f, {}, "Dragon15_neck_3" },  // 6
        //{ "TackleAttack_1", 1.0f, {}, "Dragon15_neck_1" },  // 
        //{ "TackleAttack_2", 1.0f, {}, "Dragon15_spine2" },  // 
        //{ "TackleAttack_3", 1.0f, {}, "Dragon15_spine0" },  // 
        //{ "TackleAttack_4", 1.0f, {}, "Dragon15_tail_00" }, // 10

        //// ----- �󒆂��炽�������U�� -----
        //{ "FlyAttack_0", 1.0f, {}, "Dragon15_r_hand" },    // 11
        //{ "FlyAttack_1", 1.0f, {}, "Dragon15_l_hand" },    // 12

        // ----- �R���{���������U�� -----
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
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // �U������X�V
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
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

    // �����o������X�V
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition());
        
        if(data.GetFixedY()) 
            pos.y = 0.0f;

        data.SetJointPosition(pos);
    }


    for (AttackDetectionData& data : flinchDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition()));
    }
}


#pragma region ----- �U������ -----
// ----- �S�U�����薳���� -----
void EnemyDragon::ResetAllAttackActiveFlag()
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(false);

    for (AttackDetectionData& data : attackDetectionData_)
    {
        data.SetIsActive(false);
    }
}

// ----- ��]�U������ݒ� -----
void EnemyDragon::SetTurnAttackActiveFlag(const bool& flag)
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(flag);

    for (int i = AttackData::TrunAttackStart; i <= AttackData::TrunAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- �ːi�U������ݒ� -----
void EnemyDragon::SetTackleAttackActiveFlag(const bool& flag)
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(flag);

    for (int i = AttackData::TackleAttackStart; i <= AttackData::TackleAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- �㏸�U������ݒ� -----
void EnemyDragon::SetFlyAttackActiveFlag(const bool& flag)
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(flag);

    for (int i = AttackData::FlyAttackStart; i <= AttackData::FlyAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- �R���{���������U������ݒ� -----
void EnemyDragon::SetComboSlamAttackActiveFlag(const bool& flag)
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(flag);

    for (int i = AttackData::ComboSlamAttackStart; i <= AttackData::ComboSlamAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}

// ----- ������΂��U������ݒ� -----
void EnemyDragon::SetKnockBackAttackActiveFalg(const bool& flag)
{
    // �U������t���O���Z�b�g����
    SetIsAttackActive(flag);

    for (int i = AttackData::KnockBackAttackStart; i <= AttackData::KnockBackAttackEnd; ++i)
    {
        GetAttackDetectionData(i).SetIsActive(flag);
    }
}
#pragma endregion ----- �U������ -----

// ----- �����o������ -----
void EnemyDragon::SetDownCollisionActiveFlag(const bool& flag)
{
    // �S�Ẵt���O�����Z�b�g����
    const bool resetFlag = !flag;
    //for (auto& data : GetCollisionDetectionData())
    for (int i = 0; i < collisionDetectionData_.size(); ++i)
    {
        GetCollisionDetectionData(i).SetIsActive(resetFlag);
    }
    // �_�E�����̉����o������ݒ�
    for (int i = CollisionData::DownStart; i < CollisionData::DownEnd; ++i)
    {
        GetCollisionDetectionData(i).SetIsActive(flag);
    }
}

// ----- ���ʃ_���[�W���� -----
void EnemyDragon::AddDamagePart(const float& damage, const int& dataIndex)
{
    const DamageData partIndex = static_cast<DamageData>(dataIndex);

    // ��
    if (partIndex == DamageData::Head)
    {
        partHealth_[static_cast<int>(PartName::Head)] -= damage;
    }
    // ��
    else if (partIndex == DamageData::Chest)
    {
        partHealth_[static_cast<int>(PartName::Chest)] -= damage;
    }
    // ��
    else if (partIndex == DamageData::Body)
    {
        partHealth_[static_cast<int>(PartName::Body)] -= damage;
    }
    // �O��
    else if (partIndex >= DamageData::FrontLeg && partIndex <= DamageData::FrontLegEnd)
    {
        partHealth_[static_cast<int>(PartName::Leg)] -= damage;
    }
    // ��둫
    else if (partIndex >= DamageData::BackLeg && partIndex <= DamageData::BackLegEnd)
    {
        partHealth_[static_cast<int>(PartName::Leg)] -= damage;
    }
    // �K��
    else if (partIndex >= DamageData::Tail && partIndex <= DamageData::TailEnd)
    {
        partHealth_[static_cast<int>(PartName::Tail)] -= damage;
    }
    // ��
    else if (partIndex >= DamageData::Wings && partIndex <= DamageData::WingsEnd)
    {
        partHealth_[static_cast<int>(PartName::Wings)] -= damage;
    }
}