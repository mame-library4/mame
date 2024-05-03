#include "ResourceManager.h"

const std::shared_ptr<Model> ResourceManager::LoadModelResource(
    ID3D11Device*  device, 
    const char* const       fbxFileName,
    const bool              triangulate,
    const float             sampling_rate)
{
    // ���f��������
    const ModelMap::iterator it = models.find(fbxFileName);
    if (it != models.end())
    {
        // �����N(����)���؂�Ă��Ȃ����m�F
        if (it->second.expired() == false)
        {
            // �����������f���Ɠ������f�������������炻���Ԃ�
            return it->second.lock();
        }
    }

    // ������Ȃ�������V�K���f�����\�[�X���쐬�E�ǂݍ���
    const std::shared_ptr<Model> model = {
        std::make_shared<Model>(
            device, 
            fbxFileName,
            triangulate, 
            sampling_rate)
    };

    // �}�b�v�ɓo�^
    models[fbxFileName] = model;

    return model;
}

const std::shared_ptr<Model> ResourceManager::LoadModelResource(
    ID3D11Device*             device, 
    const char* const                  fbxFileName,
    std::vector<std::string> animationFileName, 
    const bool                         triangulate, 
    const float                        sampling_rate)
{
    // ���f��������
    const ModelMap::iterator it = models.find(fbxFileName);
    if (it != models.end())
    {
        // �����N(����)���؂�Ă��Ȃ����m�F
        if (it->second.expired() == false)
        {
            // �����������f���Ɠ������f�������������炻���Ԃ�
            return it->second.lock();
        }
    }

    // ������Ȃ�������V�K���f�����\�[�X���쐬�E�ǂݍ���
    const std::shared_ptr<Model> model = {
        std::make_shared<Model>(
            device,
            fbxFileName,
            animationFileName,
            triangulate,
            sampling_rate)
    };

    // �}�b�v�ɓo�^
    models[fbxFileName] = model;

    return model;
}

