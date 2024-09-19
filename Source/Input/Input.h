#pragma once

#include "GamePad.h"
#include "Mouse.h"

// �C���v�b�g
class Input
{
public:
    Input(HWND hWnd);
    ~Input() {}

public:
    // �C���X�^���X�擾
    static Input& Instance() { return *instance_; }

    // �X�V����
    void Update(const float& elapsedTime);


    GamePad& GetGamePad() { return gamePad_; }
    Mouse& GetMouse() { return mouse_; }

    [[nodiscard]] const bool IsAnyInputDown();

private:
    static Input* instance_;
    GamePad gamePad_;
    Mouse mouse_;
};

