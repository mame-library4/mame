#include "Input.h"

Input* Input::instance_ = nullptr;

// �R���X�g���N�^
Input::Input(HWND hWnd)
    :mouse_(hWnd)
{
    instance_ = this;
}

// �X�V����
void Input::Update(const float& elapsedTime)
{
    gamePad_.Update(elapsedTime);
    mouse_.Update();
}