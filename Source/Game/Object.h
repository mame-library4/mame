#pragma once
#include "GltfModel.h"
#include "Transform.h"

class Object
{
public:
    Object(const std::string filename);
    ~Object();

    void Update(const float& elapsedTime);
    void Render(const float& scaleFactor, ID3D11PixelShader* psShader = nullptr);
    void DrawDebug();

    // ---------- Animation ----------
    void PlayAnimation(const int& animationIndex, const bool& loop);
    void PlayBlendAnimation(const int& blendAnimationIndex, const bool& loop);
    void UpdateAnimation(const float& elapsedTime);
    [[nodiscard]] const bool IsPlayAnimation() const { return !animationEndFlag_; }
    const float GetCurrentAnimationSeconds() const { return currentAnimationSeconds_; }

public:
    // ---------- Transform ----------
    Transform* GetTransform() { return &transform_; }

    // ---------- JointPosition ----------
    //[[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeIndex, scaleFactor, offsetPosition); }
    //[[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor, const DirectX::XMFLOAT3& offsetPosition = {}) { return gltfModel_.GetJointPosition(nodeName, scaleFactor, offsetPosition); }
    
    //DirectX::XMMATRIX GetJointGlobalTransform(const size_t& nodeIndex) { return gltfModel_.GetJointGlobalTransform(nodeIndex); }
    //DirectX::XMMATRIX GetJointGlobalTransform(const std::string& nodeName) { return gltfModel_.GetJointGlobalTransform(nodeName); }
    //DirectX::XMMATRIX GetJointWorldTransform(const std::string& nodeName, const float& scaleFacter) { return gltfModel_.GetJointWorldTransform(nodeName, scaleFacter); }

    // ---------- Node ----------
    [[nodiscard]] const int GetNodeIndex(const std::string& nodeName) { return gltfModel_.GetNodeIndex(nodeName); }
    std::vector<GltfModel::Node>* GetNodes() { return gltfModel_.GetNodes(); }

private:
    GltfModel gltfModel_;
    Transform transform_;

    // ---------- Animation ----------
    std::vector<GltfModel::Node>    animatedNodes_[2];
    std::vector<GltfModel::Node>    blendAnimatedNodes_;
    int                             currentAnimationIndex_      = -1;
    float                           currentAnimationSeconds_    = 0.0f;
    float                           animationFactor_            = 0.0f;
    float                           transitionTime_             = 1.0f;
    bool                            isBlendAnimation_           = false;
    bool                            animationLoopFlag_          = false;
    bool                            animationEndFlag_           = false;
};

