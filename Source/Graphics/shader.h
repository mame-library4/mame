#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <windows.h>
#include <memory>
#include <wrl.h>

#include "ConstantBuffer.h"

#include "../Graphics/Camera.h"

// SHADOW
#include "../Graphics/ShadowMap.h"

#define POINT_LIGHT_ONE 1

#if !POINT_LIGHT_ONE
const int pointLightMax = 8;
#endif

class Shader
{
public: // enum関連
    enum class SAMPLER_STATE {
        POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE/*SHADOW*/,
        LINEAR_BORDER_OPAQUE_BLACK, POINT_CLAMP, COUNT,
    };
    //enum class SAMPLER_STATE {
    //    POINT_WRAP, LINEAR_WRAP, ANISOTROPIC_WRAP, POINT_CLAMP, LINEAR_CLAMP, ANISOTROPIC_CLAMP,
    //    POINT_BORDER_OPAQUE_BLACK, LINEAR_BORDER_OPAQUE_BLACK, POINT_BORDER_OPAQUE_WHITE, LINEAR_BORDER_OPAQUE_WHITE,
    //    COMPARISON_DEPTH, COUNT
    //};
    
    enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
    enum class BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY, MRT };
    enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };


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

#pragma region Light[struct]
    // DIRECTION_LIGHT
    struct DirectionLight
    {
        DirectX::XMFLOAT4 direction;    // ライトの方向
        DirectX::XMFLOAT4 color;        // ライトの位置
    };

    // POINT_LIGHT
    struct PointLight
    {
        DirectX::XMFLOAT4 position = {};    // ポイントライトの位置
        DirectX::XMFLOAT4 color = {};       // ポイントライトの色
        float range = 0;                    // ポイントライトの影響範囲
        DirectX::XMFLOAT3 step = {};        // ダミー
    };

    // SPOT_LIGHT
    struct SpotLight
    {
        DirectX::XMFLOAT4 position = {};    // スポットライトの位置
        DirectX::XMFLOAT4 color = {};       // スポットライトの色
        float range = 0;                    // スポットライトの影響範囲
        DirectX::XMFLOAT3 step = {};        // ダミー
        DirectX::XMFLOAT3 direction = {};   // スポットライトの射出方向
        float angle = 0;                    // スポットライトの射出角度
    };
#pragma endregion// Light[struct]

    // HEMISPHERE_LIGHT
    struct HemisphereLight
    {
        DirectX::XMFLOAT4 groundColor;  // 地面色
        DirectX::XMFLOAT4 skyColor;     // 天球色
        DirectX::XMFLOAT4 groundNormal; // 地面の法線
    };

    struct LightConstants
    {
        DirectionLight directionLight;      // ディレクションライト
        
#if POINT_LIGHT_ONE
        PointLight pointLight;              // ポイントライト
#else
        PointLight pointLight[pointLightMax];
#endif// POINT_LIGHT_ONE

        SpotLight spotLight;                // スポットライト
        HemisphereLight hemisphereLight;    // 半球ライト
    }lightConstant;


    // FOG
    struct FogConstants
    {
        DirectX::XMFLOAT4 fogColor = { 1.000f, 1.000f, 1.000f, 0.894f }; // w: fog intensuty
        float fogDensity = 0.0005f;
        //float fogHeightFalloff = 0.9313f;
        float fogHeightFalloff = 10.0f;
        float startDistance = 5.00f;
        float fogCutoffDistance = 500.0f;
        float timeScale = 0.5f;
        float speedScale = 0.2f;
        float pads[2];
    };

    // postEffect
    struct PostEffectConstants
    {
        DirectX::XMFLOAT4 shiftSize = {};

        DirectX::XMFLOAT4 noiseColor{ 1.0f,1.0f,1.0f,1.0f };
        float noiseTimer = 0.0f;
        float scanLineTimer = 0.0f;

        float bokehAperture = 0.0f;
        float bokehFocus = 0.3f;

        float vignetteValue = 0.2f;
        DirectX::XMFLOAT3 dummy;
    };

    // Emissice
    struct EmissiceConstants
    {
        DirectX::XMFLOAT4 emissiceColor_ = { 1, 1, 1, 1 };
        float emissiveIntensity = 1.5f;
        DirectX::XMFLOAT3 dummy{};
    };

    struct DissolveConstants
    {
        DirectX::XMFLOAT4 parameters = { 1, 1, 1, 1 };
        DirectX::XMFLOAT4 edgeColor = { 1, 1, 1, 1 };
    };

private:// Constants
    std::unique_ptr<ConstantBuffer<FogConstants>> fogConstants_;                // fog
    std::unique_ptr<ConstantBuffer<PostEffectConstants>> postEffectConstants_;  // postEffect
    std::unique_ptr<ConstantBuffer<EmissiceConstants>> emissiveConstants_;      // emissive
    std::unique_ptr<ConstantBuffer<DissolveConstants>> dissolveConstants_;      // dissolve

public:
    Shader(ID3D11Device* device);
    ~Shader() {}

    void DrawDebug();

public:// 定数バッファー [Update. Get. Set]
#pragma region UpdateConstants
    void UpdatePostEffectConstants(int slot);
    void UpdateFogConstants(int slot);
    void UpdateEmissiveConstants(int slot);
    void UpdateDissolveConstants(int slot);
#pragma endregion// UpdateConstants

#pragma region [Get,Set]Function
    // ----- Emissive -----
    void SetEmissiveColor(DirectX::XMFLOAT4 color) { emissiveConstants_->data.emissiceColor_ = color; }
    void SetEmissiveIntensity(float intensity) { emissiveConstants_->data.emissiveIntensity = intensity; }

    // ----- Dissolve -----
    void SetDissolveIntensity(float intensity) { dissolveConstants_->data.parameters.x = intensity; }

#pragma endregion[Get,Set]Function

public:// 各種ステート設定
    void SetDepthStencileState(DEPTH_STATE depthStencileState);
    void SetBlendState(BLEND_STATE blendState);
    void SetRasterizerState(RASTER_STATE rasterizerState);

    void SetSamplerState(ID3D11DeviceContext* deviceContext);

public:
    const DirectX::XMFLOAT4 GetViewPosition() { return view.position; }
    const DirectX::XMFLOAT4 GetViewCamera() { return view.camera; }


private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;

    Microsoft::WRL::ComPtr<ID3D11BlendState>        blendStates[5];
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   rasterizerStates[4];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[4];

    Microsoft::WRL::ComPtr<ID3D11SamplerState>  samplerState[static_cast<UINT>(SAMPLER_STATE::COUNT)];
};


HRESULT CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
HRESULT CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

HRESULT CreateGsFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader);
HRESULT CreateCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader);

HRESULT CreateDsFromCso(ID3D11Device* device, const char* csoName, ID3D11DomainShader** domainShader);
HRESULT CreateHsFromCso(ID3D11Device* device, const char* csoName, ID3D11HullShader** hullShader);