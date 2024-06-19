#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- コンストラクタ -----
Object::Object(const std::string filename)
    : gltfModel_(filename)
{
}

// ----- デストラクタ -----
Object::~Object()
{
}

void Object::Update(const float& elapsedTime)
{
    gltfModel_.UpdateAnimation(elapsedTime);
}

// ----- 描画 -----
void Object::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
    gltfModel_.Render(scaleFactor, psShader);
}

// ----- ImGui用 -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}

// ----- weight値加算 -----
void Object::AddWeight(const float& weight)
{
    // weightは0~1の間に収める
    float w = GetWeight();
    w += weight;
    w = std::clamp(w, 0.0f, 1.0f);
    SetWeight(w);
}

void Object::RootMotionInitialize()
{
    animatedNodes_ = gltfModel_.nodes_;
    gltfModel_.Animate(0, 0, animatedNodes_);
    zeroAnimatedNodes_ = animatedNodes_;
}

void Object::RootMotionUpdate(const float& elapsedTime, const std::string& rootName)
{   
    gltfModel_.Animate(GetBlendAnimationIndex2(), GetBlendAnimationSeconds(), animatedNodes_);
    const int rootJointIndex = GetNodeIndex(rootName);

    GltfModel::Node& node = gltfModel_.nodes_.at(rootJointIndex);

    if (GetBlendAnimationSeconds() == 0)
    {
        previousPosition_ = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };
    }

    DirectX::XMFLOAT3 position = { node.globalTransform_._41, node.globalTransform_._42, node.globalTransform_._43 };
    DirectX::XMFLOAT3 displacement = { position.x - previousPosition_.x, position.y - previousPosition_.y, position.z - previousPosition_.z };

    DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&GetTransform()->GetCoordinateSystemTransforms(Transform::CoordinateSystem::cRightYup));
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(GetTransform()->GetScale().x, GetTransform()->GetScale().y, GetTransform()->GetScale().z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(GetTransform()->GetRotationX(), GetTransform()->GetRotationY(), GetTransform()->GetRotationZ());
    DirectX::XMStoreFloat3(&displacement, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&displacement), C * S * R));

    DirectX::XMFLOAT3 translation = GetTransform()->GetPosition();
    translation.x += displacement.x;
    translation.y += displacement.y;
    translation.z += displacement.z;
    GetTransform()->SetPosition(translation);

    node.globalTransform_._41 = zeroAnimatedNodes_.at(GetBlendAnimationIndex2()).globalTransform_._41;
    node.globalTransform_._42 = zeroAnimatedNodes_.at(GetBlendAnimationIndex2()).globalTransform_._42;
    node.globalTransform_._43 = zeroAnimatedNodes_.at(GetBlendAnimationIndex2()).globalTransform_._43;

    std::function<void(int, int)> traverse = [&](int parentIndex, int nodeIndex)
    {
        GltfModel::Node& node = animatedNodes_.at(nodeIndex);
        if (parentIndex > -1)
        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale_.x, node.scale_.y, node.scale_.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation_.x, node.rotation_.y, node.rotation_.z, node.rotation_.w));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translation_.x, node.translation_.y, node.translation_.z);
            DirectX::XMStoreFloat4x4(&node.globalTransform_, S * R * T * DirectX::XMLoadFloat4x4(&animatedNodes_.at(parentIndex).globalTransform_));
        }
        for (int childIndex : node.children_)
        {
            traverse(nodeIndex, childIndex);
        }
    };
    traverse(-1, rootJointIndex);
    
    previousPosition_ = position;
}
