#include "ResourceManager.h"

const std::shared_ptr<Model> ResourceManager::LoadModelResource(
    ID3D11Device*  device, 
    const char* const       fbxFileName,
    const bool              triangulate,
    const float             sampling_rate)
{
    // モデルを検索
    const ModelMap::iterator it = models.find(fbxFileName);
    if (it != models.end())
    {
        // リンク(寿命)が切れていないか確認
        if (it->second.expired() == false)
        {
            // 検索したモデルと同じモデルが見つかったらそれを返す
            return it->second.lock();
        }
    }

    // 見つからなかったら新規モデルリソースを作成・読み込み
    const std::shared_ptr<Model> model = {
        std::make_shared<Model>(
            device, 
            fbxFileName,
            triangulate, 
            sampling_rate)
    };

    // マップに登録
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
    // モデルを検索
    const ModelMap::iterator it = models.find(fbxFileName);
    if (it != models.end())
    {
        // リンク(寿命)が切れていないか確認
        if (it->second.expired() == false)
        {
            // 検索したモデルと同じモデルが見つかったらそれを返す
            return it->second.lock();
        }
    }

    // 見つからなかったら新規モデルリソースを作成・読み込み
    const std::shared_ptr<Model> model = {
        std::make_shared<Model>(
            device,
            fbxFileName,
            animationFileName,
            triangulate,
            sampling_rate)
    };

    // マップに登録
    models[fbxFileName] = model;

    return model;
}

