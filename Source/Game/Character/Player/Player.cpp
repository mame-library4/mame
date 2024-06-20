#include "Player.h"
#include "PlayerState.h"
#include "Graphics.h"
#include "Camera.h"

#define USE_ROOT_MOTION 0

// ----- コンストラクタ -----
Player::Player()
#if USE_ROOT_MOTION
    : Character("./Resources/Model/Character/aaa.glb")
#else
    : Character("./Resources/Model/Character/Player/Orc.gltf")
#endif
{
    // --- ステートマシン ---
    {
        stateMachine_.reset(new StateMachine<State<Player>>);

        // ステートを登録する
        GetStateMachine()->RegisterState(new PlayerState::IdleState(this));             // 待機
        GetStateMachine()->RegisterState(new PlayerState::MoveState(this));             // 移動
        GetStateMachine()->RegisterState(new PlayerState::DamageState(this));           // ダメージ
        GetStateMachine()->RegisterState(new PlayerState::DeathState(this));            // 死亡
        GetStateMachine()->RegisterState(new PlayerState::AvoidanceState(this));        // 回避
        GetStateMachine()->RegisterState(new PlayerState::CounterState(this));          // カウンター
        GetStateMachine()->RegisterState(new PlayerState::CounterComboState(this));     // カウンターコンボ
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_0(this));        // コンボ0_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_1(this));        // コンボ0_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_2(this));        // コンボ0_2
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack0_3(this));        // コンボ0_3
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack1_0(this));        // コンボ1_0
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack1_1(this));        // コンボ1_1
        GetStateMachine()->RegisterState(new PlayerState::ComboAttack1_2(this));        // コンボ1_2

        // 一番初めのステートを設定する
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
    }

    // -------------------- LookAt処理 -------------------------
    // ----- (初期姿勢時の頭ノードのローカル空間前方向を求める) -----
    LookAtInitilaize("R:R:j_Head");
    //LookAtInitilaize("R:R:j_Head_end");
}

// ----- デストラクタ -----
Player::~Player()
{
}

// ----- 初期化 -----
void Player::Initialize()
{
#if USE_ROOT_MOTION
    // RootMotion
    RootMotionInitialize();
#endif

    // 生成位置設定
    GetTransform()->SetPositionZ(60);

    // サイズ設定
    GetTransform()->SetScaleFactor(0.75f);

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
    UpdateCollisions(elapsedTime, 0.01f);
    GetCollisionDetectionData("collide0").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide1").SetPosition(GetTransform()->GetPosition());
    GetCollisionDetectionData("collide2").SetPosition(GetTransform()->GetPosition());

    // アニメーション更新
    Character::Update(elapsedTime);
    
#if USE_ROOT_MOTION
    // RootMotion
    RootMotionUpdate(elapsedTime, "mixamorig:Hips");
    //RootMotionUpdate(elapsedTime, "root");
#endif

    

    GetStateMachine()->Update(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();

   
    //const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_middle", 0.01f);
    //const DirectX::XMFLOAT3 startPos = swordModel_.GetJointPosition("R1:R:j_bottom", 0.01f);
    //const DirectX::XMFLOAT3 endPos = swordModel_.GetJointPosition("R1:R:j_top", 0.01f);
    //swordTrail_.Update(startPos, endPos);




    // LookAt
    LookAtUpdate();
}

// ----- 描画 -----
void Player::Render(ID3D11PixelShader* psShader)
{
    const float scaleFactor = 1.0f;

    Object::Render(scaleFactor, psShader);
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

    // アニメーションをいい感じにする
    const float weight = std::min(1.0f, length / GetMaxSpeed());
    SetWeight(weight);
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

    if (GetComboAttack0KeyDown())
    {
        if (nextInput == NextInput::None)
        {
            ChangeState(STATE::ComboAttack0_0);
            return true;
        }

        nextInput_ = NextInput::ComboAttack0;
    }

    if (GetComboAttack1KeyDown())
    {
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

void Player::PlayBlendAnimation(const Animation& index1, const Animation& index2, const bool& loop, const float& speed)
{
    Object::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed);
}

void Player::PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed)
{    
    Object::PlayBlendAnimation(static_cast<int>(index), loop, speed);
}

void Player::UpdateCollisions(const float& elapsedTime, const float& scaleFactor)
{
    // くらい判定更新
    for (DamageDetectionData& data : damageDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));

        data.Update(elapsedTime);
    }
    // 攻撃判定更新
    for (AttackDetectionData& data : attackDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        //data.SetJointPosition(swordModel_.GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
    // 押し出し判定更新
    for (CollisionDetectionData& data : collisionDetectionData_)
    {
        // ジョイントの名前で位置設定 ( 名前がジョイントの名前ではないとき別途更新必要 )
        data.SetJointPosition(GetJointPosition(data.GetName(), scaleFactor, data.GetOffsetPosition()));
    }
}

void Player::SetWeight(const float& weight)
{
    Object::SetWeight(weight);
}

void Player::AddWeight(const float& weight)
{
    Object::AddWeight(weight);
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
