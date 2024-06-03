#include "PrimitiveRenderer.h"
#include "Graphics.h"
#include "Misc.h"
#include "Camera.h"
#include "Texture.h"

// ----- コンストラクタ -----
PrimitiveRenderer::PrimitiveRenderer()
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Graphics::Instance().CreateVsFromCso("./Resources/Shader/PrimitiveRendererVS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PrimitiveRendererPS.cso", pixelShader_.GetAddressOf());

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(Vertex) * vertexCapacity_;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    result = device->CreateBuffer(&bufferDesc, nullptr, vertexBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    constantBuffer_ = std::make_unique<ConstantBuffer<SceneConstants>>();

    D3D11_TEXTURE2D_DESC desc = {};
    //Texture::Instance().LoadTexture(L"./Resources/Image/noise.png", shaderResourceView_.GetAddressOf(), &desc);    
    Texture::Instance().LoadTexture(L"./Resources/Image/photo_12.png", shaderResourceView_.GetAddressOf(), &desc);    
    //Texture::Instance().LoadTexture(L"./Resources/Image/Mask/noise1.png", shaderResourceView_.GetAddressOf(), &desc);    
}

// ----- 頂点追加 -----
void PrimitiveRenderer::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord)
{
    Vertex& v = vertices_.emplace_back();
    v.position_ = position;
    v.color_ = color;
    v.texcoord_ = texcoord;
}

void PrimitiveRenderer::SetTexcoord()
{
    if (vertices_.size() == 0) return;

    const size_t size = vertices_.size();
    const float texcoord = 1.0f / (size / 2);

    for (int i = 0; i < size; i += 2)
    {
        vertices_.at(i).texcoord_.x = texcoord * (size -i );
        vertices_.at(i + 1).texcoord_.x = texcoord * (size -i);
    }

    //for (int i = size - 1; i > -1; i -=2)
    //{
    //    vertices_.at(i).texcoord_.x = texcoord * i;
    //    vertices_.at(i - 1).texcoord_.x = texcoord * i;
    //}
#if 0
    for (int i = size - 1; i > -1; --i)
    {
        vertices_.at(i).texcoord_.x = texcoord * i;
    }
#endif
}

// ----- 描画 -----
void PrimitiveRenderer::Render(ID3D11PixelShader* pixelShader)
{
    HRESULT result = S_OK;
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    // シェーダ設定
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    pixelShader ? deviceContext->PSSetShader(pixelShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
    deviceContext->IASetInputLayout(inputLayout_.Get());

    // テクスチャ
    deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

    // 定数バッファー設定
    const DirectX::XMMATRIX V = Camera::Instance().GetViewMatrix();
    const DirectX::XMMATRIX P = Camera::Instance().GetProjectionMatrix();    
    DirectX::XMStoreFloat4x4(&constantBuffer_->GetData()->viewProjection_, V * P);
    constantBuffer_->Activate(0);

    // 頂点バッファー設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

    // 描画
    UINT totalVertexCount = static_cast<UINT>(vertices_.size());
    UINT start = 0;
    UINT count = (totalVertexCount < vertexCapacity_) ? totalVertexCount : vertexCapacity_;

    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);

    while (start < totalVertexCount)
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        result = deviceContext->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        memcpy(mappedSubresource.pData, &vertices_[start], sizeof(Vertex) * count);

        deviceContext->Unmap(vertexBuffer_.Get(), 0);

        deviceContext->Draw(count, 0);

        start += count;
        if ((start + count) > totalVertexCount)
        {
            count = totalVertexCount - start;
        }
    }
}

void PrimitiveRenderer::ClearVertices()
{
    vertices_.clear();
}
