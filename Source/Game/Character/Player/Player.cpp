#include "Player.h"
#include "PlayerState.h"
#include "Graphics.h"
#include "Camera.h"

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
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));           // �_���[�W
        GetStateMachine()->RegisterState(new PlayerState::DeathState(this));            // ���S
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));        // ���
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));          // �J�E���^�[
        GetStateMachine()->RegisterState(new PlayerState::CounterComboState(this));     // �J�E���^�[�R���{
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_0(this));        // �R���{0_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_1(this));        // �R���{0_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_2(this));        // �R���{0_2
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_3(this));        // �R���{0_3

        // ��ԏ��߂̃X�e�[�g��ݒ肷��
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }

    // LookAt������
    //LookAtInitilaize("Head");
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

    // �T�C�Y�ݒ�
    GetTransform()->SetScaleFactor(0.8f);

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
    // Collision�f�[�^�X�V
    UpdateCollisions(elapsedTime);
    GetCollisionDetectionData("collide0").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide1").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide2").SetPosition(GetTransform()->GetPosition());

    // �X�e�[�g�}�V���X�V
    GetStateMachine()->Update(elapsedTime);
    
    // �A�j���[�V�����X�V [���X�e�[�g�}�V���X�V��]
    Character::Update(elapsedTime);
    

    // �ړ�����
    Move(elapsedTime);

    // �X�e�[�W�̊O�ɏo�Ȃ��悤�ɂ���
    CollisionCharacterVsStage();

    UpdateRootMotion();
   
    //const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_middle", 0.01f);
    //const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_bottom", 0.01f);
    //const DirectX::XMFLOAT3 endPos = swordModel_.GetJointPosition("R1:R:j_top", 0.01f);
    //swordTrail_.Update(startPos, endPos);



    //const float toRadian = 0.01745f;
    //const float toMetric = 0.01f;
    //const int weaponJointIndex = GetNodeIndex("MaceJoint");
    //const GltfModel::Node node = GetNodes()->at(weaponJointIndex);
    //
    //DirectX::XMMATRIX boneTransform = DirectX::XMLoadFloat4x4(&node.globalTransform_);
    //DirectX::XMMATRIX socketTransform = DirectX::XMMatrixScaling(socketScale_.x, socketScale_.y, socketScale_.z)
    //    * DirectX::XMMatrixRotationX(-socketRotation_.x * toRadian)
    //    * DirectX::XMMatrixRotationX(-socketRotation_.y * toRadian)
    //    * DirectX::XMMatrixRotationX(socketRotation_.z * toRadian)
    //    * DirectX::XMMatrixTranslation(socketLocation_.x * toMetric, socketLocation_.y * toMetric, socketLocation_.z * toMetric);
    //DirectX::XMMATRIX dxUE5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    //DirectX::XMMATRIX UE5Gltf = DirectX::XMMatrixSet(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Z-Up Y-Forward(UE5) -> RHS Y-Up Z-Forward(glTF) 
    //DirectX::XMStoreFloat4x4(&weaponWorld_, dxUE5 * socketTransform * UE5Gltf * boneTransform * GetTransform()->CalcWorldMatrix(GetScaleFactor()));


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

        ImGui::DragFloat("SlowSpeed", &slowAnimationSpeed_, 0.1f);

        Character::DrawDebug();
        Object::DrawDebug();

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
        float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

        // ���ςŉ�]�����Z�o
        float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

        if (forwardDot > -0.01f) return;

        // -2.0 ~ 0.0;
        float rotateY = forwardDot * speed;      
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCorss > 0)
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
bool Player::CheckNextInput(const Player::NextInput& nextInput, const float& nextAttackFrame)
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

    // �R���{�U��0
    if (GetComboAttack0KeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::ComboAttack0_0);
            return true;
        }

        // ��s���͂̎�ނ��R���{�U��0�̏ꍇ
        if (nextInput == NextInput::ComboAttack0)
        {
            // ��s���͂�AttackFrame�����O�ɍs��ꂽ
            if (GetAnimationSeconds() < nextAttackFrame &&
                nextInput_ != NextInput::ComboAttack0)
            {
                SetUseBlendAnimation(false);
            }
            else
            {
                SetUseBlendAnimation(true);
            }

            nextInput_ = NextInput::ComboAttack0;        
            return true;
        }

        nextInput_ = NextInput::ComboAttack0;
    }

    // �R���{�U��1
    if (GetComboAttack1KeyDown())
    {
        return false;
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::ComboAttack1_0);
            return true;
        }

        nextInput_ = NextInput::ComboAttack1;
    }

    return false;
}

void Player::ResetFlags()
{
    nextInput_ = NextInput::None; // ��s���͊Ǘ��t���O
    SetIsAvoidance(false);                          // �����͔���p�t���O
}

void Player::UpdateCollisions(const float& elapsedTime)
{
    // ���炢����X�V
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // �U������X�V
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        //data.SetJointPosition(swordModel_.GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));
    }
    // �����o������X�V
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // �W���C���g�̖��O�ňʒu�ݒ� ( ���O���W���C���g�̖��O�ł͂Ȃ��Ƃ��ʓr�X�V�K�v )
        data.SetJointPosition(GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));
    }
}

// ----- RootMotion�X�V -----
void Player::UpdateRootMotion()
{
    const Animation animationIndex = static_cast<Animation>(GetAnimationIndex());
    if (animationIndex == Animation::Idle ||
        animationIndex == Animation::Walk ||
        animationIndex == Animation::Run)
    {

    }
    else
    {
        Character::UpdateRootMotion();
    }
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

// ----- �X�e�[�g�ύX -----
void Player::ChangeState(const STATE& state)
{
    // ���݃u�����h�A�j���[�V�������Ȃ̂ŃX�e�[�g�ύX���s��Ȃ�
    if (GetIsBlendAnimation()) return;

    stateMachine_.get()->ChangeState(static_cast<int>(state));
}
