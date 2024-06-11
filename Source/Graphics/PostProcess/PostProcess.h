#pragma once
#include <d3d11.h>
#include <memory>
#include "../FrameBuffer.h"
#include "../FullscreenQuad.h"
#include "Bloom.h"

class PostProcess
{
private:
    struct Constants
    {
        float blurPower_ = 0.0f;
        DirectX::XMFLOAT3 dummy_;
    };

    PostProcess();
    ~PostProcess();

public:
    static PostProcess& Instance()
    {
        static PostProcess instance;
        return instance;
    }

    void Activate();
    void Deactivate();
    void Draw();

    void DrawDebug();

    ConstantBuffer<Constants>* GetConstants() { return constant_.get(); }

private:
    std::unique_ptr<FullscreenQuad>             renderer_;
    std::unique_ptr<FrameBuffer>                postProcess_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   postProcessPS_;

    std::unique_ptr<ConstantBuffer<Constants>> constant_;

    Bloom   bloom_;
};