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
        GetStateMachine()->RegisterState(new PlayerState::FlinchState(this));
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));           // ダメージ
        GetStateMachine()->RegisterState(new PlayerState::DeathState(this));            // 死亡
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));        // 回避
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));          // カウンター
        GetStateMachine()->RegisterState(new PlayerState::CounterComboState(this));     // カウンターコンボ
        GetStateMachine()->RegisterState(new PlayerState::RunAttackState(this));        // コンボ0_3
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_0(this));        // コンボ0_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_1(this));        // コンボ0_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_2(this));        // コンボ0_2
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_3(this));        // コンボ0_3

        // 一番初めのステートを設定する
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }

    // CollisionData登録
    RegisterCollisionData();

    // LookAt初期化
    //LookAtInitilaize("head");
}

// ----- デストラクタ -----
Player::~Player()
{
}

// ----- 初期化 -----
void Player::Initialize()
{
    // ルートモーションの移動値を消す
    SetUseRootMotionMovement(false);

    // 生成位置設定
    GetTransform()->SetPositionZ(60);

    // サイズ設定
    GetTransform()->SetScaleFactor(0.8f);

    // ステージとの判定offset設定
    SetCollisionRadius(0.2f);

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
    // ステートマシン更新
    GetStateMachine()->Update(elapsedTime);
    
    // アニメーション更新 [※ステートマシン更新後]
    Character::Update(elapsedTime);

    // 移動処理
    Move(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();
   
    //const DirectX::XMFLOAT3 startPos = GetJointPosition("R1:R:j_middle", 0.01f);
    //const DirectX::XMFLOAT3 startPos = GetJointPosition("index_01_r", GetScaleFactor()) + DirectX::XMFLOAT3(0, 0, 10);
    //const DirectX::XMFLOAT3 endPos = GetJointPosition("index_01_r", GetScaleFactor());
    //swordTrail_.Update(startPos, endPos);


    // 剣の座標更新
    UpdateSwordTransform();

    // Collisionデータ更新
    UpdateCollisions(elapsedTime);
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

// ----- 旋回処理 -----
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

        // 外積をしてどちらに回転するのかを判定する
        float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

        // 内積で回転幅を算出
        float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

        if (forwardDot > -0.01f) return;

        // -2.0 ~ 0.0;
        const float speed = GetRotateSpeed() * elapsedTime;
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
bool Player::CheckNextInput(const Player::NextInput& nextInput)
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

    // カウンター受付
    if (GetCounterStanceKey())
    {
        ChangeState(STATE::Counter);
        return true;
    }

    // コンボ攻撃0
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
    nextInput_              = NextInput::None;  // 先行入力管理フラグ
    isAvoidance_            = false;            // 回避入力判定用フラグ
    isCounter_              = false;            // カウンター状態かフラグ
    isAbleCounterAttack_    = false;            // カウンター攻撃可能か
    isAbleAttack_           = false;            // 攻撃できるかのフラグ
}

// ----- CollisionData登録 -----
void Player::RegisterCollisionData()
{
    // 押し出し判定登録
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

    // くらい判定登録
    DamageDetectionData damageDetectionData[] =
    {
        { "head",       0.2f, {} },
        { "spine_02",   0.2f, {} },
        { "pelvis",     0.2f, {} },
    };
    for (int i = 0; i < _countof(damageDetectionData); ++i)
    {
        RegisterDamageDetectionData(damageDetectionData[i]);
    }

    // 攻撃判定登録
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

// ----- 剣の座標更新 -----
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
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition()));
    }

    // 押し出し判定更新
    UpdateCollisionDetectionData();
}

// ----- 押し出し判定位置更新 -----
void Player::UpdateCollisionDetectionData()
{
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        DirectX::XMFLOAT3 pos = GetJointPosition(data.GetUpdateName(), GetScaleFactor(), data.GetOffsetPosition());

        if (data.GetFixedY()) pos.y = 0.0f;

        data.SetJointPosition(pos);
    }
    GetCollisionDetectionData("collide").SetPosition(GetTransform()->GetPosition());
}

// ----- ステート変更 -----
void Player::ChangeState(const STATE& state)
{
    // 現在のステートを記録
    currentState_ = state;

    stateMachine_.get()->ChangeState(static_cast<int>(state));
}
