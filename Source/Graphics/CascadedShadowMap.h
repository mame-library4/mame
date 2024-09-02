#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <functional>

#include "ConstantBuffer.h"

class CascadedShadowMap
{
public:
    CascadedShadowMap();
    ~CascadedShadowMap() {}

    void Make(const DirectX::XMFLOAT4& lightDirection, std::function<void()> drawcallback);

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D>         depthStencilBuffer_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  depthStencilView_;
    D3D11_VIEWPORT viewPort_;

    std::vector<DirectX::XMFLOAT4X4> viewProjection_;
    std::vector<float> distances_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    struct Constats
    {
        DirectX::XMFLOAT4X4 viewProjectionMatrices_[16];
        float cascadePlaneDistances_[16];
        int cascadeCount_ = 4;
    };
    std::unique_ptr<ConstantBuffer<Constats>> constants_;

    float criticalDepthValue_ = 200.0f;

    const UINT cascadeCount_;
    float splitSchemeWeight_ = 0.7f;
};

