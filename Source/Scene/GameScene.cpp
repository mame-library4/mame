#include "GameScene.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/EnemyDragon.h"
#include "Camera.h"
#include "Collision/CollisionManager.h"

#include "UI/UINumber.h"

#include "Projectile/ProjectileManager.h"
#include "UI/UIPartDestruction.h"

// ----- ステージの真ん中位置 -----
DirectX::XMFLOAT3 GameScene::stageCenter_ = {};

// ----- リソース生成 -----
void GameScene::CreateResource()
{
    // プレイヤー生成
    PlayerManager::Instance().GetPlayer() = std::make_unique<Player>();

    stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/OnlyStage/stageOneMesh.gltf");
    //stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/stageAndFlag/stage.gltf");


    EnemyManager::Instance().Register(new EnemyDragon);

    // IBLテクスチャ
    D3D11_TEXTURE2D_DESC textureDesc = {};

#if 1
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
        iblTextures_[0].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
        iblTextures_[1].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
        iblTextures_[2].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/environments/lut_ggx.DDS",
        iblTextures_[3].GetAddressOf(), &textureDesc);
#else
    Texture::Instance().LoadTexture(L"./Resources/SkyBox/RainSky/skybox.dds",
        iblTextures_[0].GetAddressOf(), &textureDesc);
    Texture::Instance().LoadTexture(L"./Resources/SkyBox/RainSky/diffuse_iem.dds",
        iblTextures_[1].GetAddressOf(), &textureDesc); 
    Texture::Instance().LoadTexture(L"./Resources/SkyBox/RainSky/specular_pmrem.dds",
        iblTextures_[2].GetAddressOf(), &textureDesc); 
    Texture::Instance().LoadTexture(L"./Resources/SkyBox/RainSky/lut_ggx.dds",
        iblTextures_[3].GetAddressOf(), &textureDesc);
#endif

    particles_ = std::make_unique<decltype(particles_)::element_type>();

    stone_ = std::make_unique<Stone>();


    Effect* effect0 = new Effect("./Resources/Effect/Counter.efk", "Counter");
    
    //Effect* effect1 = new Effect("./Resources/Effect/Attack.efk", "Mikiri");
    Effect* effect1 = new Effect("./Resources/Effect/Mikiri.efk", "Mikiri");
    
    Effect* effect2 = new Effect("./Resources/Effect/Fire.efk", "Fire");
}

// ----- 初期化 -----
void GameScene::Initialize()
{
    // 現在のSceneを設定
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Game);

    stage_->GetTransform()->SetScaleFactor(100.0f);
    //stage_->GetTransform()->SetScaleFactor(1.5f);
    //stage_->GetTransform()->SetScaleFactor(6000.0f);

    particles_->Initialize(0);

    stageCenter_ = stage_->GetTransform()->GetPosition();

    // プレイヤー初期化
    PlayerManager::Instance().Initialize();

    // 敵初期化
    EnemyManager::Instance().Initialize();

    // 発射物
    ProjectileManager::Instance().Initialize();

    // カメラ初期化
    Camera::Instance().SetTarget({ PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() });
}

// ----- 終了化 -----
void GameScene::Finalize()
{
    // プレイヤー終了化
    PlayerManager::Instance().Finalize();

    // 敵終了化
    EnemyManager::Instance().Finalize();

    // 発射物
    ProjectileManager::Instance().Finalize();
}

// ----- 更新 -----
void GameScene::Update(const float& elapsedTime)
{
    if (Input::Instance().GetMouse().GetButtonDown() & Mouse::BTN_RIGHT)
    {
        UIPartDestruction* data = new UIPartDestruction();
    }

    // プレイヤー更新
    PlayerManager::Instance().Update(elapsedTime);

    // 敵更新
    EnemyManager::Instance().Update(elapsedTime);

    // 発射物
    ProjectileManager::Instance().Update(elapsedTime);

    // ステージ位置更新
    stageCenter_ = stage_->GetTransform()->GetPosition();

    // Collision更新
    CollisionManager::Instance().Update(elapsedTime);

    if (GetAsyncKeyState('Q') & 0x8000)
    {
        PlayerManager::Instance().GetPlayer()->SetHealth(0);
    }
    

    // カメラの位置更新
    //const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    //Camera::Instance().SetTarget(cameraTargetPosition);

    if (GetAsyncKeyState('T') & 0x8000)
    {
        particles_->Initialize(0);
    }
    particles_->Update(elapsedTime);
}

void GameScene::ShadowRender()
{
    PlayerManager::Instance().CastShadow();
    EnemyManager::Instance().CastShadow();

    stage_->CastShadow(0.01f);
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
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f, gBufferPixelShader);
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // プレイヤー描画
    PlayerManager::Instance().Render(gBufferPixelShader);

    // 敵描画
    EnemyManager::Instance().Render(gBufferPixelShader);

    // 発射物
    ProjectileManager::Instance().Render(gBufferPixelShader);

    stone_->Render(gBufferPixelShader);
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
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // プレイヤー描画
    PlayerManager::Instance().Render();

    // 敵描画
    EnemyManager::Instance().Render();

    ProjectileManager::Instance().Render();

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

void GameScene::Render()
{
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);

    PlayerManager::Instance().RenderTrail();
    particles_->Render();

    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
//#ifdef _DEBUG
#if 1
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

        if (EnemyManager::Instance().GetEnemyCount() > 0)
        {
            DirectX::XMFLOAT3 dragonPos = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
            const float radius = PlayerManager::Instance().GetPlayer()->GetCounterActiveRadius();
            debugRenderer->DrawCylinder(dragonPos, radius, 1.5f, { 1, 0, 0, 1 });
        }

        ProjectileManager::Instance().DebugRender(debugRenderer);
    }
#endif
}

// ----- ImGui用 -----
void GameScene::DrawDebug()
{
    if (ImGui::BeginMainMenuBar())
    {
        // プレイヤーImGui
        PlayerManager::Instance().DrawDebug();

        // 敵ImGui
        EnemyManager::Instance().DrawDebug();

        ProjectileManager::Instance().DrawDebug();

        ImGui::EndMainMenuBar();
    }

    ImGui::Checkbox("Debug", &isDebugRenderer_);
    ImGui::DragFloat("stageRadius", &stageRadius1_);

    particles_->DrawDebug();

    stone_->DrawDebug();


    if (ImGui::BeginMenu("stage"))
    {
        stage_->DrawDebug();
        ImGui::EndMenu();
    }

}

// ----- プレイヤーのカウンター判定 -----
void GameScene::UpdateCounterCollisions()
{
    Player* player = PlayerManager::Instance().GetPlayer().get();

    // 現在プレイヤーがカウンターStateではないのでここで終了
    if (player->GetCurrentState() != Player::STATE::Counter) return;
    // 現在カウンター受付中ではないのでここで終了
    if (player->GetIsCounter() == false) return;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
    // 現在敵の攻撃判定が有効ではないのでここで終了
    if (enemy->GetIsAttackActive() == false) return;


    const float distance = enemy->CalcDistanceToPlayer();
    const float counterActiveRadius = player->GetCounterActiveRadius();

    // カウンターが成功した
    if (distance < counterActiveRadius)
    {
        player->SetIsAbleCounterAttack(true);
    }
}
