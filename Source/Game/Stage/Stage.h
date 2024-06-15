#pragma once
#include "GltfModelStaticMesh.h"

class Stage
{
public:
    Stage(std::string filename);
    virtual ~Stage() {}

    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render(const float& scaleFacter, ID3D11PixelShader* psShader = nullptr);
    virtual void DrawDebug();

    Transform* GetTransform() { return model_.GetTransform(); }

protected:
    GltfModelStaticMesh model_;
};

