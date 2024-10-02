#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "HighResolutionTimer.h"
#include "ImGui/ImGuiCtrl.h"

#include "Graphics.h"

#include "../Graphics/shader.h"
#include "../Input/Input.h"

#include "ShadowMap.h"
#include "CascadedShadowMap.h"

#include "../Graphics/DeferredRendering.h"
#include "SkyMap.h"

#include "ConstantBuffer.h"

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
//CONST LONG SCREEN_WIDTH{ 1920 };
//CONST LONG SCREEN_HEIGHT{ 1080 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"Mame" };

class Application
{
public:
	Application(HWND hwnd);
	~Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) noexcept = delete;
	Application& operator=(Application&&) noexcept = delete;

	int Run();
	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void CalculateFrameStats();

private:
	bool Initialize();
	bool Finalize();
	void Update(const float& elapsedTime);
	void Render();
	void DrawDebug();

private:
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4 viewProjection_;
		DirectX::XMFLOAT4	lightDirection_;
		DirectX::XMFLOAT4	cameraPosition_;
		DirectX::XMFLOAT4X4	inverseProjection_;
		DirectX::XMFLOAT4X4 inverseViewProjection_;
	};

private:
	CONST HWND hwnd_;
	Graphics graphics_;
	Input input_;

	DeferredRendering	deferredRendering_;

	SkyMap skymap_;

	HighResolutionTimer tictoc_;
	uint32_t			frames_			= 0;
	float				elapsedTime_	= 0.0f;

	bool isDeferred_ = true; // deferredを使うか

	// --- シーン定数バッファー ---
	ConstantBuffer<SceneConstants> sceneConstants_;
};