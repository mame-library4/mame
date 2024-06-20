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
    void PlayBlendAnimation(const int& index, const bool& loop, const float& speed) { gltfModel_.PlayBlendAnimation(index, loop, speed); }
    [[nodiscard]] const bool IsPlayAnimation() { return gltfModel_.IsPlayAnimation(); }
    [[nodiscard]] const int GetAnimationIndex() const { return gltfModel_.GetAnimationIndex(); }
    [[nodiscard]] const float GetAnimationSeconds() const { return gltfModel_.GetAnimationSeconds(); }
    [[nodiscard]] const float GetAnimationSpeed() const { return gltfModel_.GetAnimationSpeed(); }
    [[nodiscard]] const float GetTransitionTime() const { return gltfModel_.GetTransitionTime(); }
    void SetAnimationSpeed(const float& speed) { gltfModel_.SetAnimationSpeed(speed); }
    void SetTransitionTime(const float& time) { gltfModel_.SetTransitionTime(time); }

public:
    // ---------- Transform ----------
    Transform* GetTransform() { return gltfModel_.GetTransform(); }

    // ---------- JointPosition ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeIndex, scaleFactor, offsetPosition); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeName, scaleFactor, offsetPosition); }
    
    DirectX::XMMATRIX GetJointGlobalTransform(const size_t& nodeIndex) { return gltfModel_.GetJointGlobalTransform(nodeIndex); }
    DirectX::XMMATRIX GetJointGlobalTransform(const std::string& nodeName) { return gltfModel_.GetJointGlobalTransform(nodeName); }
    DirectX::XMMATRIX GetJointWorldTransform(const std::string& nodeName, const float& scaleFacter) { return gltfModel_.GetJointWorldTransform(nodeName, scaleFacter); }


    // ---------- Node ----------
    [[nodiscard]] const int GetNodeIndex(const std::string& nodeName) { return gltfModel_.GetNodeIndex(nodeName); }
    std::vector<GltfModel::Node>* GetNodes() { return gltfModel_.GetNodes(); }



private:
    GltfModel gltfModel_;

    // ---------- RootMotion ----------
    std::vector<GltfModel::Node>    animatedNodes_;
    std::vector<GltfModel::Node>    zeroAnimatedNodes_;
    DirectX::XMFLOAT3               previousPosition_    = {};
    float rootMotionTimer_ = 0;
};

