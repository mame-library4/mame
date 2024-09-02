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
