#include "Graphics.h"
#include "../Other/misc.h"
#include "../Core/Application.h"
#include <dxgi.h>

#pragma comment(lib,"dxgi.lib")

void acquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumeratedAdapter;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf())); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		hr = enumeratedAdapter->GetDesc1(&adapterDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		if (adapterDesc.VendorId == 0x1002/*AMD*/ || adapterDesc.VendorId == 0x10DE/*NVIDIA*/)
		{
			OutputDebugStringW((std::wstring(adapterDesc.Description) + L" has been selected.\n").c_str());
			OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapterDesc.VendorId) + '\n').c_str());
			OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapterDesc.DeviceId) + '\n').c_str());
			OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapterDesc.SubSysId) + '\n').c_str());
			OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapterDesc.Revision) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapterDesc.DedicatedVideoMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapterDesc.DedicatedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapterDesc.SharedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapterDesc.AdapterLuid.HighPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapterDesc.AdapterLuid.LowPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapterDesc.Flags) + '\n').c_str());
			break;
		}
	}
	*dxgiAdapter3 = enumeratedAdapter.Detach();
}

Graphics* Graphics::instance_ = nullptr;

#define ADAPTER 0

// ----- コンストラクタ -----
Graphics::Graphics(HWND hWnd, BOOL fullscreen)
{
	// インスタンス設定
	_ASSERT_EXPR(instance_ == nullptr, "already instantiated");
	instance_ = this;

	HRESULT hr = S_OK;

	// アダプタ
#if ADAPTER
	IDXGIFactory* factory;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));
	IDXGIAdapter* adapter;
	for (UINT adapterIndex = 0; S_OK == factory->EnumAdapters(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);
		if (adapterDesc.VendorId == 0x1002/*AMD*/ || adapterDesc.VendorId == 0x10DE/*NVIDIA*/)
		{
			break;
		}
		adapter->Release();
	}
	factory->Release();
#endif

	{
		// スワップチェーンの作成
		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
		swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = !FULLSCREEN;

#if ADAPTER
        hr = D3D11CreateDeviceAndSwapChain(adapter/*ADAPTER*/, D3D_DRIVER_TYPE_UNKNOWN/*ADAPTER*/, NULL, createDeviceFlags,
        	&featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc,
        	&swapchain_, &device_, NULL, &deviceContext_);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        if (adapter != nullptr) adapter->Release();
#endif
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
			&featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc,
			&swapchain_, &device_, NULL, &deviceContext_);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// レンダーターゲットビューの作成
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
		hr = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		hr = device_->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView_);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));


		// 深度ステンシルビューの作成
#if 1
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		D3D11_TEXTURE2D_DESC texture2dDesc{};

#if 1
		texture2dDesc.Width = SCREEN_WIDTH;
		texture2dDesc.Height = SCREEN_HEIGHT;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
#else
		texture2dDesc.Width = SCREEN_WIDTH;
		texture2dDesc.Height = SCREEN_HEIGHT;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
#endif

		hr = device_->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		//depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device_->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device_->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, depthShaderResourceView_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

#endif

		// ビューポートの設定
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(SCREEN_WIDTH);
		viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		deviceContext_->RSSetViewports(1, &viewport);
	}

	// シェーダー
	{
		shader_ = std::make_unique<Shader>();
	}

	// デバッグレンダラ生成
	{
		debugRenderer_ = std::make_unique<DebugRenderer>();
#ifdef _DEBUG
#endif
	}
}

Graphics::~Graphics()
{
}