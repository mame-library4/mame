#include "FireBallParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"

// ----- コンストラクタ -----
FireBallParticle::FireBallParticle()
    :ParticleSystem(3000)
{
	CreateFireBallParticle();
}

void FireBallParticle::Update(const float& elapsedTime)
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(csUAVSlot_, 1, particleBufferUAV_.GetAddressOf(), NULL);

	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->CSSetConstantBuffers(cbSlot_, 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleUpdateCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(csUAVSlot_, 1, &nullUnorderedAccessView, NULL);
}

void FireBallParticle::Render()
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->VSSetShader(particleVS_.Get(), NULL, 0);
	deviceContext->PSSetShader(particlePS_.Get(), NULL, 0);
	deviceContext->GSSetShader(particleGS_.Get(), NULL, 0);
	deviceContext->GSSetShaderResources(gsSRVSlot_, 1, particleBufferSRV_.GetAddressOf());

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->VSSetConstantBuffers(cbSlot_, 1, constantBuffer_.GetAddressOf());
	deviceContext->PSSetConstantBuffers(cbSlot_, 1, constantBuffer_.GetAddressOf());
	deviceContext->GSSetConstantBuffers(cbSlot_, 1, constantBuffer_.GetAddressOf());

	deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->Draw(static_cast<UINT>(maxParticleCount_), 0);

	ID3D11ShaderResourceView* nullShaderResourceView{};
	deviceContext->GSSetShaderResources(gsSRVSlot_, 1, &nullShaderResourceView);
	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
}

void FireBallParticle::DrawDebug()
{
}

void FireBallParticle::PlayFireBallParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(csUAVSlot_, 1, particleBufferUAV_.GetAddressOf(), NULL);

	constants_.emitterPosition_ = emitterPosition;
	constants_.currentPosition_ = emitterPosition;
	constants_.oldPosition_		= emitterPosition;
	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;
	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
	deviceContext->CSSetConstantBuffers(cbSlot_, 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleInitializeCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(csUAVSlot_, 1, &nullUnorderedAccessView, NULL);
}

void FireBallParticle::UpdateFireBallParticle(const DirectX::XMFLOAT3& position)
{
	constants_.oldPosition_ = constants_.currentPosition_;
	constants_.currentPosition_ = position;
}

// ----- 生成 -----
void FireBallParticle::CreateFireBallParticle()
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

	Graphics::Instance().CreateVsFromCso("./Resources/Shader/ParticleVS.cso", particleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	Graphics::Instance().CreatePsFromCso("./Resources/Shader/FireBallPS.cso", particlePS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateGsFromCso("./Resources/Shader/FireBallGS.cso", particleGS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/FireBallUpdateCS.cso", particleUpdateCS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso("./Resources/Shader/FireBallInitializeCS.cso", particleInitializeCS_.ReleaseAndGetAddressOf());
}
