#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

class FrameBuffer
{
public:
    FrameBuffer(const uint32_t& width, const uint32_t& height, const bool& hasDepthStancil = true/*BLOOM*/);
    virtual ~FrameBuffer() = default;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews_[2];
    D3D11_VIEWPORT viewport_;

    void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
    void Activate();
    void Deactivate();

private:
    UINT viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    D3D11_VIEWPORT cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView_;
};