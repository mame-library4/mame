#pragma once
#include <vector>
#include <set>
#include "ImGui/ImGuiCtrl.h"

class UI;

class UIManager
{
private:
    UIManager() {}
    ~UIManager() {}

public:
    static UIManager& Instance()
    {
        static UIManager instance;
        return instance;
    }

    // ---------- UIの種類 ----------
    //   上の項目から順番に描画される
    // ------------------------------
    enum class UIType
    {
        // ------- 描画順 -------
        UINumber,           // |
        UICrosshair,        // |
        UINotifications,    // |
        UIActionGuide,      // |
        UIGuardGauge,       // |
        UIStamina,          // |
        UIHealth,           // |
        UISword,            // |
        UIRush,             // |
        UITitle,            // |
        UIFlashOut,         // |
        UIFader,            // |
        Max,                // ↓
    };

public:
    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(UI* ui);              // 登録
    void Remove(UI* ui);                // 削除 指定のUI
    void Remove(const UIType& type);    // 削除 指定の種類のUI全て
    void Clear();                       // 全削除

    void PlayAllUIFadeOut(); // 全UIフェードアウトする

    // ひとつだけ存在するUIを取得する
    UI* GetUI(const UIType& type);

public:// [Get, Set] Function
    [[nodiscard]] const int GetImGuiNameNum() { return imguiNameNum_++; }

private:
    std::vector<UI*>    userInterface_;
    std::set<UI*>       removes_;
    std::set<UI*>       generates_;

    int imguiNameNum_ = 0;
};

