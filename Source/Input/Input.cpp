#include "Input.h"

Input* Input::instance = nullptr;

// コンストラクタ
Input::Input(HWND hWnd)
    :mouse(hWnd)
{
    instance = this;
}

// 更新処理
void Input::Update(const float& elapsedTime)
{
    gamePad.Update(elapsedTime);
    mouse.Update();
}
