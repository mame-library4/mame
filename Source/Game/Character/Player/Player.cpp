#include "Player.h"
#include "PlayerState.h"
#include "../../../Graphics/Graphics.h"

// ----- �R���X�g���N�^ -----
Player::Player()
    : Character("./Resources/Model/Character/Player.glb")
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
    RegisterCollisionDetectionData({ "collide", 0.25f });

    // �����蔻��p�ݒ�
    RegisterDamageDetectionData({ "head", 0.15f, 10, { 0, 1.4f, 0 } });
    RegisterDamageDetectionData({ "body", 0.25f, 10, { 0, 1.0f, 0 } });
    RegisterDamageDetectionData({ "foot", 0.25f, 10, { 0, 0.5f, 0 } });

    // �̗͐ݒ�
    SetMaxHealth(100.0f);
    SetHealth(GetMaxHealth());

    Object::PlayAnimation(animationIndex_, true, speed_);
}

void Player::Finalize()
{
}

// ----- �X�V -----
void Player::Update(const float& elapsedTime)
{
    GetCollisionDetectionData("collide").SetJointPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("head").SetPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("body").SetPosition(GetTransform()->GetPosition());
    GetDamageDetectionData("foot").SetPosition(GetTransform()->GetPosition());

    Character::Update(elapsedTime);

    Camera::Instance().SetTarget(GetTransform()->GetPosition() + offset_);

    GetStateMachine()->Update(elapsedTime);

    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    CollisionCharacterVsStage();

    //GetTransform()->SetPositionY(0.0f);
}

// ----- �`�� -----
void Player::Render()
{
    Object::Render(0.01f);
}

// ----- ImGui�p -----
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
                ChangeState(Player::STATE::StrongAttack0);
            }
            break;
            // Y
        case Player::STATE::StrongAttack0:
        case Player::STATE::StrongAttack1:
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
