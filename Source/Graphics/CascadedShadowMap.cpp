#include "CascadedShadowMap.h"
#include <array>
#include "Graphics.h"
#include "Camera.h"
#include "Common.h"

std::array<DirectX::XMFLOAT4, 8> MakeFrustumCornersWorldSpace(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    const DirectX::XMMATRIX ViewProjection = DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

    std::array<DirectX::XMFLOAT4, 8> frustumCorners;
    size_t index = 0;
    for (size_t x = 0; x < 2; ++x)
    {
        for (size_t y = 0; y < 2; ++y)
        {
            for (size_t z = 0; z < 2; ++z)
            {
                DirectX::XMFLOAT4 pt = { 2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f };
                DirectX::XMStoreFloat4(&pt, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&pt), ViewProjection));
                frustumCorners.at(index++) = pt;
            }
        }
    }
    return frustumCorners;
}

// ----- コンストラクタ -----
CascadedShadowMap::CascadedShadowMap()
    : cascadeCount_(4)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    const UINT width  = 4096;
    const UINT height = 4096;

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = width;
    texture2dDesc.Height = height;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = cascadeCount_;
    texture2dDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texture2dDesc.CPUAccessFlags = 0;
    texture2dDesc.MiscFlags = 0;
    result = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount_);
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Flags = 0;
    result = device->CreateDepthStencilView(depthStencilBuffer_.Get(), &depthStencilViewDesc, depthStencilView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount_);
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    result = device->CreateShaderResourceView(depthStencilBuffer_.Get(), &shaderResourceViewDesc, shaderResourceView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    viewPort_.Width = static_cast<float>(width);
    viewPort_.Height = static_cast<float>(height);
    viewPort_.MinDepth = 0.0f;
    viewPort_.MaxDepth = 1.0f;
    viewPort_.TopLeftX = 0.0f;
    viewPort_.TopLeftY = 0.0f;
    
    constants_ = std::make_unique<ConstantBuffer<Constats>>();
    shadowConstants_ = std::make_unique<ConstantBuffer<ShadowConstants>>();
}

void CascadedShadowMap::Make(const DirectX::XMFLOAT4& lightDirection, std::function<void()> drawcallback)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);

    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::NONE);

    D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
    UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

    deviceContext->RSGetViewports(&viewportCount, cachedViewports);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;

    deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

    DirectX::XMFLOAT4X4 cameraView, cameraProjection = {};
    DirectX::XMStoreFloat4x4(&cameraView, Camera::Instance().GetViewMatrix());
    DirectX::XMStoreFloat4x4(&cameraProjection, Camera::Instance().GetProjectionMatrix());

    // 投影行列からの near / far の値
    float m33 = cameraProjection._33;
    float m43 = cameraProjection._43;
    float zNear = -m43 / m33;
    float zFar = (m33 * zNear) / (m33 - 1);
    zFar = criticalDepthValue_ > 0 ? min(zFar, criticalDepthValue_) : zFar;

    // view空間内の分割面の距離を計算する
    distances_.resize(static_cast<size_t>(cascadeCount_) + 1);
    for (size_t cascadeIndex = 0; cascadeIndex < cascadeCount_; ++cascadeIndex)
    {
        float idc = cascadeIndex / static_cast<float>(cascadeCount_);
        float logarithmicSplitScheme = zNear * pow(zFar / zNear, idc);
        float uniformSplitScheme = zNear + (zFar - zNear) * idc;
        distances_.at(cascadeIndex) = logarithmicSplitScheme * splitSchemeWeight_ + uniformSplitScheme * (1 - splitSchemeWeight_);
    }
    distances_.at(0) = zNear;
    distances_.at(cascadeCount_) = zFar;

    const bool fitToCascade = true;
    viewProjection_.resize(cascadeCount_);
    for (size_t cascadeIndex = 0; cascadeIndex < cascadeCount_; ++cascadeIndex)
    {
        float zn = fitToCascade ? distances_.at(cascadeIndex) : zNear;
        float zf = distances_.at(cascadeIndex + 1);

        DirectX::XMFLOAT4X4 cascadedProjection = cameraProjection;
        cascadedProjection._33 = zf / (zf / zn);
        cascadedProjection._43 = -zn * zf / (zf - zn);

        std::array<DirectX::XMFLOAT4, 8> corners = MakeFrustumCornersWorldSpace(cameraView, cascadedProjection);

        DirectX::XMFLOAT4 center = { 0, 0, 0, 1 };
        for (DirectX::XMFLOAT4 v : corners)
        {
            center.x += v.x;
            center.y += v.y;
            center.z += v.z;
        }
        center.x /= corners.size();
        center.y /= corners.size();
        center.z /= corners.size();

        DirectX::XMMATRIX V;
        V = DirectX::XMMatrixLookAtLH(
            DirectX::XMVectorSet(center.x - lightDirection.x, center.y - lightDirection.y, center.z - lightDirection.z, 1.0f),
            DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f),
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

        float minX = FLT_MAX;
        float maxX = -FLT_MAX;
        float minY = FLT_MAX;
        float maxY = -FLT_MAX;
        float minZ = FLT_MAX;
        float maxZ = -FLT_MAX;
        for (DirectX::XMFLOAT4 v : corners)
        {
            DirectX::XMFLOAT4 v2;
            DirectX::XMStoreFloat4(&v2, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&v), V));
            minX = min(minX, v2.x);
            maxX = max(maxX, v2.x);
            minY = min(minY, v2.y);
            maxY = max(maxY, v2.y);
            minZ = min(minZ, v2.z);
            maxZ = max(maxZ, v2.z);
        }

        constexpr float zMult = 50.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);
        DirectX::XMStoreFloat4x4(&viewProjection_.at(cascadeIndex), V * P);
    }

    constants_->GetData()->viewProjectionMatrices_[0] = viewProjection_.at(0);
    constants_->GetData()->viewProjectionMatrices_[1] = viewProjection_.at(1);
    constants_->GetData()->viewProjectionMatrices_[2] = viewProjection_.at(2);
    constants_->GetData()->viewProjectionMatrices_[3] = viewProjection_.at(3);

    constants_->GetData()->cascadePlaneDistances_[0] = distances_.at(1);
    constants_->GetData()->cascadePlaneDistances_[1] = distances_.at(2);
    constants_->GetData()->cascadePlaneDistances_[2] = distances_.at(3);
    constants_->GetData()->cascadePlaneDistances_[3] = distances_.at(4);

    constants_->Activate(12);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> nullDepthStencilView;
    deviceContext->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1, 0);
    deviceContext->OMSetRenderTargets(1, nullRenderTargetView.GetAddressOf(), depthStencilView_.Get());
    deviceContext->RSSetViewports(1, &viewPort_);

    drawcallback();

    deviceContext->RSSetViewports(viewportCount, cachedViewports);
    deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

void CascadedShadowMap::DrawDebug()
{
}
