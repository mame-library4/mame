#pragma once
#include <d3d11.h>
#include <memory>
#include "../FrameBuffer.h"
#include "../FullscreenQuad.h"
#include "Bloom.h"

class PostProcess
{
public:
    PostProcess(const uint32_t& width, const uint32_t& height);
    ~PostProcess();

    void Activate();
    void Deactivate();
    void Draw();

    void DrawDebug();

private:
    std::unique_ptr<FullscreenQuad>             renderer_;
    std::unique_ptr<FrameBuffer>                postProcess_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   postProcessPS_;

    Bloom   bloom_;
};