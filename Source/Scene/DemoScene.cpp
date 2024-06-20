#include "DemoScene.h"
#include "Graphics.h"
#include "Camera.h"

void DemoScene::CreateResource()
{
    model_ = std::make_unique<Object>("./Resources/Model/Character/aaa.glb");
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


    
    model_->Update(elapsedTime);
}

void DemoScene::ShadowRender()
{
}

void DemoScene::DeferredRender()
{
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();
    model_->Render(0.01f, gBufferPixelShader);
}

void DemoScene::ForwardRender()
{
    model_->Render(0.01f);
}

void DemoScene::UserInterfaceRender()
{
}

void DemoScene::Render()
{
}

void DemoScene::DrawDebug()
{
    if (ImGui::Button("Init"))
    {
        model_->GetTransform()->SetPosition({});
    }
    ImGui::DragFloat3("target", &target_.x);
}
