#include "UICrosshair.h"
#include "Character/Enemy/EnemyManager.h"
#include "Camera.h"
#include "Easing.h"
#include "AudioManager.h"

// ----- コンストラクタ -----
UICrosshair::UICrosshair()
    : UI(UIManager::UIType::UICrosshair, L"./Resources/Image/Crosshair/OuterLine.png", "UICrosshair")
{
    // アウトライン命名
    SetSpriteName("OuterLine");
    
    // センタードットSprite生成 & 命名
    centerDot_ = std::make_unique<Sprite>(L"./Resources/Image/Crosshair/CenterDot.png");
    centerDot_->SetName("CenterDot");
    centerDot_->GetTransform()->SetColorA(0.0f);
}

// ----- 更新 -----
void UICrosshair::Update(const float& elapsedTime)
{
    // 敵(ターゲット)が生きているか判定
    if (JudgementEnemyAlive() == false) return;

    // 現在ロックオンしているジョイントの位置を取得する
    const std::string jointName = Camera::Instance().GetCurrentTargetJointName();
    targetJointPosition_ = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(jointName);

    // UI描画位置が視界に入ってるかを判定する
    if (JudgementDraw() == false) return;

    // Spriteの大きさと色を更新する
    UpdateSpriteSizeAndColor(elapsedTime);

    // Spriteの位置を更新する
    UpdateSpritePosition();
}

// ----- 描画 -----
void UICrosshair::Render()
{
    // 敵(ターゲット)が生きていない
    if (isTargetEnemyAlive_ == false) return;

    // 視界に入ってないので描画しない
    if (isDraw_ == false) return;

    // アウトライン描画 ( 外枠 )
    UI::Render();

    // センタードット描画
    centerDot_->Render();
}

// ----- ImGui用 -----
void UICrosshair::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        if (ImGui::Button("ResetTimer"))
        {
            outerLineTimer_ = 0.0f;
        }
        ImGui::DragFloat("OuterLineTotalFrame_", &outerLineTotalFrame_);

        UI::DrawDebug();

        centerDot_->DrawDebug();

        ImGui::TreePop();
    }
}

// ----- Spriteの位置更新 -----
void UICrosshair::UpdateSpritePosition()
{   
    // World空間からScreen空間へ変換
    DirectX::XMFLOAT2 targetPosition = Sprite::ConvertToScreenPos(targetJointPosition_);

    // ---------- 位置を更新する ----------
    DirectX::XMFLOAT2 outerLinePosition = { targetPosition.x - GetTransform()->GetSizeX() * 0.5f, targetPosition.y - GetTransform()->GetSizeY() * 0.5f };
    GetTransform()->SetPosition(outerLinePosition, GetTransform()->GetSize());

    DirectX::XMFLOAT2 centerDotPosition = { targetPosition.x - centerDot_->GetTransform()->GetSizeX() * 0.5f, targetPosition.y - centerDot_->GetTransform()->GetSizeY() * 0.5f };
    centerDot_->GetTransform()->SetPosition(centerDotPosition, centerDot_->GetTransform()->GetSize());
}

// ----- Spriteの大きさ更新 -----
void UICrosshair::UpdateSpriteSizeAndColor(const float& elapsedTime)
{
    // 大きさの更新は終了したのでここで終了
    if (isSpriteSizeUpdated_) return;

    outerLineTimer_ += elapsedTime;
    outerLineTimer_ = min(outerLineTimer_, outerLineTotalFrame_);

    const float size = Easing::InSine(outerLineTimer_, outerLineTotalFrame_, 64.0f, 256.0f);
    const float alpha = Easing::InSine(outerLineTimer_, outerLineTotalFrame_, 1.0f, 0.0f);

    GetTransform()->SetSize(size);
    GetTransform()->SetColorA(alpha);

    // アウトライン ( 外枠 ) の更新が終わったらセンタードットを表示する
    if (outerLineTimer_ == outerLineTotalFrame_)
    {
        centerDot_->GetTransform()->SetSize(128.0f);
        centerDot_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

        AudioManager::Instance().PlaySE(SE::Lockon);

        isSpriteSizeUpdated_ = true;
    }
}

// ----- 敵(ターゲット)が生きているか判定 -----
const bool UICrosshair::JudgementEnemyAlive()
{
    isTargetEnemyAlive_ = false;

    // そもそも存在していない
    if (EnemyManager::Instance().GetEnemyCount() == 0) return false;

    // 死亡している
    if (EnemyManager::Instance().GetEnemy(0)->GetHealth() <= 0) return false;

    // 生きている
    isTargetEnemyAlive_ = true;

    return true;
}

// ----- UI描画位置が視界に入ってるかを判定判定 -----
const bool UICrosshair::JudgementDraw()
{
    isDraw_ = false;

    // TODO:Y軸を考慮していないのでもしかしたらバグるかも
    DirectX::XMFLOAT3 cameraPosition = Camera::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
    DirectX::XMFLOAT2 targetVec = XMFloat2Normalize({cameraPosition.x - targetJointPosition_.x, cameraPosition.z - targetJointPosition_.z});
    const float dot = XMFloat2Dot(targetVec, XMFloat2Normalize({ cameraForward.x, cameraForward.z }));

    if (dot > 0) return false;
    //if (dot > DirectX::XM_PIDIV2) return false;

    // 描画する
    isDraw_ = true;

    return true;
}
