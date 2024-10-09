#include "UIPartDestruction.h"

// ----- コンストラクタ -----
UIPartDestruction::UIPartDestruction()
    : UINotifications(UIManager::UIType::UINotifications, L"./Resources/Image/UI/Notifications/PartDestruction.png", "UIPartDestruction")
{
    GetTransform()->SetPosition(970.0f, 420.0f);
    
    GetTransform()->SetSize(230.0f, 40.0f);
}
