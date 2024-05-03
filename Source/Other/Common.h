#pragma once

#include <DirectXMath.h>
#include <stdlib.h>

// �|�C���^�����֐�
template <class T>
inline void SafeDeletePtr(T*& p)
{
    if (p == nullptr) return;

    delete (p);
    p = nullptr;
}