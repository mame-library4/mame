#include "Input.h"

Input* Input::instance = nullptr;

// �R���X�g���N�^
Input::Input(HWND hWnd)
    :mouse(hWnd)
{
    instance = this;
}

// �X�V����
void Input::Update(const float& elapsedTime)
{
    gamePad.Update(elapsedTime);
    mouse.Update();
}
