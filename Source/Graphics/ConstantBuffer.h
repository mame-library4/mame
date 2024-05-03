#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>
#include "../Other/misc.h"

// ----- ConstantBuffer �̎g���� -----
// --- GPU���ɓn�������ϐ����`����B��16�o�C�g�̔{���Ɍ��� (.h) ---
// struct constants
// {
//     DirectX::XMFLOAT4 parameters;
// };
// std::unique_ptr<ConstantBuffer<constants>> constant_;
//
// --- �������� ( CreateResource or Initialize ) ---
// constant_ = std::make_unique<ConstantBuffer<constants>>(device.Get());
// 
// --- �X�V ( Update and Render ) ---
// constant_->data.parameters = { 1, 0, 0, 0 };
// constant_->Activate(deviceContext.Get(), 0);
// 

#define USAGE_DYNAMIC

template <class T>
class ConstantBuffer
{
public:
    T data;

    ConstantBuffer(ID3D11Device* device)
    {
        HRESULT hr{ S_OK };
        
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = (sizeof(T) + 0x0f) & ~0x0f;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&bufferDesc, 0, bufferObject.GetAddressOf());
        assert(SUCCEEDED(hr) && hr_trace(hr));
#else
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subresourceData{};
        subresourceData.pSysMem = &data;
        subresourceData.SysMemPitch = 0;
        subresourceData.SysMemSlicePitch = 0;
        hr = device->CreateBuffer(&bufferDesc, &subresourceData, bufferObject.GetAddressOf());
        assert(SUCCEEDED(hr) && hr_trace(hr));
#endif
    }
    virtual ~ConstantBuffer() = default;
    ConstantBuffer(ConstantBuffer&) = delete;
    ConstantBuffer& operator =(ConstantBuffer&) = delete;

    void Activate(ID3D11DeviceContext* deviceContext, int slot,
        bool VSset = true, bool PSset = true, bool CSset = false,
        bool GSset = false, bool HSset = false)
    {
        HRESULT hr{ S_OK };

#ifdef USAGE_DYNAMIC
        D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
        D3D11_MAPPED_SUBRESOURCE mappedBuffer;

        hr = deviceContext->Map(bufferObject.Get(), 0, map, 0, &mappedBuffer);
        assert(SUCCEEDED(hr) && hr_trace(hr));
        memcpy_s(mappedBuffer.pData, sizeof(T), &data, sizeof(T));
        deviceContext->Unmap(bufferObject.Get(), 0);
#else
        deviceContext->UpdateSubresource(bufferObject.Get(), 0, 0, &data, 0, 0);
#endif

        if (VSset)
        {   // ���_�V�F�[�_�[ ( VertexShader )
            deviceContext->VSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
        }
        if (PSset)
        {   // �s�N�Z���V�F�[�_�[ ( PixelShader )
            deviceContext->PSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
        }
        if (CSset)
        {   // �R���s���[�g�V�F�[�_�[ ( ComputeShader )
            deviceContext->CSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
        }
        if (GSset)
        {   // �W�I���g���V�F�[�_�[ ( GeometryShader )
            deviceContext->GSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
        }
        if (HSset)
        {   // �n���V�F�[�_�[ ( HullShader )
            deviceContext->HSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
        }
    }

    void Deavtivate(ID3D11DeviceContext* deviceContext)
    {
        // NOP
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> bufferObject;
};

