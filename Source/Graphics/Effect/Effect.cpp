#include "../Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

int Effect::nameNum = 0;

// コンストラクタ
Effect::Effect(const char* filename)
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
    effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);

    // imgui名前かぶり起きないように...
    name = "Effect" + std::to_string(nameNum);
    SetName(name.c_str());
    ++nameNum;
}

// 再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 color)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);
    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
    effekseerManager->SetRotation(handle, Effekseer::Vector3D(0, 1, 0), angle);

    // color設定
    {
        //Effekseerのcolorは0~255らしい
        Effekseer::Color col{ static_cast<unsigned char>(color.x * 255),
            static_cast<unsigned char>(color.y * 255),
        static_cast<unsigned char>(color.z * 255),
        static_cast<unsigned char>(color.w * 255) };
        effekseerManager->SetAllColor(handle, col);
    }

    return handle;
}


// 停止
void Effect::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->StopEffect(handle);
}

// 座標指定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

// スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

void Effect::DrawDebug()
{
    ImGui::Begin(GetName());
    
    if (ImGui::Button("Play"))
    {
        Play(DirectX::XMFLOAT3(0,0,0));
    }

    ImGui::DragInt("drawTime", &drawTime);

    ImGui::DragFloat3("pos", &pos.x);
    ImGui::DragFloat3("scale", &scale.x);

    DirectX::XMFLOAT3 r{};
    r.x = DirectX::XMConvertToDegrees(rotate.x);
    r.y = DirectX::XMConvertToDegrees(rotate.y);
    r.z = DirectX::XMConvertToDegrees(rotate.z);
    
    ImGui::DragFloat3("rotate", &r.x);

    rotate.x = DirectX::XMConvertToRadians(r.x);
    rotate.y = DirectX::XMConvertToRadians(r.y);
    rotate.z = DirectX::XMConvertToRadians(r.z);
    
    ImGui::ColorEdit4("color", &color.x);
    ImGui::DragFloat("timer", &timer);
    ImGui::End();
}


Effekseer::Handle Effect::FadeOutEffect(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 color, const float time)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();


    Effekseer::Vector3D p{ pos.x, pos.y, pos.z };

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, p, time);
    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);


    // color設定
    {
        //Effekseerのcolorは0~255らしい
        Effekseer::Color col{ static_cast<unsigned char>(color.x * 255),
            static_cast<unsigned char>(color.y * 255),
        static_cast<unsigned char>(color.z * 255),
        static_cast<unsigned char>(color.w * 255) };
        effekseerManager->SetAllColor(handle, col);
    }

    return handle;
}

Effekseer::Handle Effect::FadeOutEffect(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT4 color, const float time)
{

    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();


    Effekseer::Vector3D p{ pos.x, pos.y, pos.z };

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, p, time);
    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
    

    effekseerManager->SetRotation(handle, rotate.x, rotate.y, rotate.z);

    // color設定
    {
        //Effekseerのcolorは0~255らしい
        Effekseer::Color col{ static_cast<unsigned char>(color.x * 255),
            static_cast<unsigned char>(color.y * 255),
        static_cast<unsigned char>(color.z * 255),
        static_cast<unsigned char>(color.w * 255) };
        effekseerManager->SetAllColor(handle, col);
    }

    return handle;
}

void Effect::FadeOutEffect(Effekseer::Handle handle, const float time)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();


    Effekseer::Vector3D p{ pos.x, pos.y, pos.z };

    effekseerManager->Play(effekseerEffect, p, time);

    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
    // color設定
    {
        //Effekseerのcolorは0~255らしい
        Effekseer::Color col{ static_cast<unsigned char>(1.0f * 255),
            static_cast<unsigned char>(0.0f * 255),
        static_cast<unsigned char>(0.0f * 255),
        static_cast<unsigned char>(1.0f * 255) };
        effekseerManager->SetAllColor(handle, col);
    }
}
