#include "StoneManager.h"
#include "Graphics.h"
#include "Common.h"


void StoneManager::Finalize()
{
    Clear();
}

void StoneManager::Update(const float& elapsedTime)
{
    // ----- 生成 -----
    for (Stone* stone : generates_)
    {
        stones_.emplace_back(stone);
    }
    generates_.clear();

    for (Stone* stone : removes_)
    {
        auto it = std::find(stones_.begin(), stones_.end(), stone);

        if (it != stones_.end())
        {
            stones_.erase(it);
        }

        SafeDeletePtr(stone);
    }
    removes_.clear();
}

void StoneManager::Render(ID3D11PixelShader* psShader)
{
    // ----- インスタンスデータ -----
    const int instanceCount = stones_.size();
    instanceCount_ = instanceCount;
    std::vector<InstanceData> instances;
    for (int i = 0; i < instanceCount; ++i)
    {
        DirectX::XMFLOAT4X4 world = {};
        DirectX::XMStoreFloat4x4(&world, stones_.at(i)->GetTransform()->CalcWorldMatrix(1.0f));

        InstanceData data = { world };
        instances.emplace_back(data);
    }
    
    const int vertexCount = 21 * instanceCount;
    vertexCount_ = vertexCount;

    D3D11_BUFFER_DESC instanceBufferDesc = {};
    instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    instanceBufferDesc.ByteWidth = sizeof(InstanceData) * instanceCount;
    instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA instanceData = {};
    instanceData.pSysMem = instances.data();

    Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
    Graphics::Instance().GetDevice()->CreateBuffer(&instanceBufferDesc, &instanceData, instanceBuffer.GetAddressOf());

    UINT strides = sizeof(InstanceData);
    UINT offsets = 0;
    Graphics::Instance().GetDeviceContext()->IASetVertexBuffers(1, 1, instanceBuffer.GetAddressOf(), &strides, &offsets);

    Graphics::Instance().GetDeviceContext()->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void StoneManager::DrawDebug()
{
    if (ImGui::BeginMenu("Stones"))
    {
        ImGui::DragInt("TotalVertexCount", &vertexCount_);
        ImGui::DragInt("InstanceCount", &instanceCount_);

        ImGui::EndMenu();
    }
}

void StoneManager::Register(Stone* stone)
{
    generates_.insert(stone);
}

void StoneManager::Remove(Stone* stone)
{
    removes_.insert(stone);
}

void StoneManager::Clear()
{
    for (Stone*& stone : stones_)
    {
        SafeDeletePtr(stone);
    }
    stones_.clear();
    stones_.shrink_to_fit();
}

