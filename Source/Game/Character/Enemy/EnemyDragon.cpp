#include "EnemyDragon.h"
#include "JudgmentDragon.h"
#include "ActionDragon.h"

// ----- �R���X�g���N�^ -----
EnemyDragon::EnemyDragon()
    : Enemy("./Resources/Model/Character/Enemy/Dragon.gltf"),
    scaleFacter_(1.0f)
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
    GetTransform()->SetPositionZ(10);

    // �T�C�Y��ݒ�
    GetTransform()->SetScaleFactor(1.5f);

    // �X�e�[�W�Ƃ̔���offset�ݒ�
    SetCollisionRadius(2.5f);

    // ��]���x�ݒ�
    SetRotateSpeed(5.0f);

    // ���s���x�ݒ�
    SetWalkSpeed(3.0f);

    // �̗͐ݒ�
    SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());
}

// ----- �I���� -----
void EnemyDragon::Finalize()
{
}

// ----- �X�V -----
void EnemyDragon::Update(const float& elapsedTime)
{
    Character::Update(elapsedTime);

    // Collision�f�[�^�X�V
    UpdateCollisions(elapsedTime, scaleFacter_);

    // behaviorTree�X�V
    UpdateNode(elapsedTime);

    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    CollisionCharacterVsStage();
}

// ----- �`�� -----
void EnemyDragon::Render(ID3D11PixelShader* psShader)
{
    Object::Render(scaleFacter_, psShader);
}

// ----- ImGui�p -----
void EnemyDragon::DrawDebug()
{
    if (ImGui::BeginMenu("Dragon"))
    {
        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

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
}

// ----- Behavior�o�^ -----
void EnemyDragon::RegisterBehaviorNode()
{
    // Behavior Node�ǉ�
    behaviorTree_->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority, nullptr, nullptr);

    // --------------- ���S ---------------
    behaviorTree_->AddNode("Root", "Death", 0, BehaviorTree::SelectRule::None, new DeathJudgment(this), new DeathAction(this));

    // --------------- ���� ---------------
    behaviorTree_->AddNode("Root", "Flinch", 1, BehaviorTree::SelectRule::None, new FlinchJudgment(this), new FlinchAction(this));

    // --------------- ��퓬 ---------------
    behaviorTree_->AddNode("Root", "NonBattle", 2, BehaviorTree::SelectRule::Priority, new NonBattleJudgment(this), nullptr);
    behaviorTree_->AddNode("NonBattle", "Idle(NonBattle)", 0, BehaviorTree::SelectRule::None, new NonBattleIdleJudgment(this), new NonBattleIdleAction(this));
    behaviorTree_->AddNode("NonBattle", "Walk(NonBattle)", 0, BehaviorTree::SelectRule::None, nullptr, new NonBattleWalkAction(this));

    // --------------- �퓬 ---------------
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

// ----- CollisionData�o�^ -----
void EnemyDragon::RegisterCollisionData()
{
    // �����o������o�^
    CollisionDetectionData collisionDetectionData[] =
    {
        {},
    };

    // ���炢����o�^
    DamageDetectionData damageDetectionData[] =
    {
        {},
    };

    // �U������o�^
    AttackDetectionData attackDetectionData[] =
    {
        {},
    };
}
