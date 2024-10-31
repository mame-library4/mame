#include "ParticleData.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"

// ----- コンストラクタ -----
ParticleData::ParticleData(const size_t& particleCount)
    : maxParticleCount_(particleCount), isActive_(false)
{
}

// ----- 生成 -----
void ParticleData::CreateParticleData(const UINT& particleDataSize, const UINT& cbSize, const char* vsCsoName,
	const char* psCsoName, const char* gsCsoName, const char* csInitCsoName, const char* csUpdateCsoName)
{
    HRESULT result = S_OK;
	
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(particleDataSize * maxParticleCount_);
	bufferDesc.StructureByteStride = particleDataSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, NULL, particleBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shaderResourceViewDesc.Buffer.ElementOffset = 0;
	shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(maxParticleCount_);
	result = Graphics::Instance().GetDevice()->CreateShaderResourceView(particleBuffer_.Get(), &shaderResourceViewDesc, particleBufferSRV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc = {};
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	unorderedAccessViewDesc.Buffer.FirstElement = 0;
	unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(maxParticleCount_);
	unorderedAccessViewDesc.Buffer.Flags = 0;
	result = Graphics::Instance().GetDevice()->CreateUnorderedAccessView(particleBuffer_.Get(), &unorderedAccessViewDesc, particleBufferUAV_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	bufferDesc.ByteWidth = cbSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	result = Graphics::Instance().GetDevice()->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

	Graphics::Instance().CreateVsFromCso(vsCsoName,		  particleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	Graphics::Instance().CreatePsFromCso(psCsoName,		  particlePS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateGsFromCso(gsCsoName,		  particleGS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso(csInitCsoName,   particleInitializeCS_.ReleaseAndGetAddressOf());
	Graphics::Instance().CreateCsFromCso(csUpdateCsoName, particleUpdateCS_.ReleaseAndGetAddressOf());
}

// ----- 再生 -----
void ParticleData::PlayParticle(const int& csSlot, const int& cbSlot, void* data)
{
	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
	deviceContext->CSSetUnorderedAccessViews(csSlot, 1, particleBufferUAV_.GetAddressOf(), NULL);

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, data, 0, 0);
	deviceContext->CSSetConstantBuffers(cbSlot, 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleInitializeCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(csSlot, 1, &nullUnorderedAccessView, NULL);

	isActive_ = true;
}

// ----- 更新 -----
void ParticleData::Update(const int& csSlot, const int& cbSlot, void* data)
{
	if (isActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->CSSetUnorderedAccessViews(csSlot, 1, particleBufferUAV_.GetAddressOf(), NULL);

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, data, 0, 0);
	deviceContext->CSSetConstantBuffers(cbSlot, 1, constantBuffer_.GetAddressOf());

	deviceContext->CSSetShader(particleUpdateCS_.Get(), NULL, 0);

	const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
	deviceContext->Dispatch(threadGroupCountX, 1, 1);

	ID3D11UnorderedAccessView* nullUnorderedAccessView = {};
	deviceContext->CSSetUnorderedAccessViews(csSlot, 1, &nullUnorderedAccessView, NULL);
}

// ----- 描画 -----
void ParticleData::Render(const int& gsSlot, const int& cbSlot, void* data)
{
	if (isActive_ == false) return;

	ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

	deviceContext->VSSetShader(particleVS_.Get(), NULL, 0);
	deviceContext->PSSetShader(particlePS_.Get(), NULL, 0);
	deviceContext->GSSetShader(particleGS_.Get(), NULL, 0);
	deviceContext->GSSetShaderResources(gsSlot, 1, particleBufferSRV_.GetAddressOf());

	deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, data, 0, 0);
	deviceContext->VSSetConstantBuffers(cbSlot, 1, constantBuffer_.GetAddressOf());
	deviceContext->PSSetConstantBuffers(cbSlot, 1, constantBuffer_.GetAddressOf());
	deviceContext->GSSetConstantBuffers(cbSlot, 1, constantBuffer_.GetAddressOf());

	deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->Draw(static_cast<UINT>(maxParticleCount_), 0);

	ID3D11ShaderResourceView* nullShaderResourceView{};
	deviceContext->GSSetShaderResources(gsSlot, 1, &nullShaderResourceView);
	deviceContext->VSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
}
