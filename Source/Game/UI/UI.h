#pragma once
#include "Sprite.h"
#include <memory>

class UI
{
public:
    UI(const wchar_t* filename = nullptr);
    virtual ~UI() {}

    virtual void Update(const float& elapsedTime) {}
    virtual void Render();
    virtual void DrawDebug();

private:
    std::unique_ptr<Sprite> sprite_;
};

