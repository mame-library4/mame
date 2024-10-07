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
    void UpdateCounterCollisions();

public:
    // ----- �X�e�[�W����p -----
    static DirectX::XMFLOAT3 stageCenter_;
    static constexpr float stageRadius_ = 29.5f; // �X�e�[�W�p�̔��a
    float stageRadius1_ = 17.5f; // �X�e�[�W�p�̔��a

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    std::unique_ptr<StageNormal> stage_;

    bool isDebugRenderer_ = false;
    //bool isDebugRenderer_ = true;

    std::unique_ptr<Stone> stone_;

    //std::unique_ptr<Effect> effect_;
};

