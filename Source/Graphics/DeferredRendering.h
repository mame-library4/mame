#pragma once
#include <d3d11.h>
#include "FrameBuffer.h"

class DeferredRendering
{
public:
    DeferredRendering(ID3D11Device* device, uint32_t width, uint32_t height);
    ~DeferredRendering() {}

    void Activate(ID3D11DeviceContext* deviceContext);

    void DrawDebug();

private:
    FrameBuffer baseColorBuffer_;
    FrameBuffer normalBuffer_;
    FrameBuffer worldBuffer_;
    FrameBuffer shadowBuffer_;
    FrameBuffer metalnessRoughnessBuffer_;
};

