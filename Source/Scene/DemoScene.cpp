#include "DemoScene.h"
#include "Graphics.h"
#include "Camera.h"

#include "Input.h"


#pragma region DEMOキャラ
DemoChara::DemoChara()
    //: Character("./Resources/Model/Character/Zombie_Punching_Anim_mixamo_com1.glb", 1.0f)
    : Character("./Resources/Model/Character/Zombie_Punching_Anim_mixamo_com1.gltf", 1.0f)
    //: Character("./Resources/Model/Character/SwordGirl.gltf", 1.0f)
    //: Character("./Resources/Model/Character/test/Dragon.gltf", 1.0f)
{
}

void DemoChara::Update(const float& elapsedTime)
{
    Object::Update(elapsedTime);
}

void DemoChara::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

void DemoChara::DrawDebug()
{
    Object::DrawDebug();
}
#pragma endregion DEMOキャラ

void DemoScene::CreateResource()
{
    demoChara_ = std::make_unique<DemoChara>();
}

void DemoScene::Initialize()
{
    demoChara_->PlayAnimation(0, true, 1.0f);
}

void DemoScene::Finalize()
{
}

void DemoScene::Update(const float& elapsedTime)
{
    Camera::Instance().SetTarget({});

    demoChara_->Update(elapsedTime);
}

void DemoScene::ShadowRender()
{
}

void DemoScene::DeferredRender()
{
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();
    
    demoChara_->Render(gBufferPixelShader);
}

void DemoScene::ForwardRender()
{
 
}


void DemoScene::Render()
{
}

void DemoScene::DrawDebug()
{
    demoChara_->DrawDebug();
}


