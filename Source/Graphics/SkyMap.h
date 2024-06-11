#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "FullscreenQuad.h"

class SkyMap
{
public:
    SkyMap();
    ~SkyMap() {}

    void Render();

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           psShader_;
    std::unique_ptr<FullscreenQuad>                     renderer_;
};