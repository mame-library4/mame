#pragma once
#include <d3d11.h>
#include <memory>
#include "../FrameBuffer.h"
#include "../FullscreenQuad.h"
#include "Bloom.h"
#include "CascadedShadowMap.h"

class PostProcess
{
private:
    struct Constants
    {
        float shadowColor_ = 0.55f;
        float shadowDepthBias_ = 0.0001f;
        bool colorizeCascadedLayer_ = false;
        
        float dummy_;
    };
    struct RadialBlurConstants
    {
        DirectX::XMFLOAT2 uvOffset_ = {};
        float strength_ = 0.4f;
        float sampleCount_ = 1.0f;
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

    void MakeCascadedShadowMap(const DirectX::XMFLOAT4& lightDirection, UINT cbSlot, std::function<void()> drawcallback);

    ConstantBuffer<Constants>* GetConstants() { return constant_.get(); }
    ConstantBuffer<RadialBlurConstants>* GetRadialBlurConstants() { return radialBlurConstants_.get(); }

    void SetUseRadialBlur(const bool& flag = true) { useRadialBlur_ = flag; }

private:
    std::unique_ptr<FullscreenQuad>             renderer_;
    std::unique_ptr<FrameBuffer>                postProcess_;
    std::unique_ptr<FrameBuffer>                radialBlur_;
    
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   postProcessPS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   radialBlurPS_;

    std::unique_ptr<ConstantBuffer<Constants>>              constant_;
    std::unique_ptr<ConstantBuffer<RadialBlurConstants>>    radialBlurConstants_;

    bool useRadialBlur_ = false;

    float criticalDepthValue_ = 0.0f;

    Bloom   bloom_;
    CascadedShadowMap cascadedShadowMap_;
};