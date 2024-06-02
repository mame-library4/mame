#pragma once
#include <d3d11.h>
#include <memory>
#include "FrameBuffer.h"
#include "FullscreenQuad.h"

class DeferredRendering
{
public:
    DeferredRendering();
    ~DeferredRendering() {}

    void Draw();

private:
    std::unique_ptr<FullscreenQuad> renderer_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> psShader_;
};

