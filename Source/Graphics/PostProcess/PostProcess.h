#pragma once
#include <d3d11.h>
#include <memory>
#include "../FrameBuffer.h"
#include "../FullscreenQuad.h"
#include "Bloom.h"

class PostProcess
{
public:
    PostProcess(ID3D11Device* device, uint32_t width, uint32_t height);
    ~PostProcess();

    void Activate(ID3D11DeviceContext* deviceContext);
    void Deactivate(ID3D11DeviceContext* deviceContext);
    void Draw(ID3D11DeviceContext* deviceContext);

    void DrawDebug();

private:
    std::unique_ptr<FullscreenQuad>             renderer_;
    std::unique_ptr<FrameBuffer>                postProcess_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   postProcessPS_;

    Bloom   bloom_;
};