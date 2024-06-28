#include "Player.h"
#include "PlayerState.h"
#include "Graphics.h"
#include "Camera.h"

// ----- コンストラクタ -----
Player::Player()
    : Character("./Resources/Model/Character/Player/SwordGirl.gltf", 0.01f),
    weapon_("./Resources/Model/Character/Sword/Sword.gltf")
{
    // --- ステートマシン ---
    {
        stateMachine_.reset(new StateMachine<State<Player>>);

        // ステートを登録する
        GetStateMachine()->RegisterState(new PlayerState::IdleState(this));             // 待機
        //GetStateMachine()->RegisterState(new PlayerState::MoveState(this));             // 移動
        GetStateMachine()->RegisterState(new PlayerState::WalkState(this));
        GetStateMachine()->RegisterState(new PlayerState::RunState(this));
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));           // ダメージ
        GetStateMachine()->RegisterState(new PlayerState::DeathState(this));            // 死亡
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));        // 回避
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));          // カウンター
        GetStateMachine()->RegisterState(new PlayerState::CounterComboState(this));     // カウンターコンボ
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_0(this));        // コンボ0_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_1(this));        // コンボ0_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_2(this));        // コンボ0_2
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_3(this));        // コンボ0_3

        // 一番初めのステートを設定する
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }

    // LookAt初期化
    //LookAtInitilaize("Head");
}

// ----- デストラクタ -----
Player::~Player()
{
}

// ----- 初期化 -----
void Player::Initialize()
{
    // 生成位置設定
    GetTransform()->SetPositionZ(60);

    // サイズ設定
    GetTransform()->SetScaleFactor(0.8f);

    // ステージとの判定offset設定
    SetCollisionRadius(0.2f);

    // 押し出し判定用変数設定
    RegisterCollisionDetectionData({ "collide0", 0.25f, { 0, 1.3f, 0} });
    RegisterCollisionDetectionData({ "collide1", 0.25f, { 0, 0.8f, 0} });
    RegisterCollisionDetectionData({ "collide2", 0.25f, { 0, 0.3f, 0} });

    // くらい判定
    RegisterDamageDetectionData({ "R:R:j_Head_end", 0.15f,  10, { -8, 2, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Spine2",   0.2f,   10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Hips",     0.2f,   10, { 10, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Reg_R",    0.15f,  10, { 0, 0, 0 } });
    RegisterDamageDetectionData({ "R:R:j_Leg_L",    0.15f,  10, { 0, 0, 0 } });

    // 攻撃判定
    RegisterAttackDetectionData({ "R1:R:j_Sword", 0.15f, { 0, 35, 0} });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -35, 0 } });
    RegisterAttackDetectionData({ "R1:R:j_Sword_end", 0.15f, { 0, -5, 0 } });

    // 体力設定
    SetMaxHealth(100.0f);
    SetHealth(GetMaxHealth());

    // 速度設定
    SetAcceleration(50.0f);
    SetDeceleration(30.0f);
    SetMaxSpeed(6.0f);

    // 回転速度設定
    SetRotateSpeed(10.0f);
}

void Player::Finalize()
{
}

// ----- 更新 -----
void Player::Update(const float& elapsedTime)
{    
    // Collisionデータ更新
    UpdateCollisions(elapsedTime);
    GetCollisionDetectionData("collide0").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide1").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide2").SetPosition(GetTransform()->GetPosition());

    // ステートマシン更新
    GetStateMachine()->Update(elapsedTime);
    
    // アニメーション更新 [※ステートマシン更新後]
    Character::Update(elapsedTime);
    

    // 移動処理
    Move(elapsedTime);

    // ステージの外に出ないようにする
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

// ----- 描画 -----
void Player::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);

    weapon_.Render(weaponWorld_, psShader);
}

void Player::RenderTrail()
{
    //swordTrail_.Render();
}

// ----- ImGui用 -----
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

// ----- デバッグ用 -----
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

// ----- 旋回処理 -----
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

        // 外積をしてどちらに回転するのかを判定する
        float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

        // 内積で回転幅を算出
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

// ----- 移動処理 -----
void Player::Move(const float& elapsedTime)
{
    DirectX::XMFLOAT3 velocity = GetVelocity();
    const float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

    // 移動入力がないので減速処理
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
    // 加速処理
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

// ----- 先行入力を受付してる -----
bool Player::CheckNextInput(const Player::NextInput& nextInput, const float& nextAttackFrame)
{
    // 回避入力があった時
    if (GetAvoidanceKeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::Avoidance);
            return true;
        }

        nextInput_ = NextInput::Avoidance;
    }

    // コンボ攻撃0
    if (GetComboAttack0KeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::ComboAttack0_0);
            return true;
        }

        // 先行入力の種類がコンボ攻撃0の場合
        if (nextInput == NextInput::ComboAttack0)
        {
            // 先行入力がAttackFrameよりも前に行われた
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

    // コンボ攻撃1
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
    nextInput_ = NextInput::None; // 先行入力管理フラグ
    SetIsAvoidance(false);                          // 回避入力判定用フラグ
}

void Player::UpdateCollisions(const float& elapsedTime)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        //data.SetJointPosition(swordModel_.GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), GetScaleFactor(), data.GetOffsetPosition()));
    }
}

// ----- RootMotion更新 -----
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
    // 代表で一番目の子の値を返す
    return GetAttackDetectionData(0).GetIsActive();
}

// ----- ステート変更 -----
void Player::ChangeState(const STATE& state)
{
    // 現在ブレンドアニメーション中なのでステート変更を行わない
    if (GetIsBlendAnimation()) return;

    stateMachine_.get()->ChangeState(static_cast<int>(state));
}
