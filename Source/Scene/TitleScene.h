#pragma once
#include "BaseScene.h"
#include <memory>
#include "Sprite.h"

class TitleScene : public BaseScene
{
public:// 基本的な関数
    TitleScene() {}
    ~TitleScene() override {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    
    void Render() override {};

    void DrawDebug()                        override; // ImGui用

private:
    std::unique_ptr<Sprite> sprite_;


};
