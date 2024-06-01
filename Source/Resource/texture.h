#pragma once
#include <WICTextureLoader.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <map>

class Texture
{
private:
    Texture() {}
    ~Texture() {}

public:
    static Texture& Instance()
    {
        static Texture instance;
        return instance;
    }

    HRESULT LoadTexture(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2Ddesc);
    HRESULT LoadTexture(const void* data, size_t size, ID3D11ShaderResourceView** shaderResourceView);
    HRESULT MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension);
    void ReleaseAllTextures();

private:
    std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources_;
};