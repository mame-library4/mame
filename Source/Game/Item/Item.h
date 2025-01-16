#pragma once
#include "Object.h"
#include <string>

class Item : public Object
{
public:
    Item(const std::string filename, const float& scaleFactor, const std::string& name = "", const int& itemType = 0);
    virtual ~Item() {}

    virtual void Initialize() = 0;
    virtual void Finalize() = 0;
    virtual void Update(const float& elapsedTime) = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug();

    virtual void OnHit() = 0;

public:
    [[nodiscard]] const DirectX::XMFLOAT3 GetOffsetPosition() const { return offsetPosition_; }
    [[nodiscard]] const float GetDamageRadius() const { return damageRadius_; }
    [[nodiscard]] const float GetAttackRadius() const { return attackRadius_; }
    [[nodiscard]] const float GetAttackPower() const { return attackPower_; }
    [[nodiscard]] const bool GetIsDrawModel() const { return isDrawModel_; }
    [[nodiscard]] const bool GetIsAttackActive() const { return isAttackActive_; }

    [[nodiscard]] const int GetId() const { return id_; }
    [[nodiscard]] const std::string GetName() const { return name_; }

    [[nodiscard]] const bool GetIsHit() const { return isHit_; }

protected:
    DirectX::XMFLOAT3   offsetPosition_ = {};
    float               damageRadius_   = 1.0f;
    float               attackRadius_   = 2.0f;
    float               attackPower_    = 20.0f;

    float effectScele_ = 0.3f;
    float effectSpeed_ = 3.0f;

    bool isDrawModel_ = true;
    bool isAttackActive_ = false;
    
    float attackTimer_ = 0.0f;
    float attackTime_ = 0.2f;

    int         id_     = 0;  // 登録番号
    std::string name_   = ""; // 名前

    bool isHit_ = false;

    const int itemType_; // アイテムの種類
};

