#include "DemoScene.h"
#include "Graphics.h"
#include "Camera.h"

DemoChara::DemoChara()
    : Character("./Resources/Model/Character/Player/Orc.gltf", 1.0f)
{
}

void DemoChara::Update(const float& elapsedTime)
{
    Object::Update(elapsedTime);

    UpdateRootMotion();
}

void DemoChara::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

void DemoChara::DrawDebug()
{
}

void DemoScene::CreateResource()
{
    //model_ = std::make_unique<Object>("./Resources/Model/Character/SKM_Manny.glb");
    //model_ = std::make_unique<Object>("./Resources/Model/Character/unitychan.glb");
    //model_ = std::make_unique<Object>("./Resources/Model/Character/Salute.glb", 1.0f);
    model_ = std::make_unique<DemoChara>();
}

void DemoScene::Initialize()
{
    model_->PlayAnimation(20, true, 0.2f);

    
}

void DemoScene::Finalize()
{
}

void DemoScene::Update(const float& elapsedTime)
{
    Camera::Instance().SetTarget(target_);

    model_->Update(elapsedTime);
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
    model_->Render(nullptr);
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


