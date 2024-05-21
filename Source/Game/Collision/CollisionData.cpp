#include "CollisionData.h"
#include "../../../External/imgui/imgui.h"
#include "../../../External/imgui/imgui_internal.h"

// ----- ImGui用 -----
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

// ----- ImGui用 -----
void CollisionSphereData::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat3("offsetPosition", &offsetPosition_.x);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::TreePop();
    }
}

// ----- ImGui用 -----
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

// ----- ImGui用 -----
void AttackDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}

// ----- ImGui用 -----
void CollisionDetectionData::DrawDebug()
{
    collisionSphereData_.DrawDebug();
}
