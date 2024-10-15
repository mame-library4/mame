#include "UIHealth.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"

// ----- コンストラクタ -----
UIHealth::UIHealth()
    : UI(UIManager::UIType::UIHealth, L"./Resources/Image/white.png", "UIHealth")
{
    healthFrame_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    healthFrame_->SetName("HealthFrame");
    healthFrame_->GetTransform()->SetPosition(50.0f, 50.0f);
    healthFrame_->GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    healthFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    autoRecoveryBar_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    autoRecoveryBar_->SetName("AutoRepairBar");
    autoRecoveryBar_->GetTransform()->SetPosition(50.0f, 50.0f);
    autoRecoveryBar_->GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    autoRecoveryBar_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    SetSpriteName("Health");
    GetTransform()->SetPosition(50.0f, 50.0f);
    GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    GetTransform()->SetColor(0.0f, 1.0f, 0.0f);

    oldHealth_ = 300.0f;
    autoRecoveryHealth_ = 300.0f;

    isAllUICreated = true;
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
}

// ----- 描画 -----
void UIHealth::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    healthFrame_->Render();

    autoRecoveryBar_->Render();

    UI::Render();
}

// ----- ImGui用 -----
void UIHealth::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str())) 
    {
        ImGui::DragFloat("HealSpeed", &healSpeed_);

        UI::DrawDebug();

        autoRecoveryBar_->DrawDebug();

        healthFrame_->DrawDebug();

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
    autoRecoveryHealth_ = currentHealth + 30.0f;
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
        SetUIPosition(healthPosition_);

        isVibration_ = false;

        return;
    }

    DirectX::XMFLOAT2 vibrationVec = { rand() % 100 - 50.0f, rand() % 100 - 50.0f };
    vibrationVec = XMFloat2Normalize(vibrationVec);

    const float vibrationVolume = Easing::InSine(vibrationTimer_, vibrationTimer_, vibrationVolume_, 0.0f);
    const DirectX::XMFLOAT2 uiPosition = vibrationVec * vibrationVolume + healthPosition_;
    
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

    float recoveryHealth = autoRecoveryHealth_ / maxHealth;
    recoveryHealth = max(maxHealthSizeX_ * recoveryHealth, 0.0f);
    autoRecoveryBar_->GetTransform()->SetSizeX(recoveryHealth);
}

// ----- 一括でUIの位置を設定する -----
void UIHealth::SetUIPosition(const DirectX::XMFLOAT2& position)
{
    GetTransform()->SetPosition(position);
    healthFrame_->GetTransform()->SetPosition(position);
    autoRecoveryBar_->GetTransform()->SetPosition(position);
}
