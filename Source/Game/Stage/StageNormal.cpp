#include "StageNormal.h"

// ----- コンストラクタ -----
StageNormal::StageNormal(std::string filename)
    : Stage(filename)
{
}

// ----- 更新 -----
void StageNormal::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void StageNormal::Render()
{
    Object::Render(0.01f);
}

// ----- ImGui -----
void StageNormal::DrawDebug()
{
    Object::DrawDebug();
}
