#include "GameScene.h"
#include "../Graphics/Graphics.h"
#include "../Resource/texture.h"
#include "../Game/Character/Player/PlayerManager.h"
#include "../Game/Character/Enemy/EnemyManager.h"

// ----- �X�e�[�W�̐^�񒆈ʒu -----
DirectX::XMFLOAT3 GameScene::stageCenter_ = {};

// ----- ���\�[�X���� -----
void GameScene::CreateResource()
{
    // �v���C���[����
    PlayerManager::Instance().GetPlayer() = std::make_unique<Player>();

    stageNormal_[0] = std::make_unique<StageNormal>("./Resources/Model/Stage/shrine.gltf");
    stageNormal_[1] = std::make_unique<StageNormal>("./Resources/Model/Stage/torii.glb");
    stageNormal_[2] = std::make_unique<StageNormal>("./Resources/Model/Stage/circle.glb");

    EnemyManager::Instance().Register(new EnemyTamamo());

    // IBL�e�N�X�`��
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

// ----- ������ -----
void GameScene::Initialize()
{
    // �v���C���[������
    PlayerManager::Instance().Initialize();

    // �G������
    EnemyManager::Instance().Initialize();

    stageNormal_[2]->GetTransform()->SetPositionZ(35);

    stageNormal_[1]->GetTransform()->SetPositionY(-100);

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
    
    // �v���C���[�`��
    PlayerManager::Instance().Render();

    // �G�`��
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
    // �f�o�b�O�����_���`��
    DirectX::XMFLOAT4X4 view, projection;
    DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
    DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());

    debugRenderer->Render(deviceContext, view, projection);
#endif
}

void GameScene::UserInterfaceRender(ID3D11DeviceContext* deviceContext)
{
}

// ----- ImGui�p -----
void GameScene::DrawDebug()
{
    ImGui::Checkbox("Debug", &isDebugRenderer_);

    // �v���C���[ImGui
    PlayerManager::Instance().DrawDebug();

    // �GImGui
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
