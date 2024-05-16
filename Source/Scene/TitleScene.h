#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Character/Player/Player.h"
#include "../Game/Stage/StageNormal.h"
#include "../Game/Character/Enemy/EnemyTamamo.h"

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
    void UserInterfaceRender()              override;

    void DrawDebug()                        override; // ImGui用

};
