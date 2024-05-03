#include "PostProcess.h"
#include "../Graphics/Graphics.h"

// ----- �R���X�g���N�^ -----
PostProcess::PostProcess(ID3D11Device* device, uint32_t width, uint32_t height)
{
    renderer_ = std::make_unique<FullscreenQuad>(device);

    CreatePsFromCso(device, "./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(device, width, height);
}

// ----- �f�X�g���N�^ -----
PostProcess::~PostProcess()
{
}

// ----- �|�X�g�v���Z�X�J�n -----
void PostProcess::Activate(ID3D11DeviceContext* deviceContext)
{
    postProcess_->Clear(deviceContext);
    postProcess_->Activate(deviceContext);
}

// ----- �|�X�g�v���Z�X�I�� -----
void PostProcess::Deactivate(ID3D11DeviceContext* deviceContext)
{
    postProcess_->Deactivate(deviceContext);
    bloom_.Execute(deviceContext, postProcess_->shaderResourceViews[0].Get());
}

// ----- �|�X�g�v���Z�X�`�� -----
void PostProcess::Draw(ID3D11DeviceContext* deviceContext)
{
    renderer_->Draw(deviceContext, bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
}

// ----- ImGui�p -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));

        bloom_.DrawDebug();

        ImGui::TreePop();
    }
}
