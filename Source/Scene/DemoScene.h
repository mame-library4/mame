#pragma once
#include "BaseScene.h"
#include "Character/Character.h"
#include <memory>
#include "Sprite.h"

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

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����

    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;

    void Render() override;

    void DrawDebug()                        override; // ImGui�p

private:
    std::unique_ptr<DemoChara> demoChara_;
};

