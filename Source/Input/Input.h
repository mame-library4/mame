#pragma once

#include "GamePad.h"
#include "Mouse.h"

// インプット
class Input
{
public:
    Input(HWND hWnd);
    ~Input() {}

public:
    // インスタンス取得
    static Input& Instance() { return *instance_; }

    // 更新処理
    void Update(const float& elapsedTime);


    GamePad& GetGamePad() { return gamePad_; }
    Mouse& GetMouse() { return mouse_; }

    [[nodiscard]] const bool IsAnyInputDown();

private:
    static Input* instance_;
    GamePad gamePad_;
    Mouse mouse_;
};

