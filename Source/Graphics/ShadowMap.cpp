#include "ShadowMap.h"
#include "Graphics.h"
#include "Misc.h"

ShadowMap::ShadowMap(const uint32_t& width, const uint32_t& height)
{
	HRESULT result = S_OK;
	ID3D11Device* device = Graphics::Instance().GetDevice();

	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width					= width;
	texture2dDesc.Height				= height;
	texture2dDesc.MipLevels				= 1;
	texture2dDesc.ArraySize				= 1;
	texture2dDesc.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count		= 1;
	texture2dDesc.SampleDesc.Quality	= 0;
	texture2dDesc.Usage					= D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags		= 0;
	texture2dDesc.MiscFlags				= 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	result = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags			= 0;
	result = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format				= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels	= 1;
	result = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void ShadowMap::Clear(const float& depth)
{
	Graphics::Instance().GetDeviceContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void ShadowMap::Activate()
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

	deviceContext->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* null_render_target_view{ NULL };
	deviceContext->OMSetRenderTargets(1, &null_render_target_view, depthStencilView.Get());
}

void ShadowMap::Deactivete()
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->RSSetViewports(viewportCount, cachedViewports);
	deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

void ShadowMap::DrawDebug()
{
	if (ImGui::TreeNode("ShadowMap"))
	{
		ImGui::Image(reinterpret_cast<ImTextureID>(shaderResourceView.Get()), ImVec2(256.0, 256.0));
		ImGui::DragFloat4("LightViewFocus", &shadowData_.lightViewFocus_.x);
		ImGui::DragFloat("LightViewDistance", &shadowData_.lightViewDistance_);
		ImGui::DragFloat("LightViewSize", &shadowData_.lightViewSize_);
		ImGui::DragFloat("LightViewNearZ", &shadowData_.lightViewNearZ_);
		ImGui::DragFloat("LightViewFarZ", &shadowData_.lightViewFarZ_);
		
		ImGui::TreePop();
	}
}

// ----- ‰e—p‚ÌviewProjections—ñŽZo -----
DirectX::XMFLOAT4X4 ShadowMap::CalcViewProjection(const float& aspectRatio)
{
	DirectX::XMVECTOR F =
	{
		DirectX::XMLoadFloat4(&shadowData_.lightViewFocus_)
	};
	DirectX::XMVECTOR E =
	{
		DirectX::XMVectorSubtract(F,
		DirectX::XMVectorScale(
			DirectX::XMVector3Normalize(
				DirectX::XMLoadFloat4(&Graphics::Instance().GetShader()->view.position)), shadowData_.lightViewDistance_))
	};
	DirectX::XMVECTOR U =
	{
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	};
	DirectX::XMMATRIX V =
	{
		DirectX::XMMatrixLookAtLH(E,F,U)
	};
	DirectX::XMMATRIX P =
	{
		DirectX::XMMatrixOrthographicLH(shadowData_.lightViewSize_ * aspectRatio,
		shadowData_.lightViewSize_, shadowData_.lightViewNearZ_, shadowData_.lightViewFarZ_)
	};

	DirectX::XMFLOAT4X4 result = {};
	DirectX::XMStoreFloat4x4(&result, V * P);

	return result;
}
