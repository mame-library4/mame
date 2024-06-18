#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Stage/StageNormal.h"

#include "../Graphics/Particle.h"

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
    void UserInterfaceRender()              override;

    void Render() override;

    void DrawDebug()                        override; // ImGui�p

public:
    // ----- �X�e�[�W����p -----
    static DirectX::XMFLOAT3 stageCenter_;
    //static constexpr float stageRadius_ = 17.5f; // �X�e�[�W�p�̔��a
    static constexpr float stageRadius_ = 26.0f; // �X�e�[�W�p�̔��a
    float stageRadius1_ = 17.5f; // �X�e�[�W�p�̔��a

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    static const int stageMax = 3;
    std::unique_ptr<StageNormal> stageNormal_[stageMax];

    bool isDebugRenderer_ = true;

    std::unique_ptr<Particles> particles_;
};

