#pragma once
#include "UIManager.h"
#include "Sprite.h"
#include <memory>
#include <string>

class UI
{
public:
    UI(const UIManager::UIType& type, const wchar_t* filename = nullptr, const std::string& name = "UI");
    virtual ~UI() {}

    virtual void Update(const float& elapsedTime) {}
    virtual void Render();
    virtual void DrawDebug();

public:
    void SetSpriteName(const std::string& name);

    [[nodiscard]] const UIManager::UIType GetType() const { return type_; }
    [[nodiscard]] const std::string GetName() const { return name_; }

    Sprite::Transform* GetTransform() { return sprite_->GetTransform(); }

private:
    const UIManager::UIType type_;

    std::unique_ptr<Sprite> sprite_;

    std::string name_;
};