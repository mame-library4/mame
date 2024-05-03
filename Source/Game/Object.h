#pragma once
#include "../Resource/GltfModel.h"

class Object
{
public:
    Object(const std::string filename);
    ~Object();

    void Update(const float& elapsedTime);
    void Render(const float& scaleFactor);
    void DrawDebug();

    void PlayAnimation(const int& index, const bool& loop, const float& speed) { gltfModel_.PlayAnimation(index, loop, speed); }
    void PlayBlendAnimation(const int& index1, const int& index2, const bool& loop, const float& speed) { gltfModel_.PlayBlendAnimation(index1, index2, loop, speed); }
    const bool IsPlayAnimation() { return gltfModel_.IsPlayAnimation(); }

public:
    // ---------- Transform ----------
    Transform* GetTransform() { return gltfModel_.GetTransform(); }

    // ---------- JointPosition ----------
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const size_t& nodeIndex, const float& scaleFactor) { return gltfModel_.GetJointPosition(nodeIndex, scaleFactor); }
    [[nodiscard]] const DirectX::XMFLOAT3 GetJointPosition(const std::string& nodeName, const float& scaleFactor) { return gltfModel_.GetJointPosition(nodeName, scaleFactor); }

    // ---------- weight ----------
    [[nodiscard]] const float GetWeight() { return gltfModel_.GetWeight(); }
    void SetWeight(const float& weight) { gltfModel_.SetWeight(weight); }
    void AddWeight(const float& weight);

private:
    GltfModel gltfModel_;
};

