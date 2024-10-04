#include "SnowParticle.h"
#include "Graphics.h"
#include "Misc.h"
#include "Texture.h"

// ----- コンストラクタ -----
SnowParticle::SnowParticle()
    : ParticleSystem(1000)
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
    unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
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

    Graphics::Instance().CreateVsFromCso("./Resources/Shader/ParticleVS.cso", particleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/ParticlePS.cso", particlePS_.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateGsFromCso("./Resources/Shader/ParticleGS.cso", particleGS_.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/SnowParticleCS.cso", particleUpdateCS_.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/SnowParticleInitCS.cso", particleInitializeCS_.ReleaseAndGetAddressOf());    
}

// ----- 初期設定 -----
void SnowParticle::Initialize(const float& deltaTime)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->CSSetUnorderedAccessViews(0, 1, particleBufferUAV_.GetAddressOf(), NULL);

    constants_.deltaTime_ = deltaTime;

    deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
    //deviceContext->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

    deviceContext->CSSetShader(particleInitializeCS_.Get(), NULL, 0);

    const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
    deviceContext->Dispatch(threadGroupCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

// ----- 更新 -----
void SnowParticle::Update(const float& deltaTime)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->CSSetUnorderedAccessViews(0, 1, particleBufferUAV_.GetAddressOf(), NULL);

    constants_.deltaTime_ = deltaTime;

    deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
    //deviceContext->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

    deviceContext->CSSetShader(particleUpdateCS_.Get(), NULL, 0);

    const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
    deviceContext->Dispatch(threadGroupCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

// ----- 描画 -----
void SnowParticle::Render()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->VSSetShader(particleVS_.Get(), NULL, 0);
    deviceContext->PSSetShader(particlePS_.Get(), NULL, 0);
    deviceContext->GSSetShader(particleGS_.Get(), NULL, 0);
    deviceContext->GSSetShaderResources(9, 1, particleBufferSRV_.GetAddressOf());

    deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &constants_, 0, 0);
    deviceContext->VSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
    deviceContext->PSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
    deviceContext->GSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

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

void SnowParticle::DrawDebug()
{
    if (ImGui::TreeNode("SnowParticle"))
    {
        ImGui::ColorEdit4("Color", &constants_.color_.x);

        ImGui::TreePop();
    }
}
