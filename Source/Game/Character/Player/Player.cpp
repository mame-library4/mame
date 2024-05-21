#include "Player.h"
#include "PlayerState.h"
#include "../../../Graphics/Graphics.h"

// ----- �R���X�g���N�^ -----
Player::Player()
    : Character("./Resources/Model/Character/Player.glb"),
    swordModel_("./Resources/Model/Character/Sword.glb")
{
    // --- �X�e�[�g�}�V�� ---
    {
        stateMachine_.reset(new StateMachine<State<Player>>);

        // �X�e�[�g��o�^����
        GetStateMachine()->RegisterState(new PlayerState::IdleState(this));                 // �ҋ@
        GetStateMachine()->RegisterState(new PlayerState::WalkState(this));                 // ����
        GetStateMachine()->RegisterState(new PlayerState::RunState(this));                  // ����
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));            // ���
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));              // �J�E���^�[
        GetStateMachine()->RegisterState(new PlayerState::CounterAttackState(this));        // �J�E���^�[�U��
        GetStateMachine()->RegisterState(new PlayerState::LightAttack0State(this));         // ��U��0
        GetStateMachine()->RegisterState(new PlayerState::LightAttack1State(this));         // ��U��1
        GetStateMachine()->RegisterState(new PlayerState::LightAttack2State(this));         // ��U��2
        GetStateMachine()->RegisterState(new PlayerState::StrongAttack0State(this));        // ���U��0
        GetStateMachine()->RegisterState(new PlayerState::StrongAttack1State(this));        // ���U��1       
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));               // �_���[�W

        // ��ԏ��߂̃X�e�[�g��ݒ肷��
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }
}

// ----- �f�X�g���N�^ -----
Player::~Player()
{
}

// ----- ������ -----
void Player::Initialize()
{
    // �����o������p�ϐ��ݒ�
    RegisterCollisionDetectionData({ "collide0", 0.25f, { 0, 1.3f, 0} });
    RegisterCollisionDetectionData({ "collide1", 0.25f, { 0, 0.8f, 0} });
    RegisterCollisionDetectionData({ "collide2", 0.25f, { 0, 0.3f, 0} });

    // ���炢����
    RegisterDamageDetectionData({ "R:R:j_Head_end", 0.15f,  10, { -8, 2, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Spine2",   0.2f,   10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Hips",     0.2f,   10, { 10, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Reg_R",    0.15f,  10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Leg_L",    0.15f,  10, { 0, 0, 0 } });

    // �U������
    RegisterAttackDetectionData({ "R1:R:j_Sword", 0.15f, { 0, 35, 0} });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -35, 0 } });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -5, 0 } });

    // �̗͐ݒ�
    SetMaxHealth(100.0f);
    SetHealth(GetMaxHealth());
}

void Player::Finalize()
{
}

// ----- �X�V -----
void Player::Update(const float& elapsedTime)
{
    swordModel_.GetTransform()->SetPosition(GetTransform()->GetPosition());
    swordModel_.GetTransform()->SetRotation(GetTransform()->GetRotation());
    
    // Collision�f�[�^�X�V
    UpdateCollisions(elapsedTime, 0.01f);
    GetCollisionDetectionData("collide0").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide1").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide2").SetPosition(GetTransform()->GetPosition());

    // �A�j���[�V�����X�V
    Character::Update(elapsedTime);
    swordModel_.UpdateAnimation(elapsedTime);

    Camera::Instance().SetTarget(GetTransform()->GetPosition() + offset_);

    GetStateMachine()->Update(elapsedTime);

    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    CollisionCharacterVsStage();

    //GetTransform()->SetPositionY(0.0f);
}

// ----- �`�� -----
void Player::Render()
{
    const float scaleFactor = 0.01f;

    Object::Render(scaleFactor);
    swordModel_.Render(scaleFactor);
}

// ----- ImGui�p -----
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

// ----- �f�o�b�O�p -----
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
    // ��s���͎�t
// TODO:��s���͂̎�t�̐�����݂���B
    if (GetLightAttackKeyDown()) SetNextInput(Player::NextInput::LightAttack);
    if (GetStrongAttackKeyDown()) SetNextInput(Player::NextInput::StrongAttack);

    // �A�j���[�V�����Đ���
    if (IsPlayAnimation()) return;

    // ��s���͂��������B
    if (GetNextInput())
    {
        switch (state)
        {
        case Player::STATE::LightAttack0:
        case Player::STATE::LightAttack1:
        case Player::STATE::LightAttack2:
            // X�{�^���������ꂽ�ꍇ
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(state);
            }
            // Y�{�^���������ꂽ�ꍇ
            else
            {
                // TODO: �A�j���[�V����������܂ŏd�U���͕��u
                break;
                ChangeState(Player::STATE::StrongAttack0);
            }
            break;
            // Y
        case Player::STATE::StrongAttack0:
        case Player::STATE::StrongAttack1:
            // TODO: �A�j���[�V����������܂ŏd�U���͕��u
            break;

            // X�{�^���������ꂽ�ꍇ
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(Player::STATE::LightAttack0);
            }
            // Y�{�^���������ꂽ�ꍇ
            else
            {
                ChangeState(state);
            }
            break;
        default:
            // X�{�^���������ꂽ�ꍇ
            if (GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                ChangeState(Player::STATE::LightAttack0);
            }
            // ����ȊO
            else
            {
                ChangeState(state);
            }
            break;
        }

        return;
    }

    // ��s���͂��Ȃ������B( �ҋ@�X�e�[�g�֑J�� )
    ChangeState(STATE::Idle);
    return;
}

void Player::ResetFlags()
{
    SetNextInput(Player::NextInput::None);  // ��s���͊Ǘ��t���O
    SetIsAvoidance(false);                  // �����͔���p�t���O
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
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // �U������X�V
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(swordModel_.GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // �����o������X�V
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
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

    // weight��0~1�̊ԂɎ��߂�
    float w = swordModel_.GetWeight();
    w += weight;
    w = std::clamp(w, 0.0f, 1.0f);
    swordModel_.SetWeight(w);
}
