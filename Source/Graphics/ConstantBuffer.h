#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Graphics.h"
#include "Misc.h"

// ----- ConstantBuffer の使い方 -----
// --- GPU側に渡したい変数を定義する。※16バイトの倍数に限る (.h) ---
// struct constants
// {
//     DirectX::XMFLOAT4 parameters;
// };
// std::unique_ptr<ConstantBuffer<constants>> constant_;
//
// --- 生成する ( CreateResource or Initialize ) ---
// constant_ = std::make_unique<ConstantBuffer<constants>>();
// 
// --- 更新 ( Update and Render ) ---
// constant_->GetData()->parameters = { 1, 0, 0, 0 };
// constant_->Activate(0);
// 

template <class T>
class ConstantBuffer
{
public:
    ConstantBuffer()
    {
        HRESULT result = S_OK;

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = (sizeof(T) + 0x0f) & ~0x0f;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, 0, buffer_.GetAddressOf());
        assert(SUCCEEDED(result) && HRTrace(result));
#else
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pSysMem = &data_;
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;
        result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, buffer_.GetAddressOf());
        assert(SUCCEEDED(result) && HRTrace(result));
#endif
    }
    virtual ~ConstantBuffer() = default;

    void Activate(const int& slot, const bool& vsSet = true, const bool& psSet = true,
        const bool& csSet = false, const bool& gsSet = false, const bool& hsSet = false)
    {
        ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

#ifdef USAGE_DYNAMIC
        HRESULT result = S_OK;
        D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
        D3D11_MAPPED_SUBRESOURCE mappedBuffer;

        result = deviceContext->Map(buffer_.Get(), 0, map, 0, &mappedBuffer);
        assert(SUCCEEDED(result) && HRTrace(result));
        memcpy_s(mappedBuffer.pData, sizeof(T), &data_, sizeof(T));
        deviceContext->Unmap(buffer_.Get(), 0);
#else
        deviceContext->UpdateSubresource(buffer_.Get(), 0, 0, &data_, 0, 0);
#endif

        if (vsSet)
        {   // 頂点シェーダー ( VertexShader )
            deviceContext->VSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
        }
        if (psSet)
        {   // ピクセルシェーダー ( PixelShader )
            deviceContext->PSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
        }
        if (csSet)
        {   // コンピュートシェーダー ( ComputeShader )
            deviceContext->CSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
        }
        if (gsSet)
        {   // ジオメトリシェーダー ( GeometryShader )
            deviceContext->GSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
        }
        if (hsSet)
        {   // ハルシェーダー ( HullShader )
            deviceContext->HSSetConstantBuffers(slot, 1, buffer_.GetAddressOf());
        }
    }
    void Deactivete() {}

    T* GetData() { return &data_; }

private:
    T data_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
};

