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

    particles_ = std::make_unique<decltype(particles_)::element_type>(100);

    stone_ = std::make_unique<Stone>();
}

// ----- 初期化 -----
void GameScene::Initialize()
{
    // 現在のSceneを設定
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Game);

    stage_->GetTransform()->SetScaleFactor(100.0f);
    //stage_->GetTransform()->SetScaleFactor(1.5f);
    //stage_->GetTransform()->SetScaleFactor(6000.0f);

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

    // Collision
    UpdateCollisions(elapsedTime);


    // カメラの位置更新
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

    // ステージ
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f, gBufferPixelShader);
    //Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    // プレイヤー描画
    PlayerManager::Instance().Render(gBufferPixelShader);

    // 敵描画
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

    // ステージ
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    stage_->Render(0.01f);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

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
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);

    PlayerManager::Instance().RenderTrail();
    //particles_->Render();
}

// ----- ImGui用 -----
void GameScene::DrawDebug()
{
    ImGui::Checkbox("Debug", &isDebugRenderer_);
    ImGui::DragFloat("stageRadius", &stageRadius1_);

    particles_->DrawDebug();

    stone_->DrawDebug();

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

// ----- 当たり判定更新 -----
void GameScene::UpdateCollisions(const float& elapsedTime)
{
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;
    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

#pragma region プレイヤーのくらい判定
    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // 敵の攻撃判定が現在有効ではないので処理をしない
            if (enemyData.GetIsActive() == false) continue;

            // 当たったかチェック
            if (Collision::IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius()))
            {
                if (player->GetIsCounter() && player->GetCurrentState() == Player::STATE::Counter)
                {
                    player->SetIsAbleCounterAttack(true);
                }
                // プレイヤーが現在DamageStateではない場合DamageStateへ
                else if (player->GetCurrentState() != Player::STATE::CounterCombo &&
                    player->GetCurrentState() != Player::STATE::Damage &&
                    player->GetIsAbleCounterAttack() == false)
                {
                    player->ChangeState(Player::STATE::Damage);
                }
            }
        }
    }
#pragma endregion プレイヤーのくらい判定

#pragma region 押し出し判定
    // 押し出し判定が有効なら、押し出し判定を行う
    if (PlayerManager::Instance().GetUseCollisionDetection())
    {
        for (int playerDataIndex = 0; playerDataIndex < player->GetCollisionDetectionDataCount(); ++playerDataIndex)
        {
            const CollisionDetectionData playerData = player->GetCollisionDetectionData(playerDataIndex);

            for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
            {
                const CollisionDetectionData enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

                // 敵の押し出し判定が現在有効ではないので処理をしない
                if (enemyData.GetIsActive() == false) continue;

                DirectX::XMFLOAT3 resultPos = {};

                // Y値が固定のデータとの判定
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
                // その他のデータとの判定
                else
                {
                    // CollisionDataの位置を更新する
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

#pragma endregion 押し出し判定

#pragma region プレイヤーの攻撃判定
    // 攻撃判定が有効ではないのでここで終了
    if (player->GetIsAbleAttack() == false) return;

    for (int playerDataIndex = 0; playerDataIndex < player->GetAttackDetectionDataCount(); ++playerDataIndex)
    {
        const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
        {
            const DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // 当たったかチェック
            if (Collision::IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius()))
            {
                // 現在チェックしている敵のデータがまだあったってない場合
                if (enemyData.GetIsHit() == false)
                {
                    // Hitフラグを立てる、このデータの無敵時間設定、ダメージ処理
                    enemy->GetDamageDetectionData(enemyDataIndex).SetIsHit(true);
                    enemy->GetDamageDetectionData(enemyDataIndex).SetHitTimer(0.01f);
                    enemy->AddDamage(enemyData.GetDamage());

                    // 攻撃当たった ( 攻撃判定を無くす )
                    player->SetIsAbleAttack(false);

                    if (player->GetCurrentState() == Player::STATE::CounterCombo)
                    {
                        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
                    }

                    // TODO: ヒットストップ処理

                    // 敵が生きていたらUI生成
                    if (PlayerManager::Instance().GetUseCollisionDetection())
                    {
                        UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());
                    }

                    return;
                }
            }
        }
    }

#pragma endregion プレイヤーの攻撃判定
}
