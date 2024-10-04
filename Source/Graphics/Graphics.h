#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include "ImGui/imgui.h"
#include "Shader.h"
#include "DebugRenderer.h"

class Graphics
{
public:
    Graphics(HWND hWnd, BOOL fullscreen);
    ~Graphics();

    // インスタンス取得
    static Graphics& Instance() { return *instance_; }

    ID3D11Device*           GetDevice()             const { return device_.Get(); }
    IDXGISwapChain*         GetSwapChain()          const { return swapchain_.Get(); }
    ID3D11DeviceContext*    GetDeviceContext()      const { return deviceContext_.Get(); }
    ID3D11RenderTargetView* GetRenderTargetView()   const { return renderTargetView_.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView()   const { return depthStencilView_.Get(); }
    
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthMap() { return depthShaderResourceView_; }



    // ミューテックス取得
    std::mutex& GetMutex() { return mutex; }

    // デバッグレンダラ取得
    DebugRenderer* GetDebugRenderer() const { return debugRenderer_.get(); }

public:// ----- シェーダー関連 -----
    // シェーダー取得
    Shader* GetShader() const { return shader_.get(); }

    HRESULT CreateVsFromCso(const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements) { return shader_->CreateVsFromCso(csoName, vertexShader, inputLayout, inputElementDesc, numElements); }
    HRESULT CreatePsFromCso(const char* csoName, ID3D11PixelShader** pixelShader) { return shader_->CreatePsFromCso(csoName, pixelShader); }
    HRESULT CreateGsFromCso(const char* csoName, ID3D11GeometryShader** geometryShader) { return shader_->CreateGsFromCso(csoName, geometryShader); }
    HRESULT CreateCsFromCso(const char* csoName, ID3D11ComputeShader** computeShader) { return shader_->CreateCsFromCso(csoName, computeShader); }
    HRESULT CreateDsFromCso(const char* csoName, ID3D11DomainShader** domainShader) { return shader_->CreateDsFromCso(csoName, domainShader); }
    HRESULT CreateHsFromCso(const char* csoName, ID3D11HullShader** hullShader) { return shader_->CreateHsFromCso(csoName, hullShader); }
    void SetDepthStencileState(const Shader::DEPTH_STATE& depthStencileState) { shader_->SetDepthStencileState(depthStencileState); }
    void SetBlendState(const Shader::BLEND_STATE& blendState) { shader_->SetBlendState(blendState); }
    void SetRasterizerState(const Shader::RASTER_STATE& rasterizerState) { shader_->SetRasterizerState(rasterizerState); }

private:
    static Graphics* instance_;

    Microsoft::WRL::ComPtr<ID3D11Device>            device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     deviceContext_;
    Microsoft::WRL::ComPtr<IDXGISwapChain>          swapchain_;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  renderTargetView_;
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D>             depthStencilBuffer_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      depthStencilView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    depthShaderResourceView_;

    std::unique_ptr<Shader> shader_;

    std::mutex mutex;

    std::unique_ptr<DebugRenderer> debugRenderer_;
};

