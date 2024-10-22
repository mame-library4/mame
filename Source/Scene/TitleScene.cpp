#include "TitleScene.h"
#include "TitleState.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "Input.h"
#include "Texture.h"
#include "Camera.h"


// ----- リソース生成 -----
void TitleScene::CreateResource()
{
    uiTitle_ = new UITitle();

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
    
    // ----- ステートマシン -----
    {
        stateMachine_.reset(new StateMachine<State<TitleScene>>);

        // ステートを登録する
        GetStateMachine()->RegisterState(new TitleState::IdleState(this));
        GetStateMachine()->RegisterState(new TitleState::SelectState(this));

        // 一番初めのステートを設定する
        GetStateMachine()->SetState(static_cast<UINT>(STATE::Idle));
        currentState_ = STATE::Idle;
    }    
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
}

// ----- 終了化 -----
void TitleScene::Finalize()
{
    UIManager::Instance().Remove(UIManager::UIType::UITitle);
}

// ----- 更新 -----
void TitleScene::Update(const float& elapsedTime)
{
    // ステートマシン更新
    GetStateMachine()->Update(elapsedTime);

    SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));

    dragonObject_->Update(elapsedTime);
    playerObject_->Update(elapsedTime);
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

// ----- ステート変更 -----
void TitleScene::ChangeState(const STATE& state)
{
    // 前回のステートを記録
    oldState_ = currentState_;

    // 現在のステートを記録
    currentState_ = state;

    stateMachine_.get()->ChangeState(static_cast<int>(state));
}
