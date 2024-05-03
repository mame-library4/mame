#include "SkyBox.h"

#include "../Graphics/shader.h"
#include "../Other/misc.h"

SkyBox::SkyBox(ID3D11Device* device, std::shared_ptr<Sprite> sprite)
    :sprite(sprite)
{
    HRESULT hr{ S_OK };

    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    CreateVsFromCso(device, "./Resources/Shader/SkyBoxVS.cso", vertexShader.GetAddressOf(),
        inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    CreatePsFromCso(device, "./Resources/Shader/SkyBoxPS.cso", pixelShader.GetAddressOf());

    // 定数バッファ
    {
        D3D11_BUFFER_DESC desc;
        ::memset(&desc, 0, sizeof(desc)); // memset関数はメモリに指定バイト数分の値をセットすることが出来る
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(CbScene);
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // ブレンドステート
    {
        D3D11_BLEND_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.IndependentBlendEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // 深度ステンシルステート
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

        hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // ラスタライザーステート
    {
        D3D11_RASTERIZER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.AntialiasedLineEnable = false;

        hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // サンプラステート
    {
        D3D11_SAMPLER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 1.0f;
        desc.BorderColor[2] = 1.0f;
        desc.BorderColor[3] = 1.0f;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

        hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}

void SkyBox::Render(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX V, DirectX::XMMATRIX P)
{
    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    deviceContext->IASetInputLayout(inputLayout.Get());

    deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    deviceContext->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

    // シーン用定数バッファ更新
    CbScene cbScene;

    DirectX::XMStoreFloat4x4(&cbScene.inverseViewProjection, DirectX::XMMatrixInverse(nullptr, V * P));
    DirectX::XMStoreFloat4(&cbScene.viewPosition, V.r[3]);

    deviceContext->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
    deviceContext->VSSetConstantBuffers(1, 1, sceneConstantBuffer.GetAddressOf());

    UINT stride{ sizeof(Sprite::Vertex) };
    UINT offset{ 0 };
    deviceContext->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
    deviceContext->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
    deviceContext->Draw(4, 0);
}