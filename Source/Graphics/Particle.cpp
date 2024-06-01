#include "Particle.h"
#include "Graphics.h"
#include "Misc.h"

#include "../Core/Application.h"

// ----- コンストラクタ -----
Particles::Particles(size_t particleCount) : maxParticleCount(particleCount)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Particle) * particleCount);
    bufferDesc.StructureByteStride = sizeof(Particle);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    hr = device->CreateBuffer(&bufferDesc, NULL, particleBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceViewDesc.Buffer.ElementOffset = 0;
    shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    hr = device->CreateShaderResourceView(particleBuffer.Get(), &shaderResourceViewDesc, particleBufferSrv.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
    unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    unorderedAccessViewDesc.Buffer.FirstElement = 0;
    unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
    unorderedAccessViewDesc.Buffer.Flags = 0;
    hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &unorderedAccessViewDesc, particleBufferUav.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    bufferDesc.ByteWidth = sizeof(ParticleConstants);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    Graphics::Instance().CreateVsFromCso("./Resources/Shader/ParticleVS.cso", particleVS.ReleaseAndGetAddressOf(), NULL, NULL, 0);
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/ParticlePS.cso", particlePS.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateGsFromCso("./Resources/Shader/ParticleGS.cso", particleGS.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ParticleCS.cso", particleCS.ReleaseAndGetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ParticleInitializerCS.cso", particleInitializerCS.ReleaseAndGetAddressOf());
}

UINT align(UINT num, UINT alignment)
{
    return (num + (alignment - 1)) & ~(alignment - 1);
}

void Particles::Integrate(float deltaTime)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->CSSetUnorderedAccessViews(0, 1, particleBufferUav.GetAddressOf(), NULL);

    particleData.time += deltaTime;
    particleData.deltaTime = deltaTime;
    particleData.options = deltaTime;

    deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
    deviceContext->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

    deviceContext->CSSetShader(particleCS.Get(), NULL, 0);

    const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
    deviceContext->Dispatch(threadGroupCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

void Particles::Initialize(float deltaTime)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->CSSetUnorderedAccessViews(0, 1, particleBufferUav.GetAddressOf(), NULL);

    particleData.time += deltaTime;
    particleData.deltaTime = deltaTime;
    particleData.options = deltaTime;

    deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);

    deviceContext->CSSetShader(particleInitializerCS.Get(), NULL, 0);

    const UINT threadGroupCountX = align(static_cast<UINT>(maxParticleCount), NUMTHREADS_X) / NUMTHREADS_X;
    deviceContext->Dispatch(threadGroupCountX, 1, 1);

    ID3D11UnorderedAccessView* nullUnorderedAccessView{};
    deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
}

void Particles::Render()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->VSSetShader(particleVS.Get(), NULL, 0);
    deviceContext->PSSetShader(particlePS.Get(), NULL, 0);
    deviceContext->GSSetShader(particleGS.Get(), NULL, 0);
    deviceContext->GSSetShaderResources(9, 1, particleBufferSrv.GetAddressOf());

    deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleData, 0, 0);
    deviceContext->VSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
    deviceContext->PSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());

    deviceContext->IASetInputLayout(NULL);
    deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    deviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    deviceContext->Draw(static_cast<UINT>(maxParticleCount), 0);

    ID3D11ShaderResourceView* nullShaderResourceView{};
    deviceContext->GSSetShaderResources(9, 1, &nullShaderResourceView);
    deviceContext->VSSetShader(NULL, NULL, 0);
    deviceContext->PSSetShader(NULL, NULL, 0);
    deviceContext->GSSetShader(NULL, NULL, 0);
}

void Particles::DrawDebug()
{
    if (ImGui::TreeNode("particle"))
    {
        ImGui::DragFloat3("emitterPosition", &particleData.emitterPosition.x);
        ImGui::DragFloat("particleSize", &particleData.particleSize);
        ImGui::ColorEdit4("color", &particleData.color.x);
        ImGui::DragFloat("time", &particleData.time);
        ImGui::DragFloat("deltaTime", &particleData.deltaTime);

        ImGui::DragFloat2("scrollDirection", &particleData.scrollDirection.x);
        ImGui::DragFloat("options", &particleData.options);

        ImGui::SliderFloat("dissolve", &particleData.dissolveParameters.x,0.0f,1.0f);
        ImGui::SliderFloat("threshold", &particleData.dissolveParameters.y,0.0f,1.0f);
        ImGui::ColorEdit4("thresholdColor", &particleData.edgeColor.x);

        ImGui::TreePop();
    }
    ImGui::Separator();

}
