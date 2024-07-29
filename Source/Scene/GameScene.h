#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Stage/StageNormal.h"

#include "../Graphics/Particle.h"

#include "Stone/Stone.h"

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

    void UpdateCollisions(const float& elapsedTime);

private:
    void UpdatePlayerAttackCollisions(const float& elapsedTime);
    void UpdatePlayerDamageCollisions(const float& elapsedTime);
    void UpdateCounterCollisions();

public:
    // ----- �X�e�[�W����p -----
    static DirectX::XMFLOAT3 stageCenter_;
    //static constexpr float stageRadius_ = 17.5f; // �X�e�[�W�p�̔��a
    static constexpr float stageRadius_ = 26.0f; // �X�e�[�W�p�̔��a
    float stageRadius1_ = 17.5f; // �X�e�[�W�p�̔��a

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    std::unique_ptr<StageNormal> stage_;

    bool isDebugRenderer_ = true;

    std::unique_ptr<Particles> particles_;

    std::unique_ptr<Stone> stone_;
};

