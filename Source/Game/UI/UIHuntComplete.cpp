#include "UIHuntComplete.h"

// ----- �R���X�g���N�^ -----
UIHuntComplete::UIHuntComplete()
    : UINotifications(UIManager::UIType::UINotifications, L"./Resources/Image/UI/Notifications/HuntComplete.png", "UIHuntComplete")
{
    GetTransform()->SetPosition(970.0f, 420.0f);

    GetTransform()->SetSize(230.0f, 40.0f);
}