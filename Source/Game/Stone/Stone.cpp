#include "Stone.h"

// ----- �R���X�g���N�^ -----
Stone::Stone()
    : Object("./Resources/Model/Stone/stone.gltf", 1.0f)
{
}

Stone::~Stone()
{
}

void Stone::Update(const float& elapsedTime)
{
}

// ----- �`�� -----
void Stone::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

void Stone::DrawDebug()
{
    if (ImGui::BeginMenu("Stone"))
    {
        Object::DrawDebug();
        ImGui::EndMenu();
    }
}
