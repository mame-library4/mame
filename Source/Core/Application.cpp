#include "Application.h"
#include <sstream>
#include "PostProcess/PostProcess.h"

#include "../Scene/SceneManager.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"
#include "../Scene/DemoScene.h"

#include "Camera.h"

#include "AudioManager.h"

#include "Effect/EffectManager.h"
#include "UI/UIManager.h"

// ----- コンストラクタ -----
Application::Application(HWND hwnd)
    : hwnd_(hwnd),
    graphics_(hwnd, FALSE),
    input_(hwnd),
    deferredRendering_(),
    sceneConstants_()
{
}

// ----- デストラクタ -----
Application::~Application()
{
}

// ----- 初期化 -----
bool Application::Initialize()
{
    // --- 入力の初期設定 ---
    input_.GetMouse().SetScreenWidth(SCREEN_WIDTH);
    input_.GetMouse().SetScreenHeight(SCREEN_HEIGHT);

    // --- オーディオ読み込み ---
    AudioManager::Instance().LoadAudio();
    AudioManager::Instance().StopAllAudio();

    // --- カメラ初期化 ---
    Camera::Instance().Initialize();

    // --- シーン初期化 ---
    //SceneManager::Instance().ChangeScene(new TitleScene);
    SceneManager::Instance().ChangeScene(new GameScene);
    //SceneManager::Instance().ChangeScene(new DemoScene);

    // エフェクト初期化
    EffectManager::Instance().Initialize();

//#ifndef _DEBUG
//    ShowCursor(!FULLSCREEN);	// フルスクリーン時はカーソルを消す
//#endif

    sceneConstants_.GetData()->lightDirection_ = { 0, -1, -1, 0 };

    return true;
}

// ----- 終了化 -----
bool Application::Finalize()
{
    // --- シーン終了化 ---
    SceneManager::Instance().Clear();

    // UserInterface
    UIManager::Instance().Clear();

    EffectManager::Instance().Finalize();

    return false;
}

// ----- 更新 -----
void Application::Update(const float& elapsedTime)
{
    // ImGui更新
    IMGUI_CTRL_CLEAR_FRAME();

    // 入力更新処理
    input_.Update(elapsedTime);

    // カメラ更新
    Camera::Instance().Update(elapsedTime);

    // シーン更新処理
    SceneManager::Instance().Update(elapsedTime);

    // エフェクト更新
    EffectManager::Instance().Update(elapsedTime);

    UIManager::Instance().Update(elapsedTime);
}

// ----- 描画 -----
void Application::Render()
{
    ID3D11DeviceContext* deviceContext = graphics_.GetDeviceContext();

    Camera& camera = Camera::Instance();

    // --- 描画初期化 ---
    ID3D11RenderTargetView* renderTargetView = graphics_.GetRenderTargetView();
    ID3D11DepthStencilView* depthStencilView = graphics_.GetDepthStencilView();
    FLOAT color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // --- サンプラーステート設定 ---
    graphics_.GetShader()->SetSamplerState();

    camera.SetPerspectiveFov();
    DirectX::XMStoreFloat4x4(&sceneConstants_.GetData()->viewProjection_, camera.GetViewMatrix() * camera.GetProjectionMatrix());

    //sceneConstants_.GetData()->lightDirection_ = Graphics::Instance().GetShader()->GetViewPosition();
    sceneConstants_.GetData()->cameraPosition_ = { camera.GetEye().x, camera.GetEye().y, camera.GetEye().z, 0 };

    DirectX::XMStoreFloat4x4(&sceneConstants_.GetData()->inverseProjection_, DirectX::XMMatrixInverse(NULL, camera.GetProjectionMatrix()));
    DirectX::XMStoreFloat4x4(&sceneConstants_.GetData()->inverseViewProjection_, DirectX::XMMatrixInverse(NULL, camera.GetViewMatrix() * camera.GetProjectionMatrix()));
    sceneConstants_.Activate(1, true, true, true, true);

    // Make cascade shadow map
    PostProcess::Instance().MakeCascadedShadowMap(sceneConstants_.GetData()->lightDirection_, 3, [&]() { SceneManager::Instance().ShadowRender(); });
    
    // --- deferred rendering ---
    if (isDeferred_)
    {
        Graphics::Instance().GetShader()->SetGBuffer();
        Graphics::Instance().SetBlendState(Shader::BLEND_STATE::MRT);
        Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);
        Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);
        SceneManager::Instance().DeferredRender();

        deviceContext->ClearRenderTargetView(renderTargetView, color);
        //deviceContext->ClearDepthStencilView(Graphics::Instance().GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
     

        PostProcess::Instance().Activate();

        // SkyMap
        skymap_.Render();

        deferredRendering_.Draw();
               
        SceneManager::Instance().Render();

        // デバッグレンダラ
        DirectX::XMFLOAT4X4 view, projection;
        DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
        DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());
#if _DEBUG
        Graphics::Instance().GetDebugRenderer()->Render(deviceContext, view, projection);
#endif
        EffectManager::Instance().Render();

        PostProcess::Instance().Deactivate();

        PostProcess::Instance().Draw();
    }
    // --- forward rendering ---
    else
    {        
        // ポストプロセス開始
        PostProcess::Instance().Activate();

        // SkyMap
        skymap_.Render();

        // シーン
        Graphics::Instance().SetBlendState(Shader::BLEND_STATE::NONE);
        Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::SOLID);
        Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);
        SceneManager::Instance().ForwardRender();

        SceneManager::Instance().Render();

        // デバッグレンダラ
#if _DEBUG
        DirectX::XMFLOAT4X4 view, projection;
        DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
        DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());
        Graphics::Instance().GetDebugRenderer()->Render(deviceContext, view, projection);
#endif

        Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
        Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
        Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);
        
        PostProcess::Instance().Deactivate();

        PostProcess::Instance().Draw();
    }


    // --------------- UI 描画 ---------------
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);
    
    UIManager::Instance().Render();
    

    DrawDebug();

    // --- ImGui表示 ---
    IMGUI_CTRL_DISPLAY();

    // --- 実行 ---
    UINT syncInterval = 0;
    graphics_.GetSwapChain()->Present(syncInterval, 0);
}

void Application::DrawDebug()
{
#ifdef USE_IMGUI
    PostProcess::Instance().DrawDebug();

    Camera::Instance().DrawDebug();

    ImGui::Checkbox("isDeferred_", &isDeferred_);

    ImGui::DragFloat3("LightDirection", &sceneConstants_.GetData()->lightDirection_.x);

    graphics_.GetShader()->DrawDebug();

    UIManager::Instance().DrawDebug();

    EffectManager::Instance().DrawDebug();
    
#endif
}

// ----- 実行 -----
int Application::Run()
{
    MSG msg{};

    if (!Initialize())
    {
        return 0;
    }

    // ImGui初期化(DirectX11の初期化の下に置くこと)
    IMGUI_CTRL_INITIALIZE(hwnd_, graphics_.GetDevice(), graphics_.GetDeviceContext());

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            tictoc_.Tick();
            CalculateFrameStats();
            Update(tictoc_.TimeInterval());
            Render();
        }
    }

    // ImGui終了化
    IMGUI_CTRL_UNINITIALIZE();

#if 1
    BOOL fullscreen = 0;
    graphics_.GetSwapChain()->GetFullscreenState(&fullscreen, 0);
    if (fullscreen)
    {
        graphics_.GetSwapChain()->SetFullscreenState(FALSE, 0);
    }
#endif

    return Finalize() ? static_cast<int>(msg.wParam) : 0;
}

void Application::CalculateFrameStats()
{
    if (++frames_, (tictoc_.TimeStamp() - elapsedTime_) >= 1.0f)
    {
        float fps = static_cast<float>(frames_);
        std::wostringstream outs;
        outs.precision(6);
#if USE_IMGUI
        // FPS
        outs << L"FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
#else
        // ゲームタイトル
        outs << APPLICATION_NAME;
#endif
        SetWindowTextW(hwnd_, outs.str().c_str());

        frames_ = 0;
        elapsedTime_ += 1.0f;
    }
}

// ----- メッセージハンドラー -----
LRESULT Application::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // ImGui(先頭に置く)
    IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wparam, lparam);

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        BeginPaint(hwnd, &ps);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        break;
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
        {      
            PostQuitMessage(0);
        }
        break;
    case WM_ENTERSIZEMOVE:
        tictoc_.Stop();
        break;
    case WM_EXITSIZEMOVE:
        tictoc_.Start();
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}
