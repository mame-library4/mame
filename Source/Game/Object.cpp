#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- コンストラクタ -----
Object::Object(const std::string filename)
    : gltfModel_(filename)
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
void Object::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
    gltfModel_.Render(scaleFactor, psShader);
}

// ----- ImGui用 -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}

// ----- weight値加算 -----
void Object::AddWeight(const float& weight)
{
    // weightは0~1の間に収める
    float w = GetWeight();
    w += weight;
    w = std::clamp(w, 0.0f, 1.0f);
    SetWeight(w);
}
