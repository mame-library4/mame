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

public:// 基本的な関数
    TitleScene() {}
    ~TitleScene() override {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    
    void Render() override {};

    void DrawDebug()                        override; // ImGui用

public:
    // ---------- ステートマシン --------------------
    StateMachine<State<TitleScene>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }
    [[nodiscard]] const STATE GetOldState() const { return oldState_; }

    UITitle* GetUITitle() { return uiTitle_; }

private:
    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<TitleScene>>> stateMachine_;
    STATE currentState_ = STATE::Idle;
    STATE oldState_     = STATE::Idle;

    UITitle* uiTitle_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];
    std::unique_ptr<Object> stageObject_;
    std::unique_ptr<Object> dragonObject_;
    std::unique_ptr<Object> playerObject_;
};
