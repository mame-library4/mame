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
void StageNormal::Render(const float& scaleFacter, ID3D11PixelShader* psShader)
{
    Stage::Render(scaleFacter, psShader);
}

// ----- ImGui -----
void StageNormal::DrawDebug()
{
    Stage::DrawDebug();
}
