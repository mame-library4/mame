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

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����
    
    void ShadowRender(ID3D11DeviceContext* deviceContext)           override;
    void DeferredRender(ID3D11DeviceContext* deviceContext)         override;
    void ForwardRender(ID3D11DeviceContext* deviceContext)          override;
    void UserInterfaceRender(ID3D11DeviceContext* deviceContext)    override;

    void DrawDebug()                        override; // ImGui�p

public:
    // ----- �X�e�[�W����p -----
    static DirectX::XMFLOAT3 stageCenter_;
    static constexpr float stageRadius_ = 17.5f; // �X�e�[�W�p�̔��a

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];

    static const int stageMax = 3;
    std::unique_ptr<StageNormal> stageNormal_[stageMax];

    bool isDebugRenderer_ = true;
};

