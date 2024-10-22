#include "TitleState.h"
#include "Input.h"

// ----- IdleState -----
namespace TitleState
{
    // ----- ������ -----
    void IdleState::Initialize()
    {
        // �SUI��\��(���Z�b�g)
        owner_->GetUITitle()->SetIsDrawAllUI(false);

        // �^�C�g�����S�\��
        owner_->GetUITitle()->SetIsDrawTitleLogo(true);
    } 

    // ----- �X�V -----
    void IdleState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A)
        {
            owner_->ChangeState(TitleScene::STATE::Select);
            return;
        }
    }

    // ----- �I���� -----
    void IdleState::Finalize()
    {

    }

    // ----- ImGui�p -----
    void IdleState::DrawDebug()
    {

    }
}

// ----- SelectState -----
namespace TitleState
{
    // ----- ������ -----
    void SelectState::Initialize()
    {
        // �SUI��\��(���Z�b�g)
        owner_->GetUITitle()->SetIsDrawAllUI(false);
    }

    // ----- �X�V -----
    void SelectState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B)
        {
            owner_->ChangeState(TitleScene::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void SelectState::Finalize()
    {
    }

    // ----- ImGui�p -----
    void SelectState::DrawDebug()
    {
    }
}