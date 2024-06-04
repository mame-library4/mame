#include "Player.h"
#include "PlayerState.h"
#include "Graphics.h"
#include "Camera.h"

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
        GetStateMachine()->RegisterState(new PlayerState::MoveState(this));                 // �ړ�
        //GetStateMachine()->RegisterState(new PlayerState::WalkState(this));                 // ����
        //GetStateMachine()->RegisterState(new PlayerState::RunState(this));                  // ����
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
    // �����ʒu�ݒ�
    GetTransform()->SetPositionZ(60);

    // �X�e�[�W�Ƃ̔���offset�ݒ�
    SetCollisionRadius(0.2f);

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

    // ���x�ݒ�
    SetAcceleration(50.0f);
    SetDeceleration(30.0f);
    SetMaxSpeed(6.0f);

    // ��]���x�ݒ�
    SetRotateSpeed(10.0f);
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

   
    //const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_middle", 0.01f);
    const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_bottom", 0.01f);
    const DirectX::XMFLOAT3 endPos = swordModel_.GetJointPosition("R1:R:j_top", 0.01f);
    swordTrail_.Update(startPos, endPos);

    //GetTransform()->SetPositionY(0.0f);
}

// ----- �`�� -----
void Player::Render(ID3D11PixelShader* psShader)
{
    const float scaleFactor = 0.01f;

    Object::Render(scaleFactor, psShader);
    swordModel_.Render(scaleFactor, psShader);    
}

void Player::RenderTrail()
{
    swordTrail_.Render();
}

// ----- ImGui�p -----
void Player::DrawDebug()
{
    if (ImGui::BeginMenu("Player"))
    {

        Character::DrawDebug();
        Object::DrawDebug();

        ImGui::DragFloat3("offset", &offset_.x);

        ImGui::Checkbox("Collision", &isCollisionSphere_);
        ImGui::Checkbox("Damage", &isDamageSphere_);
        ImGui::Checkbox("Attack", &isAttackSphere_);


        swordTrail_.DrawDebug();
        ImGui::EndMenu();
    }
}

// ----- �f�o�b�O�p -----
void Player::DebugRender(DebugRenderer* debugRenderer)
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition();

    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), GetCollisionRadius(), 2.0f, { 1,1,1,1 });

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
}

// ----- ���񏈗� -----
void Player::Turn(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float aLX = gamePad.GetAxisLX();
    float aLY = gamePad.GetAxisLY();

    const float speed = GetRotateSpeed() * elapsedTime;

    DirectX::XMFLOAT2 input = { fabs(gamePad.GetAxisLX()), fabs(gamePad.GetAxisLY()) };
    DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
    DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();

    moveDirection_ =
    {
        aLY * cameraFront.x + aLX * cameraRight.x,
        0,
        aLY * cameraFront.z + aLX * cameraRight.z,
    };
    moveDirection_ = XMFloat3Normalize(moveDirection_);

    if (input.x > 0.0f || input.y > 0.0f)
    {
        DirectX::XMFLOAT2 cameraForward = { moveDirection_.x, moveDirection_.z };
        cameraForward = XMFloat2Normalize(cameraForward);

        DirectX::XMFLOAT2 playerForward = { GetTransform()->CalcForward().x, GetTransform()->CalcForward().z };
        playerForward = XMFloat2Normalize(playerForward);

        // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
        float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

        // ���ςŉ�]�����Z�o
        float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

        if (forwardCorss > 0)
        {
            GetTransform()->AddRotationY(forwardDot * speed);
        }
        else
        {
            GetTransform()->AddRotationY(-forwardDot * speed);
        }
    }
}

// ----- �ړ����� -----
void Player::Move(const float& elapsedTime)
{
    DirectX::XMFLOAT3 velocity = GetVelocity();
    const float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

    // �ړ����͂��Ȃ��̂Ō�������
    if (fabs(moveDirection_.x) + fabs(moveDirection_.z) <= 0.001f &&
        length != 0.0f)
    {
        const float deceleration = length - GetDeceleration() * elapsedTime;
        if (deceleration < 0.0f)
        {
            velocity = {};
            SetVelocity(velocity);
            ChangeState(Player::STATE::Idle);
            return;
        }

        velocity = XMFloat3Normalize(velocity) * deceleration;
    }
    // ��������
    else
    {
        float accelaration  = GetAcceleration();
        float maxSpeed      = GetMaxSpeed();

#if ActiveWalk
        // �����̏ꍇ �����x,�ړ����x����Ƃ��Ɍ��炷
        if (GetCurrentBlendAnimationIndex() == static_cast<int>(Animation::Walk))
        {
            accelaration *= 0.08f;
            maxSpeed *= 0.3f;
        }
#endif

        velocity.x += moveDirection_.x * accelaration * elapsedTime;
        velocity.z += moveDirection_.z * accelaration * elapsedTime;

        if (length > maxSpeed)
        {
            velocity = XMFloat3Normalize(velocity) * maxSpeed;
        }
    }

    SetVelocity(velocity);
    GetTransform()->AddPosition(velocity * elapsedTime);

    // �A�j���[�V���������������ɂ���
    const float weight = std::min(1.0f, length / GetMaxSpeed());
    SetWeight(weight);
}

// ----- �U���{�^���������ꂽ�� ( �X�e�[�g���ύX���ꂽ�ꍇtrue��Ԃ� ) ( ��s���͂����� ) -----
bool Player::CheckAttackButton(const Player::NextInput& nextInput)
{
    // ��U���̃{�^���������ꂽ�ꍇ
    if(GetLightAttackKeyDown())
    {
        // ��s���͎�t���Ȃ�
        if (nextInput == NextInput::None)
        {
            // �X�e�[�g��ύX���ďI��
            ChangeState(STATE::LightAttack0);
            return true;
        }

        // ��s���͎�t
        if (nextInput == NextInput::LightAttack)
        {
            nextInput_ = static_cast<int>(NextInput::LightAttack);
        }
        // nextInput��StrongAttack�̏ꍇ�A��U���̐�s���͎͂󂯕t���Ȃ�
    }

    // ���U���̃{�^���������ꂽ�ꍇ
    if (GetStrongAttackKeyDown())
    {
        // ��s���͎�t���Ȃ�
        if (nextInput == NextInput::None)
        {
            // �X�e�[�g��ύX���ďI��
            ChangeState(STATE::StrongAttack0);          
            return true;
        }

        // ��s���͎�t
        nextInput_ = static_cast<int>(NextInput::StrongAttack);
    }

    return false;
}

void Player::ResetFlags()
{
    nextInput_ = static_cast<int>(NextInput::None); // ��s���͊Ǘ��t���O
    SetIsAvoidance(false);                          // �����͔���p�t���O
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

void Player::SetAttackFlag(const bool& activeFlag)
{
    for (int i = 0; i < GetAttackDetectionDataCount(); ++i)
    {
        GetAttackDetectionData(i).SetIsActive(activeFlag);
    }
}

bool Player::GetIsActiveAttackFlag()
{
    // ��\�ň�Ԗڂ̎q�̒l��Ԃ�
    return GetAttackDetectionData(0).GetIsActive();
}
