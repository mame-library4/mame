#include "Application.h"
#include <sstream>

#include "../Scene/SceneManager.h"
#include "../Scene/TitleScene.h"
#include "../Scene/GameScene.h"

// ----- コンストラクタ -----
Application::Application(HWND hwnd)
    : hwnd_(hwnd),
    graphics_(hwnd, FALSE),
    input_(hwnd),
    postProcess_(graphics_.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT),
    deferredRendering_(graphics_.GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT),
    sceneConstants_(graphics_.GetDevice())
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

    // --- カメラ初期化 ---
    Camera::Instance().Initialize();

    // --- シーン初期化 ---
    SceneManager::Instance().ChangeScene(new GameScene);

//#ifndef _DEBUG
//    ShowCursor(!FULLSCREEN);	// フルスクリーン時はカーソルを消す
//#endif

    return true;
}

// ----- 終了化 -----
bool Application::Finalize()
{
    // --- シーン終了化 ---
    SceneManager::Instance().Clear();

    return false;
}

// ----- 更新 -----
void Application::Update(const float& elapsedTime)
{
    // --- ImGui更新 ---
    IMGUI_CTRL_CLEAR_FRAME();

    // --- 入力更新処理 ---
    input_.Update(elapsedTime);

    // --- カメラ更新 ---
    Camera::Instance().Update(elapsedTime);

    // --- シーン更新処理 ---
    SceneManager::Instance().Update(elapsedTime);
}

// ----- 描画 -----
void Application::Render()
{
    ID3D11DeviceContext* deviceContext = graphics_.GetDeviceContext();

    // --- 描画初期化 ---
    ID3D11RenderTargetView* renderTargetView = graphics_.GetRenderTargetView();
    ID3D11DepthStencilView* depthStencilView = graphics_.GetDepthStencilView();
    FLOAT color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // --- サンプラーステート設定 ---
    graphics_.GetShader()->SetSamplerState(deviceContext);

    // --- ShadowMap作成 ---
    //postProcess_.GetShadowBuffer().Activate(deviceContext);
    //SceneManager::Instance().ShadowRender(deviceContext);
    //postProcess_.GetShadowBuffer().Deactivate(deviceContext);

    // --- deferred rendering ---
    if (isDeferred_)
    {
        // ----- G-Buffer への描画 -----
        // 現在ディファード使えてません。。。

        Shader* shader = graphics_.GetShader();
        Camera& camera = Camera::Instance();
        camera.SetPerspectiveFov();
        DirectX::XMStoreFloat4x4(&sceneConstants_.data.viewProjection_, camera.GetViewMatrix() * camera.GetProjectionMatrix());
        sceneConstants_.data.lightDirection_ = shader->GetViewPosition();
        sceneConstants_.data.cameraPosition_ = shader->GetViewCamera();
        //DirectX::XMStoreFloat4x4(&sceneConstants_.data.inverseProjection, DirectX::XMMatrixInverse(NULL, camera.GetProjectionMatrix()));
        //DirectX::XMStoreFloat4x4(&sceneConstants_.data.inverseViewProjection, DirectX::XMMatrixInverse(NULL, camera.GetViewMatrix() * camera.GetProjectionMatrix()));
        shader->SetBlendState(Shader::BLEND_STATE::NONE);
        shader->SetRasterizerState(Shader::RASTER_STATE::SOLID);
        shader->SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);

        sceneConstants_.Activate(graphics_.GetDeviceContext(), 1, true, true, true, true);

        // ポストプロセス開始
        postProcess_.Activate(deviceContext);
        SceneManager::Instance().ForwardRender();
        postProcess_.Deactivate(deviceContext);
        postProcess_.Draw(deviceContext);
    }
    // --- forward rendering ---
    else
    {

    }

    DrawDebug();

    // --- ImGui表示 ---
    IMGUI_CTRL_DISPLAY();

    // --- 実行 ---
    UINT syncInterval = 0;
    graphics_.GetSwapChain()->Present(syncInterval, 0);
}

void Application::DrawDebug()
{
    postProcess_.DrawDebug();

    Camera::Instance().DrawDebug();

    ImGui::Checkbox("isDeferred_", &isDeferred_);

    deferredRendering_.DrawDebug();
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
