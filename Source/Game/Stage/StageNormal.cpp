#include "StageNormal.h"

// ----- �R���X�g���N�^ -----
StageNormal::StageNormal(std::string filename)
    : Stage(filename)
{
}

// ----- �X�V -----
void StageNormal::Update(const float& elapsedTime)
{
}

// ----- �`�� -----
void StageNormal::Render(const float& scaleFacter, ID3D11PixelShader* psShader)
{
    Stage::Render(0.01f, psShader);
}

// ----- ImGui -----
void StageNormal::DrawDebug()
{
    Stage::DrawDebug();
}
