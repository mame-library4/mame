#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <cstdint>
#include <functional>

#include "../Graphics/FullscreenQuad.h"
#include "../Graphics/FrameBuffer.h"

class ShadowMap
{
private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

public:
    ShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);
    virtual ~ShadowMap() = default;
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator =(const ShadowMap&) = delete;
    ShadowMap(ShadowMap&&) noexcept = delete;
    ShadowMap& operator =(ShadowMap&&) noexcept = delete;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    D3D11_VIEWPORT viewport;

    void Clear(ID3D11DeviceContext* deviceContext, float depth = 1);
    void Activate(ID3D11DeviceContext* deviceContext);
    void Deactivete(ID3D11DeviceContext* deviceContext);

private:
    UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;
};

