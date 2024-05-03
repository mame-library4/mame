#include "shader.h"

#include "Camera.h"

#include <fstream>

#include "../Other/misc.h"

#include "../Resource/Model.h"

#include "Graphics.h"

#include "../Core/Application.h"

// 頂点シェーダー ( VertexShader )
HRESULT CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
    FILE* fp{ nullptr };
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    if (input_layout)
    {
        hr = device->CreateInputLayout(input_element_desc, num_elements,
            cso_data.get(), cso_sz, input_layout);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    return hr;
}

// ピクセルシェーダー ( PixelShader )
HRESULT CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
    FILE* fp{ nullptr };
    fopen_s(&fp, cso_name, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]>cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    HRESULT hr{ S_OK };
    hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

// ジオメトリシェーダー ( GeometryShader )
HRESULT CreateGsFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader)
{
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreateGeometryShader(csoData.get(), csoSz, nullptr, geometryShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

// コンピュートシェーダー ( ComputeShader )
HRESULT CreateCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader)
{
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreateComputeShader(csoData.get(), csoSz, nullptr, computeShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

// ドメインシェーダー ( DomainShader )
HRESULT CreateDsFromCso(ID3D11Device* device, const char* csoName, ID3D11DomainShader** domainShader)
{
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreateDomainShader(csoData.get(), csoSz, nullptr, domainShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

// ハルシェーダー ( HullShader )
HRESULT CreateHsFromCso(ID3D11Device* device, const char* csoName, ID3D11HullShader** hullShader)
{
    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    HRESULT hr = device->CreateHullShader(csoData.get(), csoSz, nullptr, hullShader);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    return hr;
}

// コンストラクタ　
Shader::Shader(ID3D11Device* device)
{
    // 定数バッファ
    {
        // fog
        fogConstants_ = std::make_unique<ConstantBuffer<FogConstants>>(device);

        // PostEffect
        postEffectConstants_ = std::make_unique<ConstantBuffer<PostEffectConstants>>(device);

        // emissive
        emissiveConstants_ = std::make_unique<ConstantBuffer<EmissiceConstants>>(device);

        // dissolve
        dissolveConstants_ = std::make_unique<ConstantBuffer<DissolveConstants>>(device);
    }

    // ブレンドステート
#pragma region BlendState
    {
        HRESULT hr{ S_OK };

        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = FALSE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_STATE::NONE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_STATE::ALPHA)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_STATE::ADD)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&desc, blendStates[static_cast<size_t>(BLEND_STATE::MULTIPLY)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        for (int i = 0; i < 5; ++i)
        {
            desc.RenderTarget[i].BlendEnable = FALSE;
            desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
            desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
        hr = device->CreateBlendState(&desc, blendStates[static_cast<int>(BLEND_STATE::MRT)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    }
#pragma endregion// BlendState

    // 深度ステンシルステート
#pragma region DepthStencilState
    {
        HRESULT hr{ S_OK };

        D3D11_DEPTH_STENCIL_DESC desc{};
        // 0 深度テスト：オン　深度ライト：オン
        desc.DepthEnable = TRUE;	// 深度テストを有効にします
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// 深度データで変更できる深度ステンシルバッファーの一部を特定します
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // 深度データと既存データを比較する関数
        hr = device->CreateDepthStencilState(&desc, depthStencilStates[0].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 1 深度テスト：オン　深度ライト：オフ
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        hr = device->CreateDepthStencilState(&desc, depthStencilStates[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 2 深度テスト：オフ　深度ライト：オン
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        hr = device->CreateDepthStencilState(&desc, depthStencilStates[2].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 3 深度テスト：オフ　深度ライト：オフ
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        hr = device->CreateDepthStencilState(&desc, depthStencilStates[3].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
#pragma endregion// DepthStencilState

    // ラスタライザーステート
#pragma region RasterRizerState
    {
        HRESULT hr{ S_OK };

        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = TRUE;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = TRUE;
        desc.ScissorEnable = FALSE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_BACK;
        desc.AntialiasedLineEnable = TRUE;
        hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.AntialiasedLineEnable = TRUE;
        hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_NONE;
        desc.AntialiasedLineEnable = TRUE;
        hr = device->CreateRasterizerState(&desc, rasterizerStates[static_cast<size_t>(RASTER_STATE::WIREFRAME_CULL_NONE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
#pragma endregion// RasterRizerState

    // サンプラステート
#pragma region SamplerState
    {
        HRESULT hr{ S_OK };

        D3D11_SAMPLER_DESC desc{};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.BorderColor[0] = 0;
        desc.BorderColor[1] = 0;
        desc.BorderColor[2] = 0;
        desc.BorderColor[3] = 0;
        desc.MinLOD = 0;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.BorderColor[0] = 0;
        desc.BorderColor[1] = 0;
        desc.BorderColor[2] = 0;
        desc.BorderColor[3] = 0;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.BorderColor[0] = 1;
        desc.BorderColor[1] = 1;
        desc.BorderColor[2] = 1;
        desc.BorderColor[3] = 1;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // SHADOW
        desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; // D3D11_COMPARISON_LESS_EQUAL
        desc.BorderColor[0] = 1;
        desc.BorderColor[1] = 1;
        desc.BorderColor[2] = 1;
        desc.BorderColor[3] = 1;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::COMPARISON_LINEAR_BORDER_WHITE)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 0;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 1;
        desc.MinLOD = 0.0f;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_OPAQUE_BLACK)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 0;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 0;
        desc.BorderColor[0] = 0;
        desc.MinLOD = 0.0f;
        desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&desc, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT_CLAMP)].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
#pragma endregion// SamplerState

    // LightConstants
    {
        Graphics& graphics = Graphics::Instance();
#ifdef _DEBUG
        // POINT_LIGHT

#endif// _DEBUG

        // DIRECTION_LIGHT
        {
            lightConstant.directionLight.direction = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
            lightConstant.directionLight.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // POINT_LIGHT
        {
#if POINT_LIGHT_ONE
            lightConstant.pointLight.position = DirectX::XMFLOAT4(0.0f, 1.5f, 3.0f, 0.0f);
            lightConstant.pointLight.color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
            lightConstant.pointLight.range = 5.0f;
#else
            for (int i = 0; i < pointLightMax; ++i)
            {
                lightConstant.pointLight[i].position=DirectX::XMFLOAT4(3.0f*i, 1.5f, 3.0f, 0.0f);
                lightConstant.pointLight[i].color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
                lightConstant.pointLight[i].range = 5.0f;
            }
#endif// POINT_LIGHT_ONE
        }

        // SPOT_LIGHT
        {
            lightConstant.spotLight.position = DirectX::XMFLOAT4(0.0f, 1.0f, 15.0f, 0.0f);
            lightConstant.spotLight.color = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
            lightConstant.spotLight.direction = DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f);

            lightConstant.spotLight.range = 10.0f;

            // 正規化
            DirectX::XMVECTOR spotLightDirectionVec = DirectX::XMLoadFloat3(&lightConstant.spotLight.direction);
            spotLightDirectionVec = DirectX::XMVector3Normalize(spotLightDirectionVec);
            DirectX::XMStoreFloat3(&lightConstant.spotLight.direction, spotLightDirectionVec);

            lightConstant.spotLight.angle = DirectX::XMConvertToRadians(-5.0f);
        }

        // HEMISPHERE_LIGHT
        {
            // 地面色
            lightConstant.hemisphereLight.groundColor.x = 0.7f;
            lightConstant.hemisphereLight.groundColor.y = 0.5f;
            lightConstant.hemisphereLight.groundColor.z = 0.3f;

            // 天球色
            lightConstant.hemisphereLight.skyColor.x = 0.15f;
            lightConstant.hemisphereLight.skyColor.y = 0.7f;
            lightConstant.hemisphereLight.skyColor.z = 0.95f;

            // 地面の法線を設定
            lightConstant.hemisphereLight.groundNormal.x = 0.0f;
            lightConstant.hemisphereLight.groundNormal.y = 1.0f;
            lightConstant.hemisphereLight.groundNormal.z = 0.0f;
        }
    }
}


void Shader::DrawDebug()
{
    Camera& camera = Camera::Instance();

    ImGui::Begin("view");

    if (ImGui::TreeNode("camera"))
    {
        ImGui::SliderFloat("wight", &view.viewWitdh,1.0f,10000.0f);
        ImGui::SliderFloat("height", &view.viewHeight, 1.0f, 10000.0f);
        ImGui::SliderFloat("nearZ", &view.nearZ, 1.0f, 10000.0f);
        ImGui::SliderFloat("farZ", &view.farZ, 1.0f, 10000.0f);

        ImGui::DragFloat4("positon", &view.position.x);
        ImGui::DragFloat4("camera", &view.camera.x);

        ImGui::TreePop();
    }

    camera.DrawDebug();

    ImGui::End();

    // テスト用
    //postEffectConstants_->data.noiseTimer = framework::tictoc.time_stamp();
    //postEffectConstants_->data.scanLineTimer = framework::tictoc.time_stamp();

    if (ImGui::TreeNode("postEffect"))
    {
        ImGui::DragFloat4("shiftSize", &postEffectConstants_->data.shiftSize.x);
        ImGui::ColorEdit4("noiseColor", &postEffectConstants_->data.noiseColor.x);
        ImGui::DragFloat("noiseTimer", &postEffectConstants_->data.noiseTimer);
        ImGui::DragFloat("scanTimer", &postEffectConstants_->data.scanLineTimer);

        //ImGui::DragFloat("boekhAperture", &postEffectConstants.bokehAperture);
        //ImGui::DragFloat("bokehFocus", &postEffectConstants.bokehFocus);

        ImGui::DragFloat("bokeh_aperture", &postEffectConstants_->data.bokehAperture, 0.001f, 0.0f, 1.0f, "%.3f");
        ImGui::DragFloat("bokeh_focus", &postEffectConstants_->data.bokehFocus, 0.001f, 0.0f, 1.0f, "%.3f");

        ImGui::SliderFloat("vignetteValue", &postEffectConstants_->data.vignetteValue, 0.0f, 1.0f);

        ImGui::TreePop();
    }

    if (ImGui::Begin("Light"))
    {
        // DIRECTION_LIGHT
        if (ImGui::TreeNode("DirectionLight"))
        {
            ImGui::DragFloat4("directiono", &lightConstant.directionLight.direction.x);
            ImGui::ColorEdit4("color", &lightConstant.directionLight.color.x);
            ImGui::TreePop();
        }

        // POINT_LIGHT
#if POINT_LIGHT_ONE
        if (ImGui::TreeNode("PointLight"))
        {
            ImGui::DragFloat4("position", &lightConstant.pointLight.position.x);
            ImGui::ColorEdit4("color", &lightConstant.pointLight.color.x);
            ImGui::DragFloat("range", &lightConstant.pointLight.range);
            ImGui::TreePop();
        }
#else
        for (int i = 0; i < pointLightMax; ++i)
        {
            ImGui::Begin(("pointLight" + std::to_string(i)).c_str());
            ImGui::DragFloat4("position", &lightConstant.pointLight[i].position.x);
            ImGui::ColorEdit4("color", &lightConstant.pointLight[i].color.x);
            ImGui::DragFloat("range", &lightConstant.pointLight[i].range);
            ImGui::End();
        }

#endif// POINT_LIGHT_ONE

        // SPOT_LIGHT
        if (ImGui::TreeNode("SpotLight"))
        {
            ImGui::DragFloat4("position", &lightConstant.spotLight.position.x);
            ImGui::ColorEdit4("color", &lightConstant.spotLight.color.x);
            ImGui::DragFloat("range", &lightConstant.spotLight.range);
            ImGui::DragFloat3("direction", &lightConstant.spotLight.direction.x);
            float angle = DirectX::XMConvertToDegrees(lightConstant.spotLight.angle);
            ImGui::DragFloat("angle", &angle);
            lightConstant.spotLight.angle = DirectX::XMConvertToRadians(angle);
            ImGui::TreePop();
        }

        // HEMISPHERE_LIGHT
        if (ImGui::TreeNode("HemisphereLight"))
        {
            ImGui::ColorEdit4("groundColor", &lightConstant.hemisphereLight.groundColor.x);
            ImGui::ColorEdit4("skyColor", &lightConstant.hemisphereLight.skyColor.x);
            ImGui::DragFloat4("groundNormal", &lightConstant.hemisphereLight.groundNormal.x);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Fog"))
        {
            ImGui::ColorEdit4("fogColor", &fogConstants_->data.fogColor.x);
            ImGui::DragFloat("fogDensity", &fogConstants_->data.fogDensity);
            //ImGui::SliderFloat("fogDensity", &fogConstants.fogDensity, 0.0f, 10.0f, "%.4f");
            ImGui::DragFloat("fogHeightFalloff", &fogConstants_->data.fogHeightFalloff);
            //ImGui::SliderFloat("fogHeightFalloff", &fogConstants.fogHeightFalloff, 0.0001f, 10.0f, "%.4f");
            ImGui::SliderFloat("fogCutoffDistance", &fogConstants_->data.fogCutoffDistance, 0.0f, 1000.0f, "%.4f");
            ImGui::SliderFloat("startDistance", &fogConstants_->data.startDistance, 0.0f, 100.0f, "%.4f");

            ImGui::SliderFloat("timeScale", &fogConstants_->data.timeScale, 0.0f, 10.0f, "%.4f");
            ImGui::SliderFloat("speedScale", &fogConstants_->data.speedScale, 0.0f, 0.5f, "%.4f");
            ImGui::TreePop();
        }


        ImGui::End();
    }
}

void Shader::UpdatePostEffectConstants(int slot)
{   // PostEffect
    postEffectConstants_->Activate(Graphics::Instance().GetDeviceContext(), slot, false, true);
}

void Shader::UpdateFogConstants(int slot)
{   // Fog
    fogConstants_->Activate(Graphics::Instance().GetDeviceContext(), slot, false, true);
}

void Shader::UpdateEmissiveConstants(int slot)
{   // emissive
    emissiveConstants_->Activate(Graphics::Instance().GetDeviceContext(), slot, false, true);
}

void Shader::UpdateDissolveConstants(int slot)
{   // dissolve
    dissolveConstants_->Activate(Graphics::Instance().GetDeviceContext(), slot, false, true);
}


void Shader::SetDepthStencileState(DEPTH_STATE depthStencileState)
{
    Graphics::Instance().GetDeviceContext()->
        OMSetDepthStencilState(depthStencilStates[static_cast<UINT>(depthStencileState)].Get(), 1);
}

void Shader::SetBlendState(BLEND_STATE blendState)
{
    Graphics::Instance().GetDeviceContext()->
        OMSetBlendState(blendStates[static_cast<UINT>(blendState)].Get(), nullptr, 0xFFFFFFFF);
}

void Shader::SetRasterizerState(RASTER_STATE rasterizerState)
{
    Graphics::Instance().GetDeviceContext()->
        RSSetState(rasterizerStates[static_cast<UINT>(rasterizerState)].Get());
}

void Shader::SetSamplerState(ID3D11DeviceContext* deviceContext)
{  
    // サンプラーステート
    {
        deviceContext->PSSetSamplers(0, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
        deviceContext->PSSetSamplers(1, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
        deviceContext->PSSetSamplers(2, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
        deviceContext->PSSetSamplers(3, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
        deviceContext->PSSetSamplers(4, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
        // SHADOW
        deviceContext->PSSetSamplers(5, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::COMPARISON_LINEAR_BORDER_WHITE)].GetAddressOf());
        
        deviceContext->PSSetSamplers(6, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_OPAQUE_BLACK)].GetAddressOf());
        deviceContext->PSSetSamplers(7, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT_CLAMP)].GetAddressOf());
    }
}