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

    virtual void Update(const float& elapsedTime);
    virtual void Render();
    virtual void DrawDebug();

public:
    void SetSpriteName(const std::string& name);

    [[nodiscard]] const UIManager::UIType GetType() const { return type_; }
    [[nodiscard]] const std::string GetName() const { return name_; }

    Sprite::Transform* GetTransform() { return sprite_->GetTransform(); }

    // ---------- �ړ��ړI�n ----------
    void SetMoveTarget(const DirectX::XMFLOAT2& moveValue);
    void AddMoveTargetPosition(const DirectX::XMFLOAT2& position);

private:
    const UIManager::UIType type_;
    std::unique_ptr<Sprite> sprite_;
    std::string             name_;

    // ----- UI�ړ����� -----
    DirectX::XMFLOAT2       oldPosition_        = {};
    DirectX::XMFLOAT2       moveTargetPosition_ = {};    // �ړ��ړI�n
    float                   moveTimer_          = 0.0f;
    float                   moveSpeed_          = 10.0f;
    bool                    isMovingToTarget_   = false;
};