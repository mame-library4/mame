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
    static Input& Instance() { return *instance; }

    // �X�V����
    void Update(const float& elapsedTime);

    // �Q�[���p�b�h�擾
    GamePad& GetGamePad() { return gamePad; }

    // �}�E�X�擾
    Mouse& GetMouse() { return mouse; }

private:
    static Input* instance;
    GamePad gamePad;
    Mouse mouse;
};

