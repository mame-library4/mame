#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- コンストラクタ -----
Object::Object(const std::string filename)
    : gltfModel_(Graphics::Instance().GetDevice(), filename)
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
void Object::Render(const float& scaleFactor)
{
    gltfModel_.Render(scaleFactor);
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
