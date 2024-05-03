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

    // ���f�����\�[�X�ǂݍ���
    const std::shared_ptr<Model> LoadModelResource(
        ID3D11Device* device, 
        const char* const      fbxFileName,
        const bool             triangulate      = false, 
        const float            sampling_rate    = 0
    );

    // ���f�����\�[�X�ǂݍ���(�A�j���[�V����)
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

