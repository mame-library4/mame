#include "Input.h"

Input* Input::instance_ = nullptr;

// コンストラクタ
Input::Input(HWND hWnd)
    :mouse_(hWnd)
{
    instance_ = this;
}

// 更新処理
void Input::Update(const float& elapsedTime)
{
    gamePad_.Update(elapsedTime);
    mouse_.Update();
}

const bool Input::IsAnyInputDown()
{
    // ---------- Mouse ----------
    if (mouse_.GetButtonDown() && Mouse::BTN_RIGHT ||
        mouse_.GetButtonDown() && Mouse::BTN_LEFT ||
        mouse_.GetButtonDown() && Mouse::BTN_MIDDLE)
    {
        return true;
    }

    // ---------- GamePad ----------
    const float aRx = gamePad_.GetAxisRX();
    const float aRy = gamePad_.GetAxisRY();
    const float aLx = gamePad_.GetAxisLX();
    const float aLy = gamePad_.GetAxisLY();
    if (gamePad_.GetButtonDown() && GamePad::BTN_UP             ||
        gamePad_.GetButtonDown() && GamePad::BTN_RIGHT          ||
        gamePad_.GetButtonDown() && GamePad::BTN_DOWN           ||
        gamePad_.GetButtonDown() && GamePad::BTN_LEFT           ||
        gamePad_.GetButtonDown() && GamePad::BTN_A              ||
        gamePad_.GetButtonDown() && GamePad::BTN_B              ||
        gamePad_.GetButtonDown() && GamePad::BTN_X              ||
        gamePad_.GetButtonDown() && GamePad::BTN_Y              ||
        gamePad_.GetButtonDown() && GamePad::BTN_START          ||
        gamePad_.GetButtonDown() && GamePad::BTN_BACK           ||
        gamePad_.GetButtonDown() && GamePad::BTN_LEFT_THUMB     ||
        gamePad_.GetButtonDown() && GamePad::BTN_RIGHT_THUMB    ||
        gamePad_.GetButtonDown() && GamePad::BTN_LEFT_SHOULDER  ||
        gamePad_.GetButtonDown() && GamePad::BTN_RIGHT_SHOULDER ||
        gamePad_.GetButtonDown() && GamePad::BTN_LEFT_TRIGGER   ||
        gamePad_.GetButtonDown() && GamePad::BTN_RIGHT_TRIGGER  ||
        fabsf(aRx) != 0.0f || fabsf(aRy) != 0.0f ||
        fabsf(aLx) != 0.0f || fabsf(aLy) != 0.0f)
    {
        return true;
    }

    // ---------- KeyBoard -----
    if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState('B') & 0x8000 ||
        GetAsyncKeyState('C') & 0x8000 || GetAsyncKeyState('D') & 0x8000 ||
        GetAsyncKeyState('E') & 0x8000 || GetAsyncKeyState('F') & 0x8000 ||
        GetAsyncKeyState('G') & 0x8000 || GetAsyncKeyState('H') & 0x8000 ||
        GetAsyncKeyState('I') & 0x8000 || GetAsyncKeyState('J') & 0x8000 ||
        GetAsyncKeyState('K') & 0x8000 || GetAsyncKeyState('L') & 0x8000 ||
        GetAsyncKeyState('M') & 0x8000 || GetAsyncKeyState('N') & 0x8000 ||
        GetAsyncKeyState('O') & 0x8000 || GetAsyncKeyState('P') & 0x8000 ||
        GetAsyncKeyState('Q') & 0x8000 || GetAsyncKeyState('R') & 0x8000 ||
        GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState('T') & 0x8000 ||
        GetAsyncKeyState('U') & 0x8000 || GetAsyncKeyState('V') & 0x8000 ||
        GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState('X') & 0x8000 ||
        GetAsyncKeyState('Y') & 0x8000 || GetAsyncKeyState('Z') & 0x8000)
    {
        return true;
    }

    return false;
}