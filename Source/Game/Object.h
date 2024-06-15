#pragma once
#include "../Resource/GltfModel.h"

class Object
{
public:
    Object(const std::string filename);
    ~Object();

    void Update(const float& elapsedTime);
    void Render(const float& scaleFactor, ID3D11PixelShader* psShader = nullptr);
    void DrawDebug();

    void PlayAnimation(const int& index, const bool& loop, const float& speed) { gltfModel_.PlayAnimation(index, loop, speed); }
    void PlayBlendAnimation(const int& index1, const int& index2, const bool& loop, const float& speed) { gltfModel_.PlayBlendAnimation(index1, index2, loop, speed); }
    void PlayBlendAnimation(const int& index, const bool& loop, const float& speed) { gltfModel_.PlayBlendAnimation(index, loop, speed); }
    const bool IsPlayAnimation() { return gltfModel_.IsPlayAnimation(); }
    [[nodiscard]] const float GetAnimationSpeed() const { return gltfModel_.GetAnimationSpeed(); }
    void SetAnimationSpeed(const float& speed) { gltfModel_.SetAnimationSpeed(speed); }

public:
    // ---------- Transform ----------
    Transform* GetTransform() { return gltfModel_.GetTransform(); }

    // ---------- JointPosition ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeIndex, scaleFactor, offsetPosition); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeName, scaleFactor, offsetPosition); }
    
    DirectX::XMMATRIX GetJointGlobalTransform(const size_t& nodeIndex) { return gltfModel_.GetJointGlobalTransform(nodeIndex); }
    DirectX::XMMATRIX GetJointGlobalTransform(const std::string& nodeName) { return gltfModel_.GetJointGlobalTransform(nodeName); }
    DirectX::XMMATRIX GetJointWorldTransform(const std::string& nodeName, const float& scaleFacter) { return gltfModel_.GetJointWorldTransform(nodeName, scaleFacter); }


    // ---------- Animation ----------
    [[nodiscard]] const int GetCurrentBlendAnimationIndex() const { return gltfModel_.GetCurrentBlendAnimationIndex(); }
    [[nodiscard]] const float GetBlendAnimationSeconds() const { return gltfModel_.GetBlendAnimationSeconds(); }

    // ---------- weight ----------
    [[nodiscard]] const float GetWeight() { return gltfModel_.GetWeight(); }
    virtual void SetWeight(const float& weight) { gltfModel_.SetWeight(weight); }
    virtual void AddWeight(const float& weight);

    // ---------- Node ----------
    [[nodiscard]] const int GetNodeIndex(const std::string& nodeName) { return gltfModel_.GetNodeIndex(nodeName); }
    std::vector<GltfModel::Node>* GetNodes() { return gltfModel_.GetNodes(); }

private:
    GltfModel gltfModel_;
};

