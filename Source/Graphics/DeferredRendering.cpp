#include "DeferredRendering.h"
#include "Graphics.h"

DeferredRendering::DeferredRendering(const uint32_t& width, const uint32_t& height)
    : baseColorBuffer_(width, height),
    normalBuffer_(width, height),
    worldBuffer_(width, height),
    shadowBuffer_(width, height),
    metalnessRoughnessBuffer_(width, height)
{
}

void DeferredRendering::Activate()
{
    baseColorBuffer_.Clear();
    normalBuffer_.Clear();
    worldBuffer_.Clear();
    shadowBuffer_.Clear();
    metalnessRoughnessBuffer_.Clear();

    ID3D11RenderTargetView* renderTargetView[] =
    {
        baseColorBuffer_.renderTargetView_.Get(),
        normalBuffer_.renderTargetView_.Get(),
        worldBuffer_.renderTargetView_.Get(),
        shadowBuffer_.renderTargetView_.Get(),
        metalnessRoughnessBuffer_.renderTargetView_.Get()
    };
    Graphics::Instance().GetDeviceContext()->OMSetRenderTargets(_countof(renderTargetView), renderTargetView, Graphics::Instance().GetDepthStencilView());
}

void DeferredRendering::DrawDebug()
{
    if (ImGui::TreeNode("G-Buffer"))
    {
        //ImGui::Image(reinterpret_cast<void*>(baseColorBuffer_.shaderResourceViews->GetAddressOf()), ImVec2(256.0, 256.0));
        //ImGui::Image(reinterpret_cast<void*>(normalBuffer_.shaderResourceViews->GetAddressOf()), ImVec2(256.0, 256.0));
        //ImGui::Image(reinterpret_cast<void*>(worldBuffer_.shaderResourceViews->GetAddressOf()), ImVec2(256.0, 256.0));
        //ImGui::Image(reinterpret_cast<void*>(shadowBuffer_.shaderResourceViews->GetAddressOf()), ImVec2(256.0, 256.0));
        //ImGui::Image(reinterpret_cast<void*>(metalnessRoughnessBuffer_.shaderResourceViews->GetAddressOf()), ImVec2(256.0, 256.0));
        ImGui::TreePop();
    }
}
