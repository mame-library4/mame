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
    void Draw(ID3D11ShaderResourceView* cascadeShadowMap);

    void DrawDebug();

    ConstantBuffer<Constants>* GetConstants() { return constant_.get(); }

    void SetUseRadialBlur(const bool& flag = true) { useRadialBlur_ = flag; }

private:
    std::unique_ptr<FullscreenQuad>             renderer_;
    std::unique_ptr<FrameBuffer>                postProcess_;
    
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   postProcessPS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   roarPS_;

    std::unique_ptr<ConstantBuffer<Constants>> constant_;

    bool useRadialBlur_ = false;

    Bloom   bloom_;
};