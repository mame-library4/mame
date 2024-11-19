#include "UIHealth.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"
#include "NoiseTexture.h"

// ----- コンストラクタ -----
UIHealth::UIHealth()
    : UI(UIManager::UIType::UIHealth, L"./Resources/Image/white.png", "UIHealth")
{
    // 初期位置を保存しておく
    healthPosition_ = DirectX::XMFLOAT2(68.0f, 33.0f);
    healthFramePosition_ = DirectX::XMFLOAT2(68.0f, 30.0f);
    healthRhombusPosition_ = DirectX::XMFLOAT2(60.0f, 31.0f);
    healthRhombusFramePosition_ = DirectX::XMFLOAT2(57.0f, 28.0f);
    
    healthRhombus_      = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    autoRecoveryBar_    = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    healthFrame_        = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    healthRhombusFrame_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    healthIcon_         = std::make_unique<Sprite>(L"./Resources/Image/UI/Health.png");

    // 初期化
    Initialize();

    oldHealth_ = 300.0f;
    autoRecoveryHealth_ = 300.0f;

    isAllUICreated = true;
}

// ----- 初期化 -----
void UIHealth::Initialize()
{
    const float sizeY = 6.0f;

    // ----- 体力ゲージ -----
    SetSpriteName("Health");
    SetIsActiveUVScroll();
    SetNoiseTextureNum(0);
    SetScrollDirection({ 0.25f, 0.0f });
    GetTransform()->SetPosition(healthPosition_);
    GetTransform()->SetSize(maxHealthSizeX_, sizeY);
    GetTransform()->SetColor(0.0f, 1.0f, 0.0f);

    // ----- ◇ひし形 -----
    healthRhombus_->SetName("HealthRhombus");
    healthRhombus_->SetIsActiveUVScroll();
    healthRhombus_->SetNoiseTextureNum(1);
    healthRhombus_->SetScrollDirection({ 0.5f, 0.5f });
    healthRhombus_->GetTransform()->SetPosition(healthRhombusPosition_);
    healthRhombus_->GetTransform()->SetSize(10.0f);
    healthRhombus_->GetTransform()->SetAngle(45.0f);
    healthRhombus_->GetTransform()->SetColor(0.0f, 1.0f, 0.0f);

    // ----- 赤ゲージ -----
    autoRecoveryBar_->SetName("AutoRepairBar");
    autoRecoveryBar_->GetTransform()->SetPosition(healthPosition_);
    autoRecoveryBar_->GetTransform()->SetSize(maxHealthSizeX_, sizeY);
    autoRecoveryBar_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    // ----- 体力の枠(黒色) -----
    healthFrame_->SetName("HealthFrame");
    healthFrame_->GetTransform()->SetPosition(healthFramePosition_);
    healthFrame_->GetTransform()->SetSize(453.0f, 12.0f);
    healthFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ----- ◇ひし形の枠(黒色) -----
    healthRhombusFrame_->SetName("HealthRhombusFrame");
    healthRhombusFrame_->GetTransform()->SetPosition(healthRhombusFramePosition_);
    healthRhombusFrame_->GetTransform()->SetSize(16.0f);
    healthRhombusFrame_->GetTransform()->SetAngle(45.0f);
    healthRhombusFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ----- 体力アイコン -----
    healthIcon_->SetName("HealthIcon");
    healthIcon_->SetIsActiveUVScroll();
    healthIcon_->SetNoiseTextureNum(1);
    healthIcon_->SetScrollDirection({ 0.0f, 0.5f });
    healthIcon_->GetTransform()->SetPosition(20.0f, 20.0f);
    healthIcon_->GetTransform()->SetSize(32.0f);
}

// ----- 更新 -----
void UIHealth::Update(const float& elapsedTime)
{
    if (isAllUICreated == false) return;

    // 表示していないときは更新しない
    if (GetIsDraw() == false)
    {
        oldHealth_ = PlayerManager::Instance().GetPlayer()->GetHealth();

        return;
    }

    // ダメージ効果をUIに反映させる
    ApplyDamageEffect();

    // 振動更新
    UpdateVibration(elapsedTime);

    // 自動回復
    UpdateAutoRecovery(elapsedTime);

    // 画像サイズ更新
    UpdateSpriteSize();

    // 現在の体力を保存する
    oldHealth_ = PlayerManager::Instance().GetPlayer()->GetHealth();

    // 更新
    UI::Update(elapsedTime);
    healthRhombus_->Update(elapsedTime);
    healthIcon_->Update(elapsedTime);
}

// ----- 描画 -----
void UIHealth::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    healthFrame_->Render();
    healthRhombusFrame_->Render();

    autoRecoveryBar_->Render();
    
    UI::Render();

    healthRhombus_->Render();

    healthIcon_->Render();
}

// ----- ImGui用 -----
void UIHealth::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str())) 
    {
        ImGui::DragFloat("maxHealthSizeX", &maxHealthSizeX_);

        ImGui::DragFloat("HealSpeed", &healSpeed_);

        UI::DrawDebug();

        autoRecoveryBar_->DrawDebug();
        healthRhombus_->DrawDebug();

        healthFrame_->DrawDebug();
        healthRhombusFrame_->DrawDebug();

        healthIcon_->DrawDebug();

        ImGui::TreePop();
    }
}

// ----- ダメージ効果を適用 -----
void UIHealth::ApplyDamageEffect()
{
    const float currentHealth = PlayerManager::Instance().GetPlayer()->GetHealth();

    // ダメージを受けていないのでここで終了
    if (currentHealth >= oldHealth_) return;

    isVibration_     = true; // 振動更新処理適用
    vibrationVolume_ = 5.0f; // 振動の強さ
    vibrationTimer_  = 0.2f; // 振動時間計測用
    vibrationTime_   = 0.2f; // 振動時間
    
    // 自動回復するゲージの幅を求める
    if (currentHealth <= 0.0f) autoRecoveryHealth_ = 0.0f;
    else autoRecoveryHealth_ = currentHealth + 20.0f;
}

// ----- 振動更新 -----
void UIHealth::UpdateVibration(const float& elapsedTime)
{
    // 振動処理をしないのでここで終了
    if (isVibration_ == false) return;

    // タイマー更新
    vibrationTimer_ -= elapsedTime;
    vibrationTimer_ = max(vibrationTimer_, 0.0f);

    // 振動の更新時間が終了していたらUIを元の位置に戻して終了する
    if (vibrationTimer_ <= 0.0f)
    {
        // リセット
        SetUIPosition({});

        isVibration_ = false;

        return;
    }

    DirectX::XMFLOAT2 vibrationVec = { rand() % 100 - 50.0f, rand() % 100 - 50.0f };
    vibrationVec = XMFloat2Normalize(vibrationVec);

    const float vibrationVolume = Easing::InSine(vibrationTimer_, vibrationTimer_, vibrationVolume_, 0.0f);
    const DirectX::XMFLOAT2 uiPosition = vibrationVec * vibrationVolume;
    
    SetUIPosition(uiPosition);
}

// ----- 自動回復 -----
void UIHealth::UpdateAutoRecovery(const float& elapsedTime)
{
    float health = PlayerManager::Instance().GetPlayer()->GetHealth();

    // 回復量がないときはここで終了
    if (health >= autoRecoveryHealth_) return;

    health += healSpeed_ * elapsedTime;
    health = min(health, autoRecoveryHealth_);

    PlayerManager::Instance().GetPlayer()->SetHealth(health);
}

// ----- 画像サイズを更新する -----
void UIHealth::UpdateSpriteSize()
{
    const float health = PlayerManager::Instance().GetPlayer()->GetHealth();
    const float maxHealth = PlayerManager::Instance().GetPlayer()->GetMaxHealth();

    // 現在の体力の全体から見た割合を出す
    float currentHealth = health / maxHealth;

    const float healthSizeX = max(maxHealthSizeX_ * currentHealth, 0.0f);

    GetTransform()->SetSizeX(healthSizeX);
    GetTransform()->SetTexSizeX(healthSizeX);

    float recoveryHealth = autoRecoveryHealth_ / maxHealth;
    recoveryHealth = max(maxHealthSizeX_ * recoveryHealth, 0.0f);
    autoRecoveryBar_->GetTransform()->SetSizeX(recoveryHealth);
}

// ----- 一括でUIの位置を設定する -----
void UIHealth::SetUIPosition(const DirectX::XMFLOAT2& position)
{
    GetTransform()->SetPosition(healthPosition_ + position);
    autoRecoveryBar_->GetTransform()->SetPosition(healthPosition_ + position);

    healthFrame_->GetTransform()->SetPosition(healthFramePosition_ + position);

    healthRhombus_->GetTransform()->SetPosition(healthRhombusPosition_ + position);

    healthRhombusFrame_->GetTransform()->SetPosition(healthRhombusFramePosition_ + position);
}