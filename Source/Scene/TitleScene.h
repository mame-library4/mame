#pragma once
#include "BaseScene.h"
#include "Object.h"
#include "UI/UITitle.h"
#include "StateMachine/StateMachine.h"

class TitleScene : public BaseScene
{
public:
    enum class STATE
    {
        Idle,
        Select,
    };

public:// ��{�I�Ȋ֐�
    TitleScene() {}
    ~TitleScene() override {}

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    
    void Render() override {};

    void DrawDebug()                        override; // ImGui�p

public:
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<TitleScene>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }
    [[nodiscard]] const STATE GetOldState() const { return oldState_; }

    UITitle* GetUITitle() { return uiTitle_; }

private:
    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<TitleScene>>> stateMachine_;
    STATE currentState_ = STATE::Idle;
    STATE oldState_     = STATE::Idle;

    UITitle* uiTitle_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];
    std::unique_ptr<Object> stageObject_;
    std::unique_ptr<Object> dragonObject_;
    std::unique_ptr<Object> playerObject_;
};
