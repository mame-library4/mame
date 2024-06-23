#pragma once
#include "BaseScene.h"
#include "Character/Character.h"
#include <memory>

class DemoChara : public Character
{
public:
    DemoChara();
    ~DemoChara() {};

    void Update(const float& elapsedTime)override;
    void Render(ID3D11PixelShader* psShader) override;
    void DrawDebug()override;
};

class DemoScene : public BaseScene
{
public:
    DemoScene() {}
    ~DemoScene() {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理

    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    void UserInterfaceRender()              override;

    void Render() override;

    void DrawDebug()                        override; // ImGui用

private:
    std::unique_ptr<DemoChara> model_;
    DirectX::XMFLOAT3 target_ = {};
};

