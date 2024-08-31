#include "Player.h"
#include "PlayerState.h"
#include "Graphics.h"
#include "Camera.h"

#include "Effect/EffectManager.h"

// ----- �R���X�g���N�^ -----
Player::Player()
    : Character("./Resources/Model/Character/Player/SwordGirl.gltf", 0.01f),
    weapon_("./Resources/Model/Character/Sword/Sword.gltf")
{
    // --- �X�e�[�g�}�V�� ---
    {
        stateMachine_.reset(new StateMachine<State<Player>>);

        // �X�e�[�g��o�^����
        GetStateMachine()->RegisterState(new PlayerState::IdleState(this));             // �ҋ@
        //GetStateMachine()->RegisterState(new PlayerState::MoveState(this));             // �ړ�
        GetStateMachine()->RegisterState(new PlayerState::WalkState(this));
        GetStateMachine()->RegisterState(new PlayerState::RunState(this));
        GetStateMachine()->RegisterState(new PlayerState::LightFlinchState(this));
        GetStateMachine()->RegisterState(new PlayerState::FlinchState(this));
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));           // �_���[�W
        GetStateMachine()->RegisterState(new PlayerState::DeathState(this));            // ���S
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));        // ���
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));          // �J�E���^�[
        GetStateMachine()->RegisterState(new PlayerState::CounterComboState(this));     // �J�E���^�[�R���{
        GetStateMachine()->RegisterState(new PlayerState::RunAttackState(this));        // �R���{0_3
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_0(this));        // �R���{0_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_1(this));        // �R���{0_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_2(this));        // �R���{0_2
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_3(this));        // �R���{0_3

        // ��ԏ��߂̃X�e�[�g��ݒ肷��
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
        currentState_ = STATE::Idle;
    }

    // CollisionData�o�^
    RegisterCollisionData();

    // LookAt������
    //LookAtInitilaize("head");
}

// ----- �f�X�g���N�^ -----
Player::~Player()
{
}

// ----- ������ -----
void Player::Initialize()
{
    // ���[�g���[�V�����̈ړ��l������
    SetUseRootMotionMovement(false);

    // �����ʒu�ݒ�
    GetTransform()->SetPositionZ(60);

    // �T�C�Y�ݒ�
    GetTransform()->SetScaleFactor(0.8f);

    // �X�e�[�W�Ƃ̔���offset�ݒ�
    SetCollisionRadius(0.2f);

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
    // �X�e�[�g�}�V���X�V
    GetStateMachine()->Update(elapsedTime);
    
    // �A�j���[�V�����X�V [���X�e�[�g�}�V���X�V��]
    Character::Update(elapsedTime);

    // ���[�g���[�V����
    RootMotion();

    // TODO:�e�X�g�p
    if (GetHealth() <= 0.0f)
    {
        if (GetCurrentState() != STATE::Death)
        {
            ChangeState(STATE::Death);
        }

        // ���̍��W�X�V
        UpdateSwordTransform();

        return;
    }

    // �ړ�����
    Move(elapsedTime);    

    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    CollisionCharacterVsStage();
   
    //const DirectX::XMFLOAT3 startPos = GetJointPosition("R1:R:j_middle", 0.01f);
    //const DirectX::XMFLOAT3 startPos = GetJointPosition("index_01_r", GetScaleFactor()) + DirectX::XMFLOAT3(0, 0, 10);
    //const DirectX::XMFLOAT3 endPos = GetJointPosition("index_01_r", GetScaleFactor());
    //swordTrail_.Update(startPos, endPos);


    // ���̍��W�X�V
    UpdateSwordTransform();

    // Collision�f�[�^�X�V
    UpdateCollisions(elapsedTime);
}

// ----- �`�� -----
void Player::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
    weapon_.Render(weaponWorld_, psShader);
}

void Player::RenderTrail()
{
    //swordTrail_.Render();
}

// ----- ImGui�p -----
void Player::DrawDebug()
{
    if (ImGui::BeginMenu("Player"))
    {
        if (ImGui::TreeNode("Weapon"))
        {
            weapon_.DrawDebug();

            ImGui::DragFloat3("weaponLocation", &socketLocation_.x);
            ImGui::DragFloat3("weaponRotation", &socketRotation_.x);
            ImGui::DragFloat3("socketScale", &socketScale_.x);

            ImGui::TreePop();
        }

        GetStateMachine()->DrawDebug();

        Character::DrawDebug();
        Object::DrawDebug();

        ImGui::Checkbox("Collision", &isCollisionSphere_);
        ImGui::Checkbox("Damage", &isDamageSphere_);
        ImGui::Checkbox("Attack", &isAttackSphere_);

        ImGui::DragFloat("CounterActiveRadius", &counterActiveRadius_);

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
            const DirectX::XMFLOAT4 color = isAbleAttack_ ? data.GetColor() : DirectX::XMFLOAT4(1, 0, 1, 1);

            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), color);
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
        
    DirectX::XMFLOAT2 input = { fabsf(gamePad.GetAxisLX()), fabsf(gamePad.GetAxisLY()) };
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
        float forwardCross = XMFloat2Cross(cameraForward, playerForward);

        // ���ςŉ�]�����Z�o
        float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

        if (forwardDot > -0.01f) return;

        // -2.0 ~ 0.0;
        const float speed = GetRotateSpeed() * elapsedTime;
        float rotateY = forwardDot * speed;      
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCross > 0)
        {
            GetTransform()->AddRotationY(rotateY);
        }
        else
        {
            GetTransform()->AddRotationY(-rotateY);
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
        //ChangeState(Player::STATE::Idle);
        const float deceleration = length - GetDeceleration() * elapsedTime;
        if (deceleration < 0.0f)
        {
            velocity = {};
            SetVelocity(velocity);
            return;
        }

        velocity = XMFloat3Normalize(velocity) * deceleration;
    }
    // ��������
    else
    {
        float accelaration  = GetAcceleration();
        float maxSpeed      = GetMaxSpeed();

        velocity.x += moveDirection_.x * accelaration * elapsedTime;
        velocity.z += moveDirection_.z * accelaration * elapsedTime;

        if (length > maxSpeed)
        {
            velocity = XMFloat3Normalize(velocity) * maxSpeed;
        }
    }

    SetVelocity(velocity);
    GetTransform()->AddPosition(velocity * elapsedTime);
}

// ----- ��s���͂���t���Ă� -----
bool Player::CheckNextInput(const Player::NextInput& nextInput)
{
    // �����͂���������
    if (GetAvoidanceKeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::Avoidance);
            return true;
        }

        nextInput_ = NextInput::Avoidance;
    }

    // �J�E���^�[��t
    if (GetCounterStanceKey())
    {
        ChangeState(STATE::Counter);
        return true;
    }

    // �R���{�U��0
    if (GetComboAttack0KeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::ComboAttack0_0);
            return true;
        }
    }

    return false;
}

void Player::ResetFlags()
{
    nextInput_              = NextInput::None;  // ��s���͊Ǘ��t���O
    isAvoidance_            = false;            // �����͔���p�t���O
    isCounter_              = false;            // �J�E���^�[��Ԃ��t���O
    isAbleCounterAttack_    = false;            // �J�E���^�[�U���\��
    isAbleAttack_           = false;            // �U���ł��邩�̃t���O
}

// ----- CollisionData�o�^ -----
void Player::RegisterCollisionData()
{
    // �����o������o�^
    CollisionDetectionData collisionDetectionData[] =
    {
        { "head",       0.2f },
        { "spine_02",   0.2f },
        { "pelvis",     0.2f },
        { "collide",    0.2f, true, { 0.0f, 0.25f, 0.0f } },
    };
    for (int i = 0; i < _countof(collisionDetectionData); ++i)
    {
        RegisterCollisionDetectionData(collisionDetectionData[i]);
    }

    // ���炢����o�^
    DamageDetectionData damageDetectionData[] =
    {
        { "head",       0.2f, {} },
        { "spine_02",   0.2f, {} },
        { "pelvis",     0.2f, {} },
        
        { "hand_r",     0.05f, 0.0f, {-50, -13, 50} },
    };
    for (int i = 0; i < _countof(damageDetectionData); ++i)
    {
        RegisterDamageDetectionData(damageDetectionData[i]);
    }

    // �U������o�^
    AttackDetectionData attackDetectionData[] =
    {
        { "index_01_r_add0", 0.2f, { 0, 0, 0 },             "index_01_r" },
        { "index_01_r_add1", 0.2f, { 0, -7.0f, 50.0f },     "index_01_r" },
        { "index_01_r_add2", 0.2f, { 0, -15.0f, 100.0f },   "index_01_r" },
        //{ "index_01_r_add3", 0.2f, { 0, -23.0f, 150.0f },   "index_01_r" },
    };
    for (int i = 0; i < _countof(attackDetectionData); ++i)
    {
        RegisterAttackDetectionData(attackDetectionData[i]);
    }
}

// ----- ���̍��W�X�V -----
void Player::UpdateSwordTransform()
{
    const float toRadian = 0.01745f;
    const float toMetric = 0.01f;
    const int weaponJointIndex = GetNodeIndex("hand_r");
    const GltfModel::Node node = GetNodes()->at(weaponJointIndex);

    DirectX::XMMATRIX boneTransform = DirectX::XMLoadFloat4x4(&node.globalTransform_);
    DirectX::XMMATRIX socketTransform = DirectX::XMMatrixScaling(socketScale_.x, socketScale_.y, socketScale_.z)
        * DirectX::XMMatrixRotationX(-socketRotation_.x * toRadian)
        * DirectX::XMMatrixRotationX(-socketRotation_.y * toRadian)
        * DirectX::XMMatrixRotationX(socketRotation_.z * toRadian)
        * DirectX::XMMatrixTranslation(socketLocation_.x * toMetric, socketLocation_.y * toMetric, socketLocation_.z * toMetric);
    DirectX::XMMATRIX dxUE5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    DirectX::XMMATRIX UE5Gltf = DirectX::XMMatrixSet(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Z-Up Y-Forward(UE5) -> RHS Y-Up Z-Forward(glTF) 
    DirectX::XMStoreFloat4x4(&weaponWorld_, dxUE5 * socketTransform * UE5Gltf * boneTransform * GetTransform()->CalcWorldMatrix(GetScaleFactor()));
}

void Player::UpdateCollisions(const float& elapsedTime)
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

    // �����o������X�V
    UpdateCollisionDetectionData();
}

// ----- �����o������ʒu�X�V -----
void Player::UpdateCollisionDetectionData()
{
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), data.GetOffsetPosition());

        if (data.GetFixedY()) pos.y = 0.0f;

        data.SetJointPosition(pos);
    }
    GetCollisionDetectionData("collide").SetPosition(GetTransform()->GetPosition());
}

// ----- �X�e�[�g�ύX -----
void Player::ChangeState(const STATE& state)
{
    // �O��̃X�e�[�g���L�^
    oldState_ = currentState_;

    // ���݂̃X�e�[�g���L�^
    currentState_ = state;

    stateMachine_.get()->ChangeState(static_cast<int>(state));
}
