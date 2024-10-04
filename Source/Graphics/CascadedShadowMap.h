#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <functional>

class CascadedShadowMap
{
public:
    CascadedShadowMap(UINT width, UINT height, UINT cascadeCount = 4);
    virtual ~CascadedShadowMap() = default;

    void DrawDebug();

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
    D3D11_VIEWPORT viewport_;

    std::vector<DirectX::XMFLOAT4X4> cascadedMatrices_;
    std::vector<float> cascadedPlaneDistances_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    struct constants
    {
        DirectX::XMFLOAT4X4 cascadedMatrices_[4];
        float cascadedPlaneDistances_[4];
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

public:
    void Activate(const DirectX::XMFLOAT4& lightDirection, float criticalDepthValue, UINT cbSlot);
    void Deactivate();
    void Clear();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthMap() { return shaderResourceView_; }

public:
    const UINT  cascadeCount_;
    float       splitSchemeWeight_  = 1.0f;
    float       zMult_              = 10.0f;
    bool        fitToCascade_       = true;

private:
    D3D11_VIEWPORT cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    UINT viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView_;
};