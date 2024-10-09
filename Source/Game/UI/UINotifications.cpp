#include "UINotifications.h"

// ----- コンストラクタ -----
UINotifications::UINotifications(const UIManager::UIType& type, const wchar_t* filename, const std::string& name)
    : UI(type, filename, name)
{
}

// ----- 更新 -----
void UINotifications::Update(const float& elapsedTime)
{
    UI::Update(elapsedTime);

    // フェイドインする
    if (isFadingIn_)
    {
        fadeInTimer_ += fadeInSpeed_ * elapsedTime;
        fadeInTimer_ = min(fadeInTimer_, 1.0f);

        const float posX = XMFloatLerp(fadeInStartPositionX_, fadeInEndPositionX_, fadeInTimer_);

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
        fadeOutTimer_ += fadeOutSpeed_ * elapsedTime;
        fadeOutTimer_ = min(fadeOutTimer_, 1.0f);

        const float currentPosY = GetTransform()->GetPositionY();

        const float posY = XMFloatLerp(currentPosY, currentPosY - 5.0f, fadeOutTimer_);
        const float alpha = XMFloatLerp(1.0f, 0.0f, fadeOutTimer_);

        GetTransform()->SetPositionY(posY);
        GetTransform()->SetColorA(alpha);

        // フェイドアウトが完了したら自身を削除する
        if (fadeOutTimer_ == 1.0f) UIManager::Instance().Remove(this);
    }
}

// ----- 描画 -----
void UINotifications::Render()
{
    UI::Render();
}

// ----- ImGui用 -----
void UINotifications::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);
        ImGui::DragFloat("FadeOutTimer", &fadeOutTimer_);
        ImGui::DragFloat("FadeOutSpeed", &fadeOutSpeed_);

        UI::DrawDebug();

        ImGui::TreePop();
    }
#endif
}
