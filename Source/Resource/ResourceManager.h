#pragma once

#include <map>
#include <string>
#include <memory>

#include "../Resource/Model.h"

class ResourceManager
{
private:
    ResourceManager()  {}
    ~ResourceManager() {}

public:
    static ResourceManager& Instance()
    {
        static ResourceManager instance;
        return instance;
    }

    // モデルリソース読み込み
    const std::shared_ptr<Model> LoadModelResource(
        ID3D11Device* device, 
        const char* const      fbxFileName,
        const bool             triangulate      = false, 
        const float            sampling_rate    = 0
    );

    // モデルリソース読み込み(アニメーション)
    const std::shared_ptr<Model> LoadModelResource(
        ID3D11Device*                       device, 
        const char* const                   fbxFileName,
        std::vector<std::string>            animationFileName, 
        const bool                          triangulate     = false, 
        const float                         sampling_rate   = 0
    );

private:
    using ModelMap = std::map<const char*, std::weak_ptr<Model>>;

private:
    ModelMap models = {};

};

