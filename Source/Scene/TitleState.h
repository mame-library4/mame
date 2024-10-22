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
    };
}

