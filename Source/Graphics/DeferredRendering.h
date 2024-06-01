#pragma once
#include <d3d11.h>
#include "FrameBuffer.h"

class DeferredRendering
{
public:
    DeferredRendering(const uint32_t& width, const uint32_t& height);
    ~DeferredRendering() {}

    void Activate();

    void DrawDebug();

private:
    FrameBuffer baseColorBuffer_;
    FrameBuffer normalBuffer_;
    FrameBuffer worldBuffer_;
    FrameBuffer shadowBuffer_;
    FrameBuffer metalnessRoughnessBuffer_;
};

