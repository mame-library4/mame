#pragma once

#include <DirectXMath.h>
#include <stdlib.h>

// ƒ|ƒCƒ“ƒ^Á‹ŠÖ”
template <class T>
inline void SafeDeletePtr(T*& p)
{
    if (p == nullptr) return;

    delete (p);
    p = nullptr;
}