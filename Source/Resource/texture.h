#pragma once
#include <WICTextureLoader.h>

// テクスチャ読み込み
HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* fileName,
    ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);

void ReleaseAllTextures();

// ダミーテクスチャ作成関数
HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view,
    DWORD value/*0xAABBGGRR*/, UINT dimension);


HRESULT LoadTextureFromMemory(ID3D11Device* device, const void* data, size_t size,
    ID3D11ShaderResourceView** shader_resource_view);