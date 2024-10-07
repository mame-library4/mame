#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Stage/StageNormal.h"

#include "Stone/Stone.h"

#include "Effect/Effect.h"

class GameScene : public BaseScene
{
public:
    GameScene() {}
    ~GameScene() override {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;

    void Render() override;

    void DrawDebug()                        override; // ImGui用

private:
    void UpdateCounterCollisions();

public:
    // ----- ステージ判定用 -----
    static DirectX::XMFLOAT3 stageCenter_;
    static constexpr float stageRadius_ = 29.5f; // ステージ用の半径
    float stageRadius1_ = 17.5f; // ステージ用の半径

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    std::unique_ptr<StageNormal> stage_;

    bool isDebugRenderer_ = false;
    //bool isDebugRenderer_ = true;

    std::unique_ptr<Stone> stone_;

    //std::unique_ptr<Effect> effect_;
};

