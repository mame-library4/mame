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

// ----- �X�e�[�W�̐^�񒆈ʒu -----
DirectX::XMFLOAT3 GameScene::stageCenter_ = {};

// ----- ���\�[�X���� -----
void GameScene::CreateResource()
{
    // �v���C���[����
    PlayerManager::Instance().GetPlayer() = std::make_unique<Player>();

    stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/OnlyStage/stageOneMesh.gltf");
    //stage_ = std::make_unique<StageNormal>("./Resources/Model/Stage/stageAndFlag/stage.gltf");


    EnemyManager::Instance().Register(new EnemyDragon);

    // IBL�e�N�X�`��
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

// ----- ������ -----
void GameScene::Initialize()
{
    // ���݂�Scene��ݒ�
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Game);

    stage_->GetTransform()->SetScaleFactor(100.0f);
    //stage_->GetTransform()->SetScaleFactor(1.5f);
    //stage_->GetTransform()->SetScaleFactor(6000.0f);

    particles_->Initialize(0);

    stageCenter_ = stage_->GetTransform()->GetPosition();

    // �v���C���[������
    PlayerManager::Instance().Initialize();

    // �G������
    EnemyManager::Instance().Initialize();

    // ���˕�
    ProjectileManager::Instance().Initialize();

    // �J����������
    Camera::Instance().SetTarget({ PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() });
}

// ----- �I���� -----
void GameScene::Finalize()
{
    // �v���C���[�I����
    PlayerManager::Instance().Finalize();

    // �G�I����
    EnemyManager::Instance().Finalize();

    // ���˕�
    ProjectileManager::Instance().Finalize();
}

// ----- �X�V -----
void GameScene::Update(const float& elapsedTime)
{
    if (Input::Instance().GetMouse().GetButtonDown() & Mouse::BTN_RIGHT)
    {
        UIPartDestruction* data = new UIPartDestruction();
    }

    // �v���C���[�X�V
    PlayerManager::Instance().Update(elapsedTime);

    // �G�X�V
    EnemyManager::Instance().Update(elapsedTime);

    // ���˕�
    ProjectileManager::Instance().Update(elapsedTime);

    // �X�e�[�W�ʒu�X�V
    stageCenter_ = stage_->GetTransform()->GetPosition();

    // Collision�X�V
    CollisionManager::Instance().Update(elapsedTime);

    if (GetAsyncKeyState('Q') & 0x8000)
    {
        PlayerManager::Instance().GetPlayer()->SetHealth(0);
    }
    

    // �J�����̈ʒu�X�V
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

    // �X�e�[�W
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f, gBufferPixelShader);
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // �v���C���[�`��
    PlayerManager::Instance().Render(gBufferPixelShader);

    // �G�`��
    EnemyManager::Instance().Render(gBufferPixelShader);

    // ���˕�
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

    // �X�e�[�W
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // �v���C���[�`��
    PlayerManager::Instance().Render();

    // �G�`��
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

// ----- ImGui�p -----
void GameScene::DrawDebug()
{
    if (ImGui::BeginMainMenuBar())
    {
        // �v���C���[ImGui
        PlayerManager::Instance().DrawDebug();

        // �GImGui
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

// ----- �v���C���[�̃J�E���^�[���� -----
void GameScene::UpdateCounterCollisions()
{
    Player* player = PlayerManager::Instance().GetPlayer().get();

    // ���݃v���C���[���J�E���^�[State�ł͂Ȃ��̂ł����ŏI��
    if (player->GetCurrentState() != Player::STATE::Counter) return;
    // ���݃J�E���^�[��t���ł͂Ȃ��̂ł����ŏI��
    if (player->GetIsCounter() == false) return;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
    // ���ݓG�̍U�����肪�L���ł͂Ȃ��̂ł����ŏI��
    if (enemy->GetIsAttackActive() == false) return;


    const float distance = enemy->CalcDistanceToPlayer();
    const float counterActiveRadius = player->GetCounterActiveRadius();

    // �J�E���^�[����������
    if (distance < counterActiveRadius)
    {
        player->SetIsAbleCounterAttack(true);
    }
}
