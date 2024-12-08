#include "ComputeParticleSystem.h"
#include "Graphics.h"
#include "Misc.h"
#include "Texture.h"

// ----- コンストラクタ -----
ComputeParticleSystem::ComputeParticleSystem(UINT particlesCount, DirectX::XMUINT2 splitCount)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    // パーティクル数をスレッド数に合わせて制限
    numParticles_ = ((particlesCount + (numParticleThread_ - 1)) / numParticleThread_) * numParticleThread_;
    numEmitParticles_ = min(numParticles_, 10000); // 1フレームの生成制限数※要調整

    textureSplitCount_ = splitCount;
    oneShotInitialize_ = false;

    // 定数バッファ
    {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        bufferDesc.ByteWidth = sizeof(Constants);
        result = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    // パーティクルバッファ生成
    {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bufferDesc.ByteWidth = sizeof(ParticleData) * numParticles_;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(ParticleData);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        result = device->CreateBuffer(&bufferDesc, nullptr, particleDataBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        result = device->CreateShaderResourceView(particleDataBuffer_.Get(), nullptr, particleDataSRV_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        result = device->CreateUnorderedAccessView(particleDataBuffer_.Get(), nullptr, particleDataUAV_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    // パーティクルの生成 / 破棄番号をため込むバッファ生成
    {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        bufferDesc.ByteWidth = sizeof(UINT) * numParticles_;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(UINT);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        result = device->CreateBuffer(&bufferDesc, nullptr, particleAppendConsumeBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        //	Append / Consumeを利用する場合はビュー側にフラグを立てる
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = numParticles_;
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
        result = device->CreateUnorderedAccessView(particleAppendConsumeBuffer_.Get(), &uavDesc, particleAppendConsumeUAV_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    // パーティクルエミット用バッファ生成
    {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.ByteWidth = sizeof(EmitParticleData) * numEmitParticles_;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(EmitParticleData);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        result = device->CreateBuffer(&bufferDesc, nullptr, particleEmitBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        result = device->CreateShaderResourceView(particleEmitBuffer_.Get(), nullptr, particleEmitSRV_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    // パーティクルの更新・描画数の削減のためのバッファ
    {
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.ByteWidth = drawIndirectSize_;
        desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        desc.Usage = D3D11_USAGE_DEFAULT;
        result = device->CreateBuffer(&desc, nullptr, indirectDataBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        // Append / Consumeを利用する場合はビュー側にフラグを立てる
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = desc.ByteWidth / sizeof(UINT);
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        result = device->CreateUnorderedAccessView(indirectDataBuffer_.Get(), &uavDesc, indirectDataUAV_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    // コンピュートシェーダ読み込み
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ComputeParticleInitCS.cso", initShader_.GetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ComputeParticleEmitCS.cso", emitShader_.GetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ComputeParticleUpdateCS.cso", updateShader_.GetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ComputeParticleBeginFrameCS.cso", beginFrameShader_.GetAddressOf());
    Graphics::Instance().CreateCsFromCso("./Resources/Shader/ComputeParticleEndFrameCS.cso", endFrameShader_.GetAddressOf());

    // 描画用情報生成
    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    Texture::Instance().LoadTexture(L"./Resources/Image/Particle/Particle.png", shaderResourceView_.GetAddressOf(), &texture2dDesc);
    Graphics::Instance().CreateVsFromCso("./Resources/Shader/ComputeParticleRenderVS.cso", vertexShader_.GetAddressOf(), nullptr, nullptr, 0);
    Graphics::Instance().CreateGsFromCso("./Resources/Shader/ComputeParticleRenderGS.cso", geometryShader_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/ComputeParticleRenderPS.cso", pixelShader_.GetAddressOf());
}

// ----- デストラクタ -----
ComputeParticleSystem::~ComputeParticleSystem()
{
    initShader_.Reset();
    updateShader_.Reset();
    emitShader_.Reset();
    vertexShader_.Reset();
    geometryShader_.Reset();
    pixelShader_.Reset();
    shaderResourceView_.Reset();
}

// ----- 生成 -----
void ComputeParticleSystem::Emit(const EmitParticleData& data)
{
    if (emitParticles_.size() >= numEmitParticles_)return;

    emitParticles_.emplace_back(data);
}

// ----- 更新 -----
void ComputeParticleSystem::Update(const float& deltaTime)
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    // 定数バッファ設定
    {
        deviceContext->CSSetConstantBuffers(10, 1, constantBuffer_.GetAddressOf());
        deviceContext->PSSetConstantBuffers(10, 1, constantBuffer_.GetAddressOf());

        // 定数バッファ更新
        constants_.deltaTime_ = deltaTime;
        constants_.textureSplitCount_ = textureSplitCount_;
        constants_.systemNumParticles_ = numParticles_;
        constants_.totalEmitCount_ = static_cast<UINT>(emitParticles_.size());
        deviceContext->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &constants_, 0, 0);
    }

    // SRV / UAV設定
    {
        deviceContext->CSSetShaderResources(0, 1, particleEmitSRV_.GetAddressOf());

        ID3D11UnorderedAccessView* uavs[] =
        {
            particleDataUAV_.Get(),
            particleAppendConsumeUAV_.Get(),
            indirectDataUAV_.Get(),
        };
        deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
    }

    // 初期化処理
    if (!oneShotInitialize_)
    {
        oneShotInitialize_ = true;
        deviceContext->CSSetShader(initShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(numParticles_ / numParticleThread_, 1, 1);
    }

    // フレーム開始時の処理
    {
        // 現在フレームでのパーティクル総数を算出
        // それに合わせて各種設定を行う
        deviceContext->CSSetShader(beginFrameShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(1, 1, 1);
    }


    // エミット処理
    if (!emitParticles_.empty())
    {
        // エミットバッファ更新
        D3D11_BOX writeBox = {};
        writeBox.left = 0;
        writeBox.right = static_cast<UINT>(emitParticles_.size() * sizeof(EmitParticleData));
        writeBox.top = 0;
        writeBox.bottom = 1;
        writeBox.front = 0;
        writeBox.back = 1;
        deviceContext->UpdateSubresource(
            particleEmitBuffer_.Get(),
            0,
            &writeBox,
            emitParticles_.data(),
            static_cast<UINT>(emitParticles_.size() * sizeof(EmitParticleData)),
            0);
        deviceContext->CSSetShader(emitShader_.Get(), nullptr, 0);
        deviceContext->DispatchIndirect(indirectDataBuffer_.Get(), emitDispatchIndirectOffset_);
        emitParticles_.clear();
    }

    // 更新処理
    {
        deviceContext->CSSetShader(updateShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(numParticles_ / numParticleThread_, 1, 1);
    }

    // フレーム終了時の処理
    {
        // 総パーティクル数を変動させる
        deviceContext->CSSetShader(endFrameShader_.Get(), nullptr, 0);
        deviceContext->Dispatch(1, 1, 1);
    }

    // UAV設定
    {
        ID3D11UnorderedAccessView* uavs[] = { nullptr,nullptr,nullptr,nullptr };
        deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
    }
}

// ----- 描画 -----
void ComputeParticleSystem::Render()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    // 点描画設定
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // シェーダー設定
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    deviceContext->GSSetShader(geometryShader_.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);

    // 入力レイアウト設定
    deviceContext->IASetInputLayout(nullptr);

    // リソース設定
    deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
    deviceContext->GSSetShaderResources(0, 1, particleDataSRV_.GetAddressOf());

    // バッファクリア
    ID3D11Buffer* clearBuffer[] = { nullptr };
    UINT strides[] = { 0 };
    UINT offsets[] = { 0 };
    deviceContext->IASetVertexBuffers(0, 1, clearBuffer, strides, offsets);
    deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    // パーティクル情報分描画コール
    deviceContext->Draw(numParticles_, 0);

    // シェーダー無効化
    deviceContext->VSSetShader(nullptr, nullptr, 0);
    deviceContext->GSSetShader(nullptr, nullptr, 0);
    deviceContext->PSSetShader(nullptr, nullptr, 0);

    // リソースクリア
    ID3D11ShaderResourceView* clearShaderResourceViews[] = { nullptr };
    deviceContext->PSSetShaderResources(0, 1, clearShaderResourceViews);
    deviceContext->GSSetShaderResources(0, 1, clearShaderResourceViews);
}

void ComputeParticleSystem::DrawDebug()
{
    ImGui::DragFloat("ColorIntensity", &constants_.colorIntensity_);
}
