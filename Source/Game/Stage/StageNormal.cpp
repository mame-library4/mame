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
void StageNormal::Render(ID3D11PixelShader* psShader)
{
    Object::Render(0.01f, psShader);
}

// ----- ImGui -----
void StageNormal::DrawDebug()
{
    Object::DrawDebug();
}
