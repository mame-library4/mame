#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Stage/StageNormal.h"
#include "../Game/Character/Enemy/EnemyTamamo.h"

class GameScene : public BaseScene
{
public:
    GameScene() {}
    ~GameScene() override {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理
    
    void ShadowRender(ID3D11DeviceContext* deviceContext)           override;
    void DeferredRender(ID3D11DeviceContext* deviceContext)         override;
    void ForwardRender(ID3D11DeviceContext* deviceContext)          override;
    void UserInterfaceRender(ID3D11DeviceContext* deviceContext)    override;

    void DrawDebug()                        override; // ImGui用

public:
    // ----- ステージ判定用 -----
    static DirectX::XMFLOAT3 stageCenter_;
    static constexpr float stageRadius_ = 17.5f; // ステージ用の半径

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    static const int stageMax = 3;
    std::unique_ptr<StageNormal> stageNormal_[stageMax];

    bool isDebugRenderer_ = true;
};

