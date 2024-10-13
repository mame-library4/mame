#include "Sword.h"
#include "MathHelper.h"

// ----- çXêV -----
void Sword::Update(const DirectX::XMMATRIX& start, const DirectX::XMMATRIX& end)
{
    primitiveRenderer_.ClearVertices();

    //primitiveRenderer_.AddVertex(position + vertexOffset_[0], { 1,0,0,1 }, {});
    //primitiveRenderer_.AddVertex(position + vertexOffset_[1], { 0,1,0,1 }, {});
    //primitiveRenderer_.AddVertex(endPosition + vertexOffset_[2], { 0,0,1,1 }, {});
    //primitiveRenderer_.AddVertex(endPosition + vertexOffset_[3], { 1,1,0,1 }, {});

    DirectX::XMFLOAT4 color[8] =
    {
        { 1,0,0,1 },
        { 0,1,0,1 },
        { 0,0,1,1 },
        { 1,1,0,1 },
        
        { 1,0,0,1 },
        { 0,1,0,1 },
        { 0,0,1,1 },
        { 1,1,0,1 },
    };

    AddVertex(0, start);
    AddVertex(1, start);
    AddVertex(2, start);
    AddVertex(3, start);
    AddVertex(4, end);
    AddVertex(5, end);
    AddVertex(6, end);
    AddVertex(7, end);
    
    AddVertex(0, start);
    AddVertex(1, start);
    
    AddVertex(2, start);
    AddVertex(6, end);
    AddVertex(4, end);

    AddVertex(3, start);
    AddVertex(5, end);
    AddVertex(7, end);
    AddVertex(0, start);
}

// ----- ï`âÊ -----
void Sword::Render()
{
    primitiveRenderer_.Render();
}

void Sword::DrawDebug()
{
    if (ImGui::TreeNode("Sword"))
    {
        ImGui::DragFloat3("vertexOffset0", &vertexOffset_[0].x);
        ImGui::DragFloat3("vertexOffset1", &vertexOffset_[1].x);
        ImGui::DragFloat3("vertexOffset2", &vertexOffset_[2].x);
        ImGui::DragFloat3("vertexOffset3", &vertexOffset_[3].x);
        ImGui::DragFloat3("vertexOffset4", &vertexOffset_[4].x);
        ImGui::DragFloat3("vertexOffset5", &vertexOffset_[5].x);
        ImGui::DragFloat3("vertexOffset6", &vertexOffset_[6].x);
        ImGui::DragFloat3("vertexOffset7", &vertexOffset_[7].x);

        ImGui::TreePop();
    }
}

void Sword::AddVertex(const int& index, const DirectX::XMMATRIX& matrix)
{
    const DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    const DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
    const DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(vertexOffset_[index].x, vertexOffset_[index].y, vertexOffset_[index].z);
    DirectX::XMMATRIX M = S * R * T * matrix;
    DirectX::XMFLOAT3 pos = {};
    DirectX::XMStoreFloat3(&pos, M.r[3]);
    primitiveRenderer_.AddVertex(pos, {1,0,0,1}, {});
}
