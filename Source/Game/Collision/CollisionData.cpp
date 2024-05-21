#include "CollisionData.h"
#include "../../../External/imgui/imgui.h"
#include "../../../External/imgui/imgui_internal.h"

// ----- ImGui�p -----
void CollisionCylinderData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::DragFloat("height", &height_, 0.01f);
        ImGui::TreePop();
    }
}

// ----- ImGui�p -----
void CollisionSphereData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::TreePop();
    }
}

// ----- ImGui�p -----
void DamageDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

void DamageDetectionData::Update(const float& elapsedTime)
{
    if (isHit_)
    {
        hitTimer_ -= elapsedTime;

        if (hitTimer_ < 0.0f)
        {
            isHit_ = false;
        }
        SetColor({ 0,1,0,1 });
    }
    else
    {
        SetColor({ 0,0,1,1 });
    }
}

// ----- ImGui�p -----
void AttackDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

// ----- ImGui�p -----
void CollisionDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}
