#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- コンストラクタ -----
Object::Object(const std::string filename, const float& scaleFactor)
    : gltfModel_(filename),
    scaleFactor_(scaleFactor)
{
}

// ----- デストラクタ -----
Object::~Object()
{
}

void Object::Update(const float& elapsedTime)
{
    gltfModel_.UpdateAnimation(elapsedTime);
}

// ----- 描画 -----
void Object::Render(ID3D11PixelShader* psShader)
{
    gltfModel_.Render(scaleFactor_, psShader);
}

void Object::CastShadow()
{
    gltfModel_.CastShadow(scaleFactor_);
}

// ----- ImGui用 -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}
