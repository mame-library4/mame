#include "../Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

// ----- コンストラクタ -----
Effect::Effect(const char* filename, const std::string& effectName)
    : name_(effectName)
{
    // エフェクトを読み込みする前にロックする
    // ※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
    // フリーズする可能性があるので排他制御する
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    // Effekseerのリソースを読み込む
    // EffekseerはUTF-16のファイルパス以外は対応していないため文字コードが必要
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    // Effekseer::Managerを取得
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    // Effekseerエフェクトを読み込み
    effekseerEffect_ = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);

    EffectManager::Instance().Register(this);
}


// ----- 再生 -----
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, const float& scale, const float& speed)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
    
    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect_, position.x, position.y, position.z);
    
    effekseerManager->SetScale(handle, scale, scale, scale);
    effekseerManager->SetSpeed(handle, speed);

    return handle;
}

void Effect::Stop(const Effekseer::Handle& handle)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->StopEffect(handle);
}

// ----- 位置設定 -----
void Effect::SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
    effekseerManager->SetLocation(handle, pos);
}

// ----- 大きさ設定 -----
void Effect::SetScale(const Effekseer::Handle& handle, const float& scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetScale(handle, scale, scale, scale);
}

void Effect::DrawDebug()
{
    if (ImGui::TreeNode(name_.c_str()))
    {
        if (ImGui::Button("Play"))
        {
            Play(position_, scale_, speed_);
        }

        ImGui::DragFloat3("Position", &position_.x);
        ImGui::DragFloat("Scale", &scale_);

        DirectX::XMFLOAT3 rotate = {};
        rotate.x = DirectX::XMConvertToDegrees(rotate_.x);
        rotate.y = DirectX::XMConvertToDegrees(rotate_.y);
        rotate.z = DirectX::XMConvertToDegrees(rotate_.z);

        ImGui::DragFloat3("Rotate", &rotate.x);

        rotate_.x = DirectX::XMConvertToRadians(rotate.x);
        rotate_.y = DirectX::XMConvertToRadians(rotate.y);
        rotate_.z = DirectX::XMConvertToRadians(rotate.z);

        ImGui::ColorEdit4("Color", &color_.x);

        ImGui::DragFloat("Speed", &speed_);

        ImGui::TreePop();
    }
}