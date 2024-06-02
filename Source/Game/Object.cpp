#include "Object.h"
#include "../Graphics/Graphics.h"
#include <algorithm>

// ----- �R���X�g���N�^ -----
Object::Object(const std::string filename)
    : gltfModel_(filename)
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
void Object::Render(const float& scaleFactor, ID3D11PixelShader* psShader)
{
    gltfModel_.Render(scaleFactor, psShader);
}

// ----- ImGui�p -----
void Object::DrawDebug()
{
    gltfModel_.DrawDebug();
}

// ----- weight�l���Z -----
void Object::AddWeight(const float& weight)
{
    // weight��0~1�̊ԂɎ��߂�
    float w = GetWeight();
    w += weight;
    w = std::clamp(w, 0.0f, 1.0f);
    SetWeight(w);
}
