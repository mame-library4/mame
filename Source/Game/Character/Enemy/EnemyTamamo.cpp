#include "EnemyTamamo.h"
#include "JudgmentTamamo.h"
#include "ActionTamamo.h"
#include "../Other/MathHelper.h"

#if 0
// ----- コンストラクタ -----
EnemyTamamo::EnemyTamamo()
    : Enemy("./Resources/Model/Character/Dragon.glb")
{
}

// ----- デストラクタ -----
EnemyTamamo::~EnemyTamamo()
{
}

// ----- 初期化 -----
void EnemyTamamo::Initialize()
{
    // サイズを設定
    GetTransform()->SetScaleFactor(1.5f);

    GetTransform()->SetPositionZ(10);

    // ステージとの判定offset設定
    SetCollisionRadius(2.5f);

    // 回転速度設定
    SetRotateSpeed(5.0f);

    // 歩行速度設定
    SetWalkSpeed(3.0f);

    // 体力設定
    SetMaxHealth(500.0f);
    //SetMaxHealth(3000.0f);
    SetHealth(GetMaxHealth());

    // sprite
    hpBackSprite_->GetTransform()->SetPosition(340, 650);
    hpBackSprite_->GetTransform()->SetSize(600, 10);
    hpBackSprite_->GetTransform()->SetColor(0, 0, 0);
    hpSprite_->GetTransform()->SetPosition(340, 650);
    hpSprite_->GetTransform()->SetSize(600, 10);
    hpSprite_->GetTransform()->SetColor(1, 0, 0);
}

// ----- 終了化 -----
void EnemyTamamo::Finalize()
{
}

// ----- 更新処理 -----
void EnemyTamamo::Update(const float& elapsedTime)
{
    Character::Update(elapsedTime);

    UpdateStones(elapsedTime);

    // Collisionデータ更新
    UpdateCollisions(elapsedTime, 0.01f);

    // behaviorTree更新
    UpdateNode(elapsedTime);

    // ステージの外に出ないようにする
    CollisionCharacterVsStage();

    // sprite
    float health = GetHealth();
    float maxHealth = GetMaxHealth();
    float hp = health / maxHealth;
    hp = std::max(hp, 0.0f);
    float maxSizeX = 600;
    hpSprite_->GetTransform()->SetSizeX(maxSizeX * hp);
}

// ----- 描画 -----
void EnemyTamamo::Render(ID3D11PixelShader* psShader)
{
    Object::Render(0.01f, psShader);

    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Render(1.0f, psShader);
    }
}

void EnemyTamamo::RenderUserInterface()
{
    hpBackSprite_->Render();
    hpSprite_->Render();
}

// ----- ImGui用 -----
void EnemyTamamo::DrawDebug()
{
    if (ImGui::BeginMenu("Tamamo"))
    {
        hpBackSprite_->DrawDebug();
        hpSprite_->DrawDebug();

        stones_[0]->DrawDebug();

        ImGui::Checkbox("DamageSphere", &isDamageSphere_);
        ImGui::Checkbox("AttackSphere", &isAttackSphere_);
        ImGui::Checkbox("collision", &isCollisionSphere_);

        Character::DrawDebug();

        if (ImGui::TreeNode("Move"))
        {
            ImGui::DragFloat("WalkSpeed", &walkSpeed_);
            ImGui::TreePop();
        }

        if (ImGui::BeginMenu("Battle,Attack,Radius"))
        {
            ImGui::DragFloat("BattleRadius", &battleRadius_);
            ImGui::DragFloat("NearAttackRadius", &nearAttackRadius_);
            ImGui::DragFloat("FarAttackRadius", &farAttackRadius_);
            ImGui::EndMenu();
        }

        std::string str = (activeNode_ != nullptr) ? activeNode_->GetName() : u8"なし";
        ImGui::Text(u8"Behavior：%s", str.c_str());
        if (ImGui::Button("addAttackCount")) AddAttackComboCount();
        ImGui::DragInt("attackComboCount", &attackComboCount_);

        Object::DrawDebug();

        ImGui::EndMenu();
    }
}

// ----- デバッグ用 -----
void EnemyTamamo::DebugRender(DebugRenderer* debugRenderer)
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
    DirectX::XMFLOAT3 forwardVec = GetTransform()->CalcForward();
    DirectX::XMFLOAT3 rightVec = GetTransform()->CalcRight();

    debugRenderer->DrawCylinder(GetTransform()->GetPosition(), GetCollisionRadius(), 2.0f, { 1,1,1,1 });
    
    if (isCollisionSphere_)
    {
        for (auto& data : GetCollisionDetectionData())
        {
            // 現在アクティブではないので表示しない
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
            // 現在アクティブではないでの表示しない
            if (data.GetIsActive() == false) continue;

            debugRenderer->DrawSphere(data.GetPosition(), data.GetRadius(), data.GetColor());
        }
    }

    // 戦闘範囲
    debugRenderer->DrawCylinder(position, battleRadius_, 0.5f, { 1,1,1,1 });

    debugRenderer->DrawCylinder(position + forwardVec * battleRadius_, 0.5f, 2.0f, { 0,1,0,1 });
    debugRenderer->DrawCylinder(position + rightVec * battleRadius_, 0.5f, 2.0f, { 0,1,0,1 });
    debugRenderer->DrawCylinder(position + rightVec * -battleRadius_, 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + forwardVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + rightVec + rightVec) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + forwardVec + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    debugRenderer->DrawCylinder(position + (XMFloat3Normalize(forwardVec + (rightVec * -1) + (rightVec * -1)) * battleRadius_), 0.5f, 2.0f, {0,1,0,1});
    

    // 攻撃範囲
    debugRenderer->DrawCylinder(position, nearAttackRadius_, 0.5f, { 1,0,0,1 });
    debugRenderer->DrawCylinder(position, farAttackRadius_, 0.5f, { 1,0,1,1 });

}

void EnemyTamamo::InitializeStones()
{
    // 生成位置を前足元に設定する
    DirectX::XMFLOAT3 pos = GetTransform()->GetPosition() + GetTransform()->CalcForward() * 4;

    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Initialize(pos);
    }

}

void EnemyTamamo::UpdateStones(const float& elapsedTime)
{
    for (int i = 0; i < maxStoneNum_; ++i)
    {
        stones_[i]->Update(elapsedTime);
    }
}

// ----- 全攻撃判定設定 -----
void EnemyTamamo::SetAllAttackFlag(const bool& activeFlag)
{
    for (int i = 0; i < GetAttackDetectionDataCount(); ++i)
    {
        GetAttackDetectionData(i).SetIsActive(activeFlag);
    }
}

// ----- 噛みつき攻撃判定設定 -----
void EnemyTamamo::SetBiteAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:C_Tongue_1").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Head_1").SetIsActive(activeFlag);
    //GetAttackDetectionData("R:C_Spine_6").SetIsActive(activeFlag);
}

// ----- ひっかき攻撃判定設定 -----
void EnemyTamamo::SetSlashAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:R_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:R_Middle_Finger_2").SetIsActive(activeFlag);
}

// ----- 尻尾攻撃判定設定 -----
void EnemyTamamo::SetTailSwipeAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:C_Tail_3").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Tail_5").SetIsActive(activeFlag);
    GetAttackDetectionData("R:C_Tail_8").SetIsActive(activeFlag);
}

// ----- たたきつけ攻撃判定設定 -----
void EnemyTamamo::SetSlamAttackFlag(const bool& activeFlag)
{
    // 全ての攻撃判定を消す
    SetAllAttackFlag();

    // 必要な攻撃判定を設定する
    GetAttackDetectionData("R:R_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:R_Middle_Finger_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:L_Arm_2").SetIsActive(activeFlag);
    GetAttackDetectionData("R:L_Middle_Finger_2").SetIsActive(activeFlag);
}

// ----- たたきつけ押し出し判定設定 -----
void EnemyTamamo::SetSlamCollisionFlag(const bool& activeFlag)
{
    // 必要な押し出し判定を設定する
    GetCollisionDetectionData("R:C_Tail_3").SetIsActive(activeFlag);
    GetCollisionDetectionData("R:C_Tail_5").SetIsActive(activeFlag);
    GetCollisionDetectionData("R:C_Tail_8").SetIsActive(activeFlag);
}


// ----- collisionData登録 -----
void EnemyTamamo::RegisterCollisionData()
{
    // 押し出し判定
    CollisionDetectionData collisionDetectionData[] =
    {
        { "R:C_Tongue_1",   0.4f,   { 0, 0, 15 },   }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.5f,   {},             }, // 顔
        { "R:C_Spine_6",    1.1f,   { 0, -10, -20 } }, // くび
        { "R:C_Hip",        1.0f,   { 0, -20, 10 }  }, // おしり

        { "R:R_Arm_2",      0.25f,  { 0, 30, 0 }    }, // 右ひじ
        { "R:R_Arm_3",      0.25f,  { 0, -15, 0 }   }, // 右手首
        { "R:R_Hand",       0.3f,   { 0, 0, -20 }   }, // 右手

        { "R:L_Arm_2",      0.25f,  { 0, -30, 0 }   }, // 左ひじ
        { "R:L_Arm_3",      0.25f,  { 0, 15, 0 }    }, // 左手首
        { "R:L_Hand",       0.3f,   { 0, 0, 20 }    }, // 左手         

        { "R:R_Leg_2",      0.25f,  {0,45,20},      }, // 右足付け根
        { "R:R_Leg_3",      0.25f,  {0,20,-10},     }, // 右ひざ
        { "R:R_Foot",       0.3f,   {0,0,-15},      }, // 右足

        { "R:L_Leg_2",      0.25f,  {0,-45,-20},    }, // 左足付け根
        { "R:L_Leg_3",      0.25f,  {0,-20,10},     }, // 左ひざ
        { "R:L_foot",       0.3f,   {0,0,15},       }, // 左足

        { "R:C_Tail_3",     0.65f,  {},             }, // 尻尾
        { "R:C_Tail_5",     1.25f,  {},             }, // 尻尾
        { "R:C_Tail_8",     2.5f,   {},             }, // 尻尾

    };
    // くらい判定
    DamageDetectionData damageDetectionData[] =
    {// { 名前, 半径, ダメージ量, offsetPosition }
        { "R:C_Tongue_1",   0.66f, 10, { 0, 0, 15 }    }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.7f,  10, {}              }, // 顔
        { "R:C_Spine_6",    1.3f,  10, { 0, -10, -20 } }, // くび
        { "R:C_Hip",        1.0f,  10, { 0, -20, 10 }  }, // おしり

        { "R:R_Arm_2",      0.4f,  10, { 0, 30, 0 }    }, // 右ひじ
        { "R:R_Arm_3",      0.4f,  10, { 0, -15, 0 }   }, // 右手首
        { "R:R_Hand",       0.4f,  10, { 0, 0, -20 }   }, // 右手
        
        { "R:L_Arm_2",      0.4f,  10, { 0, -30, 0 }   }, // 左ひじ
        { "R:L_Arm_3",      0.4f,  10, { 0, 15, 0 }    }, // 左手首
        { "R:L_Hand",       0.4f,  10, { 0, 0, 20 }    }, // 左手         
        
        { "R:R_Leg_2",      0.4f,  10, {0,45,20},      }, // 右足付け根
        { "R:R_Leg_3",      0.4f,  10, {0,20,-10},     }, // 右ひざ
        { "R:R_Foot",       0.4f,  10, {0,0,-15},      }, // 右足

        { "R:L_Leg_2",      0.4f,  10, {0,-45,-20},    }, // 左足付け根
        { "R:L_Leg_3",      0.4f,  10, {0,-20,10},     }, // 左ひざ
        { "R:L_foot",       0.4f,  10, {0,0,15},       }, // 左足
        
        { "R:C_Tail_3",     1.0f,  10, {},             }, // 尻尾
        { "R:C_Tail_5",     1.8f,  10, {},             }, // 尻尾
        { "R:C_Tail_8",     3.0f,  10, {},             }, // 尻尾
    };
    // 攻撃判定
    AttackDetectionData attackDetectionData[] =
    {
        { "R:C_Tongue_1",   0.66f,  { 0, 0, 15 }    }, // 顔 ( 舌 )
        { "R:C_Head_1",     0.90f,  {},             }, // 顔
        { "R:C_Spine_6",    1.3f,   { 0, -10, -20 } }, // くび

        { "R:R_Arm_2",              0.5f,   { 0, 40, 0 },   }, // 右腕
        { "R:R_Middle_Finger_2",    0.5f,   { -15, 0, 0 },  }, // 右手

        { "R:L_Arm_2",              0.5f,   { 0, -40, 0 },  }, // 左腕
        { "R:L_Middle_Finger_2",    0.5f,   { -15, 0, 0 },  }, // 左手

        { "R:C_Tail_3",     1.0f,   {},             }, // 尻尾
        { "R:C_Tail_5",     1.8f,   {},             }, // 尻尾
        { "R:C_Tail_8",     3.0f,   {},             }, // 尻尾
    };

    for (int i = 0; i < _countof(collisionDetectionData); ++i)
    {
        RegisterCollisionDetectionData(collisionDetectionData[i]);
    }
    for (int i = 0; i < _countof(damageDetectionData); ++i)
    {
        RegisterDamageDetectionData(damageDetectionData[i]);
    }
    for (int i = 0; i < _countof(attackDetectionData); ++i)
    {
        RegisterAttackDetectionData(attackDetectionData[i]);
    }
}
#endif