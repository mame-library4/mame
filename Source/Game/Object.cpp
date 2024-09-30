#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- �R���X�g���N�^ -----
Object::Object(const std::string filename, const float& scaleFactor)
    : gltfModel_(filename),
    scaleFactor_(scaleFactor)
{
}

// ----- �f�X�g���N�^ -----
Object::~Object()
{
}

void Object::Update(const float& elapsedTime)
{
    gltfModel_.UpdateAnimation(elapsedTime);

    DirectX::XMFLOAT3 rotation = GetTransform()->GetRotation();
    //if (rotation.x > DirectX::XM_2PI)  rotation.x -= DirectX::XM_2PI;
    //if (rotation.z > DirectX::XM_2PI)  rotation.z -= DirectX::XM_2PI;
    if (rotation.y > DirectX::XM_2PI)   rotation.y -= DirectX::XM_2PI;
    if (rotation.y < 0.0f)              rotation.y += DirectX::XM_2PI;
    
    //if (rotation.x < -DirectX::XM_2PI) rotation.x += DirectX::XM_2PI;
    //if (rotation.y < -DirectX::XM_2PI) rotation.y += DirectX::XM_2PI;
    //if (rotation.z < -DirectX::XM_2PI) rotation.z += DirectX::XM_2PI;

    GetTransform()->SetRotation(rotation);
}

// ----- �`�� -----
void Object::Render(ID3D11PixelShader* psShader)
{
    gltfModel_.Render(scaleFactor_, psShader);
}

void Object::CastShadow()
{
    gltfModel_.CastShadow(scaleFactor_);
}

// ----- ImGui�p -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}
