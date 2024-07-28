#include "GameScene.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/EnemyDragon.h"
#include "Camera.h"
#include "Collision/Collision.h"
#include "UI/UINumber.h"


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

    particles_ = std::make_unique<decltype(particles_)::element_type>(100);

    stone_ = std::make_unique<Stone>();
}

// ----- ������ -----
void GameScene::Initialize()
{
    // ���݂�Scene��ݒ�
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Game);

    stage_->GetTransform()->SetScaleFactor(100.0f);
    //stage_->GetTransform()->SetScaleFactor(1.5f);
    //stage_->GetTransform()->SetScaleFactor(6000.0f);

    stageCenter_ = stage_->GetTransform()->GetPosition();

    // �v���C���[������
    PlayerManager::Instance().Initialize();

    // �G������
    EnemyManager::Instance().Initialize();

    //particles_->Initialize(0);
}

// ----- �I���� -----
void GameScene::Finalize()
{
    // �v���C���[�I����
    PlayerManager::Instance().Finalize();

    // �G�I����
    EnemyManager::Instance().Finalize();
}

// ----- �X�V -----
void GameScene::Update(const float& elapsedTime)
{
    // �v���C���[�X�V
    PlayerManager::Instance().Update(elapsedTime);

    // �G�X�V
    EnemyManager::Instance().Update(elapsedTime);

    // �X�e�[�W�ʒu�X�V
    stageCenter_ = stage_->GetTransform()->GetPosition();

    // Collision
    UpdateCollisions(elapsedTime);


    // �J�����̈ʒu�X�V
    //const DirectX::XMFLOAT3 cameraTargetPosition = { PlayerManager::Instance().GetTransform()->GetPositionX(), 0.0f, PlayerManager::Instance().GetTransform()->GetPositionZ() };
    //Camera::Instance().SetTarget(cameraTargetPosition);

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

    // �X�e�[�W
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f, gBufferPixelShader);
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // �v���C���[�`��
    PlayerManager::Instance().Render(gBufferPixelShader);

    // �G�`��
    EnemyManager::Instance().Render(gBufferPixelShader);

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
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);

    PlayerManager::Instance().RenderTrail();
    //particles_->Render();
}

// ----- ImGui�p -----
void GameScene::DrawDebug()
{
    ImGui::Checkbox("Debug", &isDebugRenderer_);
    ImGui::DragFloat("stageRadius", &stageRadius1_);

    particles_->DrawDebug();

    stone_->DrawDebug();

    // �v���C���[ImGui
    PlayerManager::Instance().DrawDebug();

    // �GImGui
    EnemyManager::Instance().DrawDebug();

    if (ImGui::BeginMenu("stage"))
    {
        stage_->DrawDebug();
        ImGui::EndMenu();
    }

}

// ----- �����蔻��X�V -----
void GameScene::UpdateCollisions(const float& elapsedTime)
{
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;
    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

#pragma region �v���C���[�̂��炢����
    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // �G�̍U�����肪���ݗL���ł͂Ȃ��̂ŏ��������Ȃ�
            if (enemyData.GetIsActive() == false) continue;

            // �����������`�F�b�N
            if (Collision::IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius()))
            {
                if (player->GetIsCounter() && player->GetCurrentState() == Player::STATE::Counter)
                {
                    player->SetIsAbleCounterAttack(true);
                }
                // �v���C���[������DamageState�ł͂Ȃ��ꍇDamageState��
                else if (player->GetCurrentState() != Player::STATE::CounterCombo &&
                    player->GetCurrentState() != Player::STATE::Damage &&
                    player->GetIsAbleCounterAttack() == false)
                {
                    player->ChangeState(Player::STATE::Damage);
                }
            }
        }
    }
#pragma endregion �v���C���[�̂��炢����

#pragma region �����o������
    // �����o�����肪�L���Ȃ�A�����o��������s��
    if (PlayerManager::Instance().GetUseCollisionDetection())
    {
        for (int playerDataIndex = 0; playerDataIndex < player->GetCollisionDetectionDataCount(); ++playerDataIndex)
        {
            const CollisionDetectionData playerData = player->GetCollisionDetectionData(playerDataIndex);

            for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
            {
                const CollisionDetectionData enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

                // �G�̉����o�����肪���ݗL���ł͂Ȃ��̂ŏ��������Ȃ�
                if (enemyData.GetIsActive() == false) continue;

                DirectX::XMFLOAT3 resultPos = {};

                // Y�l���Œ�̃f�[�^�Ƃ̔���
                if (enemyData.GetFixedY())
                {
                    if (Collision::IntersectSphereVsSphere(
                        enemyData.GetPosition(), enemyData.GetRadius(),
                        player->GetTransform()->GetPosition(), playerData.GetRadius(),
                        resultPos))
                    {
                        player->GetTransform()->SetPosition(resultPos);
                    }
                }
                // ���̑��̃f�[�^�Ƃ̔���
                else
                {
                    // CollisionData�̈ʒu���X�V����
                    player->UpdateCollisionDetectionData();

                    if (Collision::IntersectSphereVsSphereNotConsiderY(
                        enemyData.GetPosition(), enemyData.GetRadius(),
                        playerData.GetPosition(), playerData.GetRadius(),
                        resultPos))
                    {
                        resultPos = player->GetTransform()->GetPosition() - resultPos;
                        player->GetTransform()->SetPosition(resultPos);
                    }
                }
            }
        }
    }

#pragma endregion �����o������

#pragma region �v���C���[�̍U������
    // �U�����肪�L���ł͂Ȃ��̂ł����ŏI��
    if (player->GetIsAbleAttack() == false) return;

    for (int playerDataIndex = 0; playerDataIndex < player->GetAttackDetectionDataCount(); ++playerDataIndex)
    {
        const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
        {
            const DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // �����������`�F�b�N
            if (Collision::IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius()))
            {
                // ���݃`�F�b�N���Ă���G�̃f�[�^���܂����������ĂȂ��ꍇ
                if (enemyData.GetIsHit() == false)
                {
                    // Hit�t���O�𗧂Ă�A���̃f�[�^�̖��G���Ԑݒ�A�_���[�W����
                    enemy->GetDamageDetectionData(enemyDataIndex).SetIsHit(true);
                    enemy->GetDamageDetectionData(enemyDataIndex).SetHitTimer(0.01f);
                    enemy->AddDamage(enemyData.GetDamage());

                    // �U���������� ( �U������𖳂��� )
                    player->SetIsAbleAttack(false);

                    if (player->GetCurrentState() == Player::STATE::CounterCombo)
                    {
                        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
                    }

                    // TODO: �q�b�g�X�g�b�v����

                    // �G�������Ă�����UI����
                    if (PlayerManager::Instance().GetUseCollisionDetection())
                    {
                        UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());
                    }

                    return;
                }
            }
        }
    }

#pragma endregion �v���C���[�̍U������
}
