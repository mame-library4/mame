#pragma once
#include "StateMachine/State.h"
#include "TitleScene.h"

namespace TitleState
{
    class IdleState : public State<TitleScene>
    {
    public:
        IdleState(TitleScene* titleScene) : State(titleScene, "IdleState") {}
        ~IdleState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        [[nodiscard]] const bool CheckInput(); // 入力をチェックする

    private:
        float lerpTimer_        = 0.0f;
        float lerpSpeed_        = 5.0f;
        float startRotationY_   = 0.0f;
        const float targetRotationY_ = DirectX::XMConvertToRadians(180.0f);

        // ----- Transition -----
        bool isAllowedToTransition_ = false;
    };

    class SelectState : public State<TitleScene>
    {
    public:
        SelectState(TitleScene* titleScene) : State(titleScene, "SelectState") {}
        ~SelectState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        enum class STATE { GameStart, Options, Quit };

    private:
        void UpdateCamera(const float& elapsedTime); // カメラ更新
        void CheckInput(const float& elapsedTime);   // 入力チェック
        void ChangeState(const STATE& upState, const STATE& downState); // ステート変更

    private:
        float lerpTimer_        = 0.0f;
        float lerpSpeed_        = 5.0f;
        float startRotationY_   = 0.0f;
        const float targetRotationY_ = DirectX::XMConvertToRadians(130.0f);

        // ----- Select -----
        STATE   currentState_           = STATE::GameStart; // 現在のステート
        float   breakTime_              = 0.1f;             // 移動間のディレイ
        float   breakTimer_             = 0.0f;             // 移動間のディレイタイマー
        float   stickThreshold_         = 0.8f;             // 入力判定閾値

        // ----- Transition -----
        bool isAllowedToTransition_ = false;
    };

    class GameStartState : public State<TitleScene>
    {
    public:
        GameStartState(TitleScene* titleScene) : State(titleScene, "GameStartState") {}
        ~GameStartState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    class OptionState : public State<TitleScene>
    {
    public:
        OptionState(TitleScene* titleScene) : State(titleScene, "OptionState") {}
        ~OptionState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    class QuitState : public State<TitleScene>
    {
    public:
        QuitState(TitleScene* titleScene) : State(titleScene, "QuitState") {}
        ~QuitState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        bool isGameEnd_ = false;
    };
}

