#include "Texture.h"
#include <filesystem>
#include <DDSTextureLoader.h>
#include "Graphics.h"
#include "Misc.h"

// ----- �e�N�X�`���ǂݍ��� -----
HRESULT Texture::LoadTexture(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2Ddesc)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    std::filesystem::path ddsFilename(filename);

    ddsFilename.replace_extension("dds");
    if (std::filesystem::exists(ddsFilename.c_str()))
    {
        result = DirectX::CreateDDSTextureFromFile(device, ddsFilename.c_str(), resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }
    else
    {
        result = DirectX::CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }
    resources_.insert(std::make_pair(filename, *shaderResourceView));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    result = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    texture2d->GetDesc(texture2Ddesc);
    
    return result;
}

// ----- �e�N�X�`���ǂݍ��� -----
HRESULT Texture::LoadTexture(const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    // DDS�ǂݍ���
    result = DirectX::CreateDDSTextureFromMemory(device,
        reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shaderResourceView);
    
    if (result != S_OK)
    {
        result = DirectX::CreateWICTextureFromMemory(device,
            reinterpret_cast<const uint8_t*>(data), size, resource.GetAddressOf(), shaderResourceView);
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    return result;
}

// ----- �_�~�[�e�N�X�`���쐬 -----
HRESULT Texture::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = dimension;
    texture2dDesc.Height = dimension;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    size_t texels = dimension * dimension;
    std::unique_ptr<DWORD[]>sysmem{ std::make_unique<DWORD[]>(texels) };
    for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

    D3D11_SUBRESOURCE_DATA subresourceData{};
    subresourceData.pSysMem = sysmem.get();
    subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    result = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = texture2dDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    result = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc,
        shaderResourceView);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- �S�e�N�X�`����� -----
void Texture::ReleaseAllTextures()
{
    resources_.clear();
}
