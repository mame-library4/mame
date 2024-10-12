#include "SuperNovaParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"

// ----- コンストラクタ -----
SuperNovaParticle::SuperNovaParticle()
	: ParticleSystem(1000)
{
	CreateLavaCrawlerParticle(); // 地面を這うパーティクル生成
	CreateCoreBurstParticle();	 // メインの爆発パーティクル生成

	constants_.speed_ = 20.0f;
	constants_.particleSize_ = 0.25f;

	lavaCrawlerParticleConstants_.speed_ = 10.0f;
	lavaCrawlerParticleConstants_.particleSize_ = 0.05f;
}

// ----- 更新 -----
void SuperNovaParticle::Update(const float& elapsedTime)
{
	UpdateLavaCrawlerParticle(elapsedTime); // 地面を這うパーティクル更新
	UpdateCoreBurstParticle(elapsedTime);   // メインの爆発パーティクル更新
}

// ----- 描画 -----
void SuperNovaParticle::Render()
{	
	RenderLavaCrawlerParticle(); // 地面を這うパーティクル描画
	RenderCoreBurstParticle();	 // メインの爆発パーティクル描画
}

// ----- ImGui用 -----
void SuperNovaParticle::DrawDebug()
{
	if (ImGui::TreeNode("SuperNovaParticle"))
	{
		if (ImGui::TreeNode("LavaCrawlerParticle"))
		{
			ImGui::DragFloat("Speed", &lavaCrawlerParticleConstants_.speed_);
			ImGui::DragFloat("Size", &lavaCrawlerParticleConstants_.particleSize_);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("CoreBurstParticle"))
		{
			ImGui::DragFloat("Speed", &constants_.speed_);
			ImGui::DragFloat("Size", &constants_.particleSize_);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}


#pragma region 地面を這うパーティクル
// ----- 生成 -----
void SuperNovaParticle::CreateLavaCrawlerParticle()
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * maxLavaCrawlerParticleCount_);
	bufferDesc.StructureByteStride = sizeof(ParticleData);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, NULL, lavaCrawlerParticleBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shaderResourceViewDesc.Buffer.ElementOffset = 0;
	shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(maxLavaCrawlerParticleCount_);
	result = Graphics::Instance().GetDevice()->CreateShaderResourceView(lavaCrawlerParticleBuffer_.Get(), &shaderResourceViewDesc, lavaCrawlerParticleBufferSRV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	unorderedAccessViewDesc.Buffer.FirstElement = 0;
	unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(maxLavaCrawlerParticleCount_);
	unorderedAccessViewDesc.Buffer.Flags = 0;
	result = Graphics::Instance().GetDevice()->CreateUnorderedAccessView(lavaCrawlerParticleBuffer_.Get(), &unorderedAccessViewDesc, lavaCrawlerParticleBufferUAV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, nullptr, lavaCrawlerParticleConstantBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_TEXTURE2D_DESC textureDesc = {};
	Texture::Instance().LoadTexture(L"./Resources/Image/White.png", lavaCrawlerParticleSRV_.GetAddressOf(), &textureDesc);
	//Texture::Instance().LoadTexture(L"./Resources/Image/Particle/Soft.png", lavaCrawlerParticleSRV_.GetAddressOf(), &textureDesc);

	// TODO : Shaderファイル名を変更
	Graphics::Instance().CreateVsFromCso("./Resources/Shader/ParticleVS.cso",			   lavaCrawlerParticleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	Graphics::Instance().CreatePsFromCso("./Resources/Shader/LavaCrawlerPS.cso",		   lavaCrawlerParticlePS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateGsFromCso("./Resources/Shader/LavaCrawlerGS.cso",		   lavaCrawlerParticleGS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/LavaCrawlerUpdateCS.cso",	   lavaCrawlerParticleUpdateCS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/LavaCrawlerInitializeCS.cso", lavaCrawlerParticleInitializeCS_.ReleaseAndGetAddressOf());
}

// ----- 再生 -----
void SuperNovaParticle::PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	isCrawlerParticleActive_ = true;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleBufferUAV_.GetAddressOf(), NULL);

	lavaCrawlerParticleConstants_.emitterPosition_ = emitterPosition;
	lavaCrawlerParticleConstants_.time_ += elapsedTime;
	lavaCrawlerParticleConstants_.deltaTime_ = elapsedTime;
	deviceContext->UpdateSubresource(lavaCrawlerParticleConstantBuffer_.Get(), 0, 0, &lavaCrawlerParticleConstants_, 0, 0);
	deviceContext->CSSetConstantBuffers(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleConstantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(lavaCrawlerParticleInitializeCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxLavaCrawlerParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), 1, &nullUnorderedAccessView, NULL);
}

// ----- 更新 -----
void SuperNovaParticle::UpdateLavaCrawlerParticle(const float& elapsedTime)
{
	if (isCrawlerParticleActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleBufferUAV_.GetAddressOf(), NULL);

	lavaCrawlerParticleConstants_.time_ += elapsedTime;
	lavaCrawlerParticleConstants_.deltaTime_ = elapsedTime;

	deviceContext->UpdateSubresource(lavaCrawlerParticleConstantBuffer_.Get(), 0, 0, &lavaCrawlerParticleConstants_, 0, 0);
	deviceContext->CSSetConstantBuffers(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleConstantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(lavaCrawlerParticleUpdateCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxLavaCrawlerParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), 1, &nullUnorderedAccessView, NULL);
}

// ----- 描画 -----
void SuperNovaParticle::RenderLavaCrawlerParticle()
{
	if (isCrawlerParticleActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->VSSetShader(lavaCrawlerParticleVS_.Get(), NULL, 0);
	deviceContext->PSSetShader(lavaCrawlerParticlePS_.Get(), NULL, 0);
	deviceContext->GSSetShader(lavaCrawlerParticleGS_.Get(), NULL, 0);
	deviceContext->GSSetShaderResources(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleBufferSRV_.GetAddressOf());

	deviceContext->UpdateSubresource(lavaCrawlerParticleConstantBuffer_.Get(), 0, 0, &lavaCrawlerParticleConstants_, 0, 0);
	deviceContext->VSSetConstantBuffers(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleConstantBuffer_.GetAddressOf());
	deviceContext->PSSetConstantBuffers(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleConstantBuffer_.GetAddressOf());
	deviceContext->GSSetConstantBuffers(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, lavaCrawlerParticleConstantBuffer_.GetAddressOf());

	deviceContext->PSSetShaderResources(0, 1, lavaCrawlerParticleSRV_.GetAddressOf());

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->Draw(static_cast<UINT>(maxLavaCrawlerParticleCount_), 0);

	ID3D11ShaderResourceView* nullShaderResourceView{};
	deviceContext->GSSetShaderResources(static_cast<int>(CBSlot::LavaCrawlerParticle), 1, &nullShaderResourceView);
	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
}

#pragma endregion 地面を這うパーティクル


#pragma region メインの爆発パーティクル
// ----- 生成 -----
void SuperNovaParticle::CreateCoreBurstParticle()
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * maxParticleCount_);
	bufferDesc.StructureByteStride = sizeof(ParticleData);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, NULL, particleBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shaderResourceViewDesc.Buffer.ElementOffset = 0;
	shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(maxParticleCount_);
	result = Graphics::Instance().GetDevice()->CreateShaderResourceView(particleBuffer_.Get(), &shaderResourceViewDesc, particleBufferSRV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	unorderedAccessViewDesc.Buffer.FirstElement = 0;
	unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(maxParticleCount_);
	unorderedAccessViewDesc.Buffer.Flags = 0;
	result = Graphics::Instance().GetDevice()->CreateUnorderedAccessView(particleBuffer_.Get(), &unorderedAccessViewDesc, particleBufferUAV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_TEXTURE2D_DESC textureDesc = {};
	Texture::Instance().LoadTexture(L"./Resources/Image/Particle/Soft.png", shaderResourceView_.GetAddressOf(), &textureDesc);

	Graphics::Instance().CreateVsFromCso("./Resources/Shader/ParticleVS.cso",			 particleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	Graphics::Instance().CreatePsFromCso("./Resources/Shader/SuperNovaParticlePS.cso",   particlePS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateGsFromCso("./Resources/Shader/CoreBurstGS.cso",			 particleGS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/CoreBurstUpdateCS.cso",	 particleUpdateCS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/CoreBurstInitializeCS.cso", particleInitializeCS_.ReleaseAndGetAddressOf());
}

// ----- 再生 -----
void SuperNovaParticle::PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	isCoreBurstParticleActive_ = true;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::CoreBurstParticle), 1, particleBufferUAV_.GetAddressOf(), NULL);

	constants_.emitterPosition_ = emitterPosition;
	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;
	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->CSSetConstantBuffers(static_cast<int>(CBSlot::CoreBurstParticle), 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleInitializeCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::CoreBurstParticle), 1, &nullUnorderedAccessView, NULL);
}

// ----- 更新 -----
void SuperNovaParticle::UpdateCoreBurstParticle(const float& elapsedTime)
{
	if (isCoreBurstParticleActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::CoreBurstParticle), 1, particleBufferUAV_.GetAddressOf(), NULL);

	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->CSSetConstantBuffers(static_cast<int>(CBSlot::CoreBurstParticle), 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleUpdateCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(static_cast<int>(CSShaderSlot::CoreBurstParticle), 1, &nullUnorderedAccessView, NULL);
}

// ----- 描画 -----
void SuperNovaParticle::RenderCoreBurstParticle()
{
	if (isCoreBurstParticleActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->VSSetShader(particleVS_.Get(), NULL, 0);
	deviceContext->PSSetShader(particlePS_.Get(), NULL, 0);
	deviceContext->GSSetShader(particleGS_.Get(), NULL, 0);
	deviceContext->GSSetShaderResources(9, 1, particleBufferSRV_.GetAddressOf());

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->VSSetConstantBuffers(static_cast<int>(CBSlot::CoreBurstParticle), 1, constantBuffer_.GetAddressOf());
	deviceContext->PSSetConstantBuffers(static_cast<int>(CBSlot::CoreBurstParticle), 1, constantBuffer_.GetAddressOf());
	deviceContext->GSSetConstantBuffers(static_cast<int>(CBSlot::CoreBurstParticle), 1, constantBuffer_.GetAddressOf());

	deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->Draw(static_cast<UINT>(maxParticleCount_), 0);

	ID3D11ShaderResourceView* nullShaderResourceView{};
	deviceContext->GSSetShaderResources(9, 1, &nullShaderResourceView);
	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
}

#pragma endregion メインの爆発パーティクル
