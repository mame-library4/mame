#pragma once
#include "BaseScene.h"
#include "Object.h"
#include <memory>

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
    void UserInterfaceRender()              override;

    void Render() override;

    void DrawDebug()                        override; // ImGui�p

private:
    std::unique_ptr<Object> model_;
    DirectX::XMFLOAT3 target_ = {};
};

