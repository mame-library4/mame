#include "DeferredRendering.h"
#include "Graphics.h"

DeferredRendering::DeferredRendering(ID3D11Device* device, uint32_t width, uint32_t height)
    : baseColorBuffer_(device, width, height),
    normalBuffer_(device, width, height),
    worldBuffer_(device, width, height),
    shadowBuffer_(device, width, height),
    metalnessRoughnessBuffer_(device, width, height)
{
}

void DeferredRendering::Activate(ID3D11DeviceContext* deviceContext)
{
    baseColorBuffer_.Clear(deviceContext);
    normalBuffer_.Clear(deviceContext);
    worldBuffer_.Clear(deviceContext);
    shadowBuffer_.Clear(deviceContext);
    metalnessRoughnessBuffer_.Clear(deviceContext);

    ID3D11RenderTargetView* renderTargetView[] =
    {
        baseColorBuffer_.renderTargetView.Get(),
        normalBuffer_.renderTargetView.Get(),
        worldBuffer_.renderTargetView.Get(),
        shadowBuffer_.renderTargetView.Get(),
        metalnessRoughnessBuffer_.renderTargetView.Get()
    };
    deviceContext->OMSetRenderTargets(_countof(renderTargetView), renderTargetView, Graphics::Instance().GetDepthStencilView());
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
