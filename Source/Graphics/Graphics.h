#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>
#include "shader.h"
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

    // シェーダー取得
    Shader* GetShader()const { return shader.get(); }

    // ミューテックス取得
    std::mutex& GetMutex() { return mutex; }

    // デバッグレンダラ取得
    DebugRenderer* GetDebugRenderer() const { return debugRenderer_.get(); }

private:
    static Graphics* instance_;

    Microsoft::WRL::ComPtr<ID3D11Device>            device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     deviceContext_;
    Microsoft::WRL::ComPtr<IDXGISwapChain>          swapchain_;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>         depthStencilBuffer_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  depthStencilView_;

    std::unique_ptr<Shader> shader;

    std::mutex mutex;

    std::unique_ptr<DebugRenderer> debugRenderer_;
};

