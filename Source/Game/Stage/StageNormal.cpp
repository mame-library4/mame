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
void StageNormal::Render()
{
    Object::Render(0.01f);
}

// ----- ImGui -----
void StageNormal::DrawDebug()
{
    Object::DrawDebug();
}
