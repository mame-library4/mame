#include "PostProcess.h"
#include "../Graphics/Graphics.h"

// ----- コンストラクタ -----
PostProcess::PostProcess(ID3D11Device* device, uint32_t width, uint32_t height)
{
    renderer_ = std::make_unique<FullscreenQuad>(device);

    CreatePsFromCso(device, "./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(device, width, height);
}

// ----- デストラクタ -----
PostProcess::~PostProcess()
{
}

// ----- ポストプロセス開始 -----
void PostProcess::Activate(ID3D11DeviceContext* deviceContext)
{
    postProcess_->Clear(deviceContext);
    postProcess_->Activate(deviceContext);
}

// ----- ポストプロセス終了 -----
void PostProcess::Deactivate(ID3D11DeviceContext* deviceContext)
{
    postProcess_->Deactivate(deviceContext);
    bloom_.Execute(deviceContext, postProcess_->shaderResourceViews[0].Get());
}

// ----- ポストプロセス描画 -----
void PostProcess::Draw(ID3D11DeviceContext* deviceContext)
{
    renderer_->Draw(deviceContext, bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
}

// ----- ImGui用 -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));

        bloom_.DrawDebug();

        ImGui::TreePop();
    }
}
