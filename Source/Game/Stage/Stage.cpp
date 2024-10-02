#include "Stage.h"

// ----- �R���X�g���N�^ -----
Stage::Stage(std::string filename)
    : model_(filename)
{
}

void Stage::Render(const float& scaleFacter, ID3D11PixelShader* psShader)
{
    model_.Render(scaleFacter, psShader);
}

void Stage::DrawDebug()
{
    GetTransform()->DrawDebug();
}

void Stage::CastShadow(const float& scaleFacter)
{
    model_.CastShadow(scaleFacter);
}
