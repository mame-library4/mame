#include "IceArrow.h"
#include "ProjectileManager.h"
#include "Graphics.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
IceArrow::IceArrow()
    : Projectile("./Resources/Model/Effect/Arrow.gltf", 0.01f, "IceArrow", static_cast<int>(ProjectileManager::DrawType::Normal))
{
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/IceArrowPS.cso", iceArrowPS_.GetAddressOf());
}

void IceArrow::Initialize()
{    
    Object::SetScrollDirection({ 0.5f, -1.0f });
    Object::SetEmissiveColor({ 0, 1, 1 });
    

}

void IceArrow::Finalize()
{
}

void IceArrow::Update(const float& elapsedTime)
{
    Object::AddScrollTimer(elapsedTime);

    const float toRadian = 0.01745f;
    const float toMetric = 0.01f;
    DirectX::XMMATRIX boneTransform = PlayerManager::Instance().GetPlayer()->GetJointGlobalTransform("hand_l");

    DirectX::XMMATRIX socketTransform = DirectX::XMMatrixScaling(iceArrowScale_.x, iceArrowScale_.y, iceArrowScale_.z)
            * DirectX::XMMatrixRotationX(-iceArrowRotation_.x * toRadian)
            * DirectX::XMMatrixRotationY(-iceArrowRotation_.y * toRadian)
            * DirectX::XMMatrixRotationZ(iceArrowRotation_.z * toRadian)
            * DirectX::XMMatrixTranslation(iceArrowLocation_.x * toMetric, iceArrowLocation_.y * toMetric, iceArrowLocation_.z * toMetric);


    DirectX::XMMATRIX dxUE5 = DirectX::XMMatrixSet(-1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Y-Up Z-Forward(DX) -> LHS Z-Up Y-Forward(UE5) 
    DirectX::XMMATRIX UE5Gltf = DirectX::XMMatrixSet(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1); // LHS Z-Up Y-Forward(UE5) -> RHS Y-Up Z-Forward(glTF) 
    DirectX::XMStoreFloat4x4(&iceArrowWorld_, dxUE5 * socketTransform * UE5Gltf * boneTransform * PlayerManager::Instance().GetTransform()->CalcWorldMatrix(0.01f));
}

void IceArrow::Render(ID3D11PixelShader* psShader)
{
    if(isDrawActive_) Object::Render(iceArrowWorld_, iceArrowPS_.Get());
}

void IceArrow::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("Location", &iceArrowLocation_.x);
        ImGui::DragFloat3("Rotation", &iceArrowRotation_.x);
        ImGui::DragFloat3("Scale", &iceArrowScale_.x);

        //ImGui::DragFloat("LifeTimer", &lifeTimer_);

        Projectile::DrawDebug();

        //ImGui::DragFloat3("MoveDirection", &moveDirection_.x, 0.01f);
        //ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
        //ImGui::Checkbox("IsEffectGeneratable", &isEffectGeneratable_);

        ImGui::TreePop();
    }
}

void IceArrow::OnHit()
{
}
