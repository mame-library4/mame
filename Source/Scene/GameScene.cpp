#include "GameScene.h"
#include "../Graphics/Graphics.h"
#include "../Resource/texture.h"
#include "../Game/Character/Player/PlayerManager.h"
#include "../Game/Character/Enemy/EnemyManager.h"

// ----- ステージの真ん中位置 -----
DirectX::XMFLOAT3 GameScene::stageCenter_ = {};

// ----- リソース生成 -----
void GameScene::CreateResource()
{
    // プレイヤー生成
    PlayerManager::Instance().GetPlayer() = std::make_unique<Player>();

    stageNormal_[0] = std::make_unique<StageNormal>("./Resources/Model/Stage/shrine.gltf");
    stageNormal_[1] = std::make_unique<StageNormal>("./Resources/Model/Stage/torii.glb");
    stageNormal_[2] = std::make_unique<StageNormal>("./Resources/Model/Stage/circle.glb");

    EnemyManager::Instance().Register(new EnemyTamamo());

    // IBLテクスチャ
    D3D11_TEXTURE2D_DESC textureDesc = {};
    LoadTextureFromFile(Graphics::Instance().GetDevice(),
        L"./Resources/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
        iblTextures_[0].GetAddressOf(), &textureDesc);
    LoadTextureFromFile(Graphics::Instance().GetDevice(),
        L"./Resources/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
        iblTextures_[1].GetAddressOf(), &textureDesc);
    LoadTextureFromFile(Graphics::Instance().GetDevice(),
        L"./Resources/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
        iblTextures_[2].GetAddressOf(), &textureDesc);
    LoadTextureFromFile(Graphics::Instance().GetDevice(),
        L"./Resources/environments/lut_ggx.DDS",
        iblTextures_[3].GetAddressOf(), &textureDesc);
}

// ----- 初期化 -----
void GameScene::Initialize()
{
    // プレイヤー初期化
    PlayerManager::Instance().Initialize();

    // 敵初期化
    EnemyManager::Instance().Initialize();

    stageNormal_[2]->GetTransform()->SetPositionZ(35);

    stageNormal_[1]->GetTransform()->SetPositionY(-100);

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
    stageCenter_ = stageNormal_[2]->GetTransform()->GetPosition();
}

void GameScene::ShadowRender(ID3D11DeviceContext* deviceContext)
{
}

void GameScene::DeferredRender(ID3D11DeviceContext* deviceContext)
{
}

void GameScene::ForwardRender(ID3D11DeviceContext* deviceContext)
{
    deviceContext->PSSetShaderResources(32, 1, iblTextures_[0].GetAddressOf());
    deviceContext->PSSetShaderResources(33, 1, iblTextures_[1].GetAddressOf());
    deviceContext->PSSetShaderResources(34, 1, iblTextures_[2].GetAddressOf());
    deviceContext->PSSetShaderResources(35, 1, iblTextures_[3].GetAddressOf());
    
    // プレイヤー描画
    PlayerManager::Instance().Render();

    // 敵描画
    EnemyManager::Instance().Render();

    for (int i = 0; i < stageMax; ++i)
    {
        stageNormal_[i]->Render();
    }

    

#ifdef _DEBUG
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    if (isDebugRenderer_)
    {

        // player
        PlayerManager::Instance().DebugRender(debugRenderer);

        // enemy
        EnemyManager::Instance().DebugRender(debugRenderer);

        DirectX::XMFLOAT3 position{};
        position = stageNormal_[2]->GetTransform()->GetPosition();

        debugRenderer->DrawCylinder(position, stageRadius_, 1.5f, { 1, 0, 0, 1 });

    }
    // デバッグレンダラ描画
    DirectX::XMFLOAT4X4 view, projection;
    DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
    DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());

    debugRenderer->Render(deviceContext, view, projection);
#endif
}

void GameScene::UserInterfaceRender(ID3D11DeviceContext* deviceContext)
{
}

// ----- ImGui用 -----
void GameScene::DrawDebug()
{
    ImGui::Checkbox("Debug", &isDebugRenderer_);

    // プレイヤーImGui
    PlayerManager::Instance().DrawDebug();

    // 敵ImGui
    EnemyManager::Instance().DrawDebug();

    if (ImGui::TreeNode("stage1"))
    {
        stageNormal_[1]->DrawDebug();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("stage2"))
    {
        stageNormal_[2]->DrawDebug();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("jinja"))
    {
        stageNormal_[0]->DrawDebug();
        ImGui::TreePop();
    }
}
