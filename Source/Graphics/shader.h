#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <windows.h>
#include <memory>
#include <wrl.h>

class Shader
{
public:// ----- 定数 -----
    enum class BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY, MRT, MAX };
    enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE, MAX };
    enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF, MAX };
    enum class SAMPLER_STATE 
    {
        POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE/*SHADOW*/,
        LINEAR_BORDER_OPAQUE_BLACK, POINT_CLAMP, MAX,
    };
    enum class GBufferId { BaseColor, Emissive, Normal, Parameters, Depth, Max };

public:
    struct View
    {
        float viewWitdh = 100;
        float viewHeight = 100;
        float nearZ = 0;
        float farZ = 100;

        DirectX::XMFLOAT4 position{ 0, -4, -5, 0 };
        DirectX::XMFLOAT4 camera{ 0, 0, 1, 0 };
    }view;

public:
    Shader();
    ~Shader() {}

    void DrawDebug();

    // ---------- シェーダーオブジェクト生成関数 ----------
    HRESULT CreateVsFromCso(const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
    HRESULT CreatePsFromCso(const char* csoName, ID3D11PixelShader** pixelShader);
    HRESULT CreateGsFromCso(const char* csoName, ID3D11GeometryShader** geometryShader);
    HRESULT CreateCsFromCso(const char* csoName, ID3D11ComputeShader** computeShader);
    HRESULT CreateDsFromCso(const char* csoName, ID3D11DomainShader** domainShader);
    HRESULT CreateHsFromCso(const char* csoName, ID3D11HullShader** hullShader);

public:
    void SetDepthStencileState(const DEPTH_STATE& depthStencileState);
    void SetBlendState(const BLEND_STATE& blendState);
    void SetRasterizerState(const RASTER_STATE& rasterizerState);
    void SetSamplerState();

    void SetGBuffer();
    void SetGBufferShaderResourceView();
    ID3D11ShaderResourceView** GetGBufferBaseColorShaderResourceView() { return gBufferShaderResourceView_[0].GetAddressOf(); }
    ID3D11PixelShader* GetGBufferPixelShader() { return gBufferPixelShader_.Get(); }

    const DirectX::XMFLOAT4 GetViewPosition() { return view.position; }
    const DirectX::XMFLOAT4 GetViewCamera() { return view.camera; }

private:
    void CreateBlendStates();           // ブレンドステート作成
    void CreateRasterizerStates();      // ラスタライザステート作成
    void CreateDepthStencilStates();    // デプスステンシルステート作成
    void CreateSamplerStates();         // サンプラーステート作成
    void CreateGBuffer();               // G-Buffer作成

private:
    Microsoft::WRL::ComPtr<ID3D11BlendState>            blendStates_[static_cast<int>(BLEND_STATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>       rasterizerStates_[static_cast<int>(RASTER_STATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>     depthStencilStates_[static_cast<int>(DEPTH_STATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11SamplerState>          samplerState[static_cast<int>(SAMPLER_STATE::MAX)];

    // -------------------- G-Buffer --------------------
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      gBufferRenderTargetView_[static_cast<int>(GBufferId::Max)];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    gBufferShaderResourceView_[static_cast<int>(GBufferId::Max)];
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           gBufferPixelShader_;
};


