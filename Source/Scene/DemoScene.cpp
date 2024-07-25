#include "DemoScene.h"
#include "Graphics.h"
#include "Camera.h"

#include "Input.h"


#pragma region DEMOƒLƒƒƒ‰
DemoChara::DemoChara()
    : Character("./Resources/Model/Character/test/Dragon.gltf", 1.0f)
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
}
#pragma endregion DEMOƒLƒƒƒ‰

void DemoScene::CreateResource()
{
}

void DemoScene::Initialize()
{
 
}

void DemoScene::Finalize()
{
}

void DemoScene::Update(const float& elapsedTime)
{
    Camera::Instance().SetTarget({});
}

void DemoScene::ShadowRender()
{
}

void DemoScene::DeferredRender()
{
    ID3D11PixelShader* gBufferPixelShader = Graphics::Instance().GetShader()->GetGBufferPixelShader();
    
 
}

void DemoScene::ForwardRender()
{
 
}

void DemoScene::UserInterfaceRender()
{
 
}

void DemoScene::Render()
{
}

void DemoScene::DrawDebug()
{
 
}


