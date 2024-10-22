#include "TitleScene.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "Input.h"
#include "Texture.h"
#include "Camera.h"

// ----- リソース生成 -----
void TitleScene::CreateResource()
{
    D3D11_TEXTURE2D_DESC textureDesc = {};
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
        iblTextures_[0].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
        iblTextures_[1].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
        iblTextures_[2].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/lut_ggx.DDS",
        iblTextures_[3].GetAddressOf(), &textureDesc);

    stageObject_  = std::make_unique<Object>("./Resources/Model/Stage/OnlyStage/stageOneMesh.gltf", 1.0f);
    dragonObject_ = std::make_unique<Object>("./Resources/Model/TitleObject/Dragon/DragonObject.gltf", 1.0f);
    playerObject_ = std::make_unique<Object>("./Resources/Model/TitleObject/Player/PlayerObject.gltf", 0.01f);
    
    //uiTitle_ = new UITitle();
}

// ----- 初期化 -----
void TitleScene::Initialize()
{
    // 現在のSceneを設定
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Title);

    Camera::Instance().SetTitleCamera();

    //dragonObject_->PlayAnimation(1, true, 1.0f);
    dragonObject_->PlayAnimation(0, true, 1.0f);
    dragonObject_->GetTransform()->SetScaleFactor(1.5f);
    playerObject_->PlayAnimation(0, true, 1.0f);

    dragonObject_->GetTransform()->SetPosition(4.0f, 0.0f, -7.0f);
    dragonObject_->GetTransform()->SetRotation(0.0f, DirectX::XMConvertToRadians(325.0f), 0.0f);

    playerObject_->GetTransform()->SetPosition(-1.0f, 0.0f, 2.0f);
    playerObject_->GetTransform()->SetRotation(0.0f, DirectX::XMConvertToRadians(162.0f), 0.0f);

    // 変数初期化
    isDrawUI_ = false;
}

// ----- 終了化 -----
void TitleScene::Finalize()
{
    UIManager::Instance().Remove(UIManager::UIType::UITitle);
}

// ----- 更新 -----
void TitleScene::Update(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    dragonObject_->Update(elapsedTime);
    playerObject_->Update(elapsedTime);

    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
        return;
    }

    // UI描画判定更新
    if (isDrawUI_ == false)
    {
        UI* ui = UIManager::Instance().GetUI(UIManager::UIType::UITitle);
        if (ui != nullptr)
        {
            ui->SetIsDraw();

            isDrawUI_ = true;
        }
    }

    //uiTitle_->SetState(0);
}

void TitleScene::ShadowRender()
{
}

void TitleScene::DeferredRender()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();

    deviceContext->PSSetShaderResources(32, 1, iblTextures_[0].GetAddressOf());
    deviceContext->PSSetShaderResources(33, 1, iblTextures_[1].GetAddressOf());
    deviceContext->PSSetShaderResources(34, 1, iblTextures_[2].GetAddressOf());
    deviceContext->PSSetShaderResources(35, 1, iblTextures_[3].GetAddressOf());

    stageObject_->Render(gBufferPixelShader);
    dragonObject_->Render(gBufferPixelShader);
    playerObject_->Render(gBufferPixelShader);
}

void TitleScene::ForwardRender()
{
}

// ----- ImGui用 -----
void TitleScene::DrawDebug()
{
    if (ImGui::TreeNode("Dragon"))
    {
        dragonObject_->DrawDebug();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Player"))
    {
        playerObject_->DrawDebug();
        ImGui::TreePop();
    }
}
