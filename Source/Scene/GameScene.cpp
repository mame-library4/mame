#include "GameScene.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/EnemyDragon.h"
#include "Camera.h"


// ----- ステージの真ん中位置 -----
DirectX::XMFLOAT3 GameScene::stageCenter_ = {};

// ----- リソース生成 -----
void GameScene::CreateResource()
{
    // プレイヤー生成
    PlayerManager::Instance().GetPlayer() = std::make_unique<Player>();

    //stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/tomorrow.glb");
    //stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/today.glb");
    stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/circle.glb");
    //stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/arena.glb");


    EnemyManager::Instance().Register(new EnemyDragon);

    // IBLテクスチャ
    D3D11_TEXTURE2D_DESC textureDesc = {};
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
        iblTextures_[0].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
        iblTextures_[1].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
        iblTextures_[2].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/lut_ggx.DDS",
        iblTextures_[3].GetAddressOf(), &textureDesc);

    particles_ = std::make_unique<decltype(particles_)::element_type>(100);
}

// ----- 初期化 -----
void GameScene::Initialize()
{
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Game);

    stage_->GetTransform()->SetScaleFactor(1.5f);

    stageCenter_ = stage_->GetTransform()->GetPosition();

    // プレイヤー初期化
    PlayerManager::Instance().Initialize();

    // 敵初期化
    EnemyManager::Instance().Initialize();

    //particles_->Initialize(0);
}

// ----- 終了化 -----
void GameScene::Finalize()
{
    // プレイヤー終了化
    PlayerManager::Instance().Finalize();

    // 敵終了化
    EnemyManager::Instance().Finalize();
}

// ----- 更新 -----
void GameScene::Update(const float& elapsedTime)
{
    // プレイヤー更新
    PlayerManager::Instance().Update(elapsedTime);

    // 敵更新
    EnemyManager::Instance().Update(elapsedTime);

    // ステージ位置更新
    stageCenter_ = stage_->GetTransform()->GetPosition();

    // カメラの位置更新
    Camera::Instance().SetTarget(PlayerManager::Instance().GetTransform()->GetPosition());

    if (GetAsyncKeyState('T') & 0x8000)
    {
        particles_->Initialize(0);
    }
    particles_->Integrate(elapsedTime);
}

void GameScene::ShadowRender()
{

    PlayerManager::Instance().Render();
    EnemyManager::Instance().Render();
}

void GameScene::DeferredRender()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();

    deviceContext->PSSetShaderResources(32, 1, iblTextures_[0].GetAddressOf());
    deviceContext->PSSetShaderResources(33, 1, iblTextures_[1].GetAddressOf());
    deviceContext->PSSetShaderResources(34, 1, iblTextures_[2].GetAddressOf());
    deviceContext->PSSetShaderResources(35, 1, iblTextures_[3].GetAddressOf());

    // ステージ
    stage_->Render(0.01f, gBufferPixelShader);

    // プレイヤー描画
    PlayerManager::Instance().Render(gBufferPixelShader);

    // 敵描画
    EnemyManager::Instance().Render(gBufferPixelShader);

}

void GameScene::ForwardRender()
{
#if 1
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->PSSetShaderResources(32, 1, iblTextures_[0].GetAddressOf());
    deviceContext->PSSetShaderResources(33, 1, iblTextures_[1].GetAddressOf());
    deviceContext->PSSetShaderResources(34, 1, iblTextures_[2].GetAddressOf());
    deviceContext->PSSetShaderResources(35, 1, iblTextures_[3].GetAddressOf());

    // ステージ
    stage_->Render(0.01f);

    // プレイヤー描画
    PlayerManager::Instance().Render();

    // 敵描画
    EnemyManager::Instance().Render();
#endif




    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
#ifdef _DEBUG
    if (isDebugRenderer_)
    {
        // player
        PlayerManager::Instance().DebugRender(debugRenderer);

        // enemy
        EnemyManager::Instance().DebugRender(debugRenderer);

        DirectX::XMFLOAT3 position{};
        position = stage_->GetTransform()->GetPosition();

        debugRenderer->DrawCylinder(position, stageRadius_, 1.5f, { 1, 0, 0, 1 });
        //debugRenderer->DrawCylinder(position, stageRadius1_, 1.5f, { 1, 0, 0, 1 });

    }
#endif
}

void GameScene::UserInterfaceRender()
{
    Graphics::Instance().GetShader()->SetBlendState(Shader::BLEND_STATE::ALPHA);

}

void GameScene::Render()
{
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);

    PlayerManager::Instance().RenderTrail();
    //particles_->Render();
}

// ----- ImGui用 -----
void GameScene::DrawDebug()
{
    ImGui::Checkbox("Debug", &isDebugRenderer_);
    ImGui::DragFloat("stageRadius", &stageRadius1_);

    particles_->DrawDebug();

    // プレイヤーImGui
    PlayerManager::Instance().DrawDebug();

    // 敵ImGui
    EnemyManager::Instance().DrawDebug();

    if (ImGui::BeginMenu("stage"))
    {
        stage_->DrawDebug();
        ImGui::EndMenu();
    }

}
