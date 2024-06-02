#pragma once
#include "Stage.h"

class StageNormal : public Stage
{
public:
    StageNormal(std::string filename);
    ~StageNormal() override {}

    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
};

