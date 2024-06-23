#include "DemoScene.h"
#include "Graphics.h"
#include "Camera.h"

void DemoScene::CreateResource()
{
    //model_ = std::make_unique<Object>("./Resources/Model/Character/SKM_Manny.glb");
    //model_ = std::make_unique<Object>("./Resources/Model/Character/unitychan.glb");
    model_ = std::make_unique<Object>("./Resources/Model/Character/Salute.glb", 1.0f);
    //model_ = std::make_unique<Object>("./Resources/Model/Character/Player/Orc.gltf");
}

void DemoScene::Initialize()
{
    model_->PlayAnimation(1, true, 0.2f);

    
}

void DemoScene::Finalize()
{
}

void DemoScene::Update(const float& elapsedTime)
{
    Camera::Instance().SetTarget(target_);

    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, model_->GetTransform()->CalcWorldMatrix(0.01f));
   
    int jointIndex = 2;
    //int jointIndex = 48;
    //int jointIndex = model_->GetNodeIndex("head");


    GltfModel::Node& node = model_->GetNodes()->at(jointIndex);
    DirectX::XMFLOAT4 jointPosition = { node.globalTransform_._41, node.globalTransform_._42,node.globalTransform_._43, 1.0f };

    DirectX::XMFLOAT4 targetPosition = { target_.x, target_.y, target_.z, 1.0f };
    DirectX::XMStoreFloat4(&targetPosition, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&targetPosition), DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&world))));
    
    DirectX::XMFLOAT3 toTarget = { targetPosition.x - jointPosition.x, targetPosition.y - jointPosition.y ,targetPosition.z - jointPosition.z };
    DirectX::XMFLOAT3 forward  = { node.globalTransform_._31, node.globalTransform_._32,node.globalTransform_._33 };
    //DirectX::XMFLOAT3 forward = { 0,0,1 };

    DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&node.globalTransform_));
    DirectX::XMStoreFloat3(&toTarget, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&toTarget), inverseGlobalTransform));
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&forward), inverseGlobalTransform));

    DirectX::XMVECTOR axis = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&forward), DirectX::XMLoadFloat3(&toTarget));
    float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(DirectX::XMLoadFloat3(&forward), DirectX::XMLoadFloat3(&toTarget)));
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(axis), angle);

    DirectX::XMStoreFloat4x4(&node.globalTransform_, rotation * DirectX::XMLoadFloat4x4(&node.globalTransform_));

    std::function<void(int, int)> traverse = [&](int parentIndex, int nodeIndex)
    {
        GltfModel::Node& node = model_->GetNodes()->at(nodeIndex);
        if (parentIndex > -1)
        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&model_->GetNodes()->at(parentIndex).globalTransform_));
        }
        for (int childIndex : node.children_)
        {
            traverse(nodeIndex, childIndex);
        }
    };
    traverse(-1, jointIndex);
}

void DemoScene::ShadowRender()
{
}

void DemoScene::DeferredRender()
{
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();
    model_->Render(gBufferPixelShader);
}

void DemoScene::ForwardRender()
{
    model_->Render();
}

void DemoScene::UserInterfaceRender()
{
}

void DemoScene::Render()
{
}

void DemoScene::DrawDebug()
{
    model_->GetTransform()->DrawDebug();

    if (ImGui::Button("Init"))
    {
        model_->GetTransform()->SetPosition({});
    }
    ImGui::DragFloat3("target", &target_.x);
}
