#include "UIPartDestruction.h"

// ----- コンストラクタ -----
UIPartDestruction::UIPartDestruction()
    : UI(UIManager::UIType::UIPartDestruction, L"./Resources/Image/White.png", "UIPartDestruction")
{
    // TODO:部位破壊UI
    // とりあえずUIを書くまでWhiteSpriteで代用するので
    // サイズと位置を設定する
    GetTransform()->SetPosition(970.0f, 420.0f);
    GetTransform()->SetSize(290.0f, 70.0f);
}

// ----- 更新 -----
void UIPartDestruction::Update(const float& elapsedTime)
{
    UI::Update(elapsedTime);

    // フェイドインする
    if (isFadingIn_)
    {
        fadeInTimer_ += fadeInSpeed_ * elapsedTime;
        fadeInTimer_ = min(fadeInTimer_, 1.0f);

        const float posX = XMFloatLerp(1280.0f, 970.0f, fadeInTimer_);

        GetTransform()->SetPositionX(posX);

        // フェイドインが完了したら終了する
        if (fadeInTimer_ == 1.0f)
        {
            oldPostiion_ = GetTransform()->GetPosition();

            isFadingIn_ = false;
        }
        // フェイドイン中はここで終了
        else
        {
            return;
        }
    }

    // 生存時間を更新する
    lifeTimer_ -= elapsedTime;

    // 生存時間がなくなったらフェイドアウトのフラグを立てる
    if (lifeTimer_ <= 0.0f && isFadingOut_ == false) isFadingOut_ = true;

    // フェイドアウトしてUIを消去する
    if (isFadingOut_)
    {
        fadeOutTimer_ += fadeOutSpeed_* elapsedTime;
        fadeOutTimer_ = min(fadeOutTimer_, 1.0f);

        const float currentPosY = GetTransform()->GetPositionY();

        const float posY = XMFloatLerp(currentPosY, currentPosY - 10.0f, fadeOutTimer_);
        const float alpha = XMFloatLerp(1.0f, 0.0f, fadeOutTimer_);

        GetTransform()->SetPositionY(posY);
        GetTransform()->SetColorA(alpha);

        // UIクラスでの移動処理を上手く機能させるためにここで更新する
        const float addPosY = oldPostiion_.y - posY;
        oldPostiion_.y = posY;
        AddMoveTargetPosition({ 0.0f, addPosY });

        // フェイドアウトが完了したら自身を削除する
        if (fadeOutTimer_ == 1.0f) UIManager::Instance().Remove(this);
    }
}

// ----- 描画 -----
void UIPartDestruction::Render()
{
    UI::Render();
}

// ----- ImGui -----
void UIPartDestruction::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);
        ImGui::DragFloat("FadeOutTimer", &fadeOutTimer_);
        ImGui::DragFloat("FadeOutSpeed", &fadeOutSpeed_);

        UI::DrawDebug();

        ImGui::TreePop();
    }
}
