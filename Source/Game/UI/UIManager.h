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

    // ---------- UIÌíÞ ----------
    //   ãÌÚ©çÔÉ`æ³êé
    // ------------------------------
    enum class UIType
    {
        // ------- `æ -------
        UINumber,           // |
        UICrosshair,        // |
        UINotifications,    // |
        UIStamina,          // |
        UIHealth,           // |
        UIFader,            // |
        Max,                // «
    };

public:
    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(UI* ui);              // o^
    void Remove(UI* ui);                // í wèÌUI
    void Remove(const UIType& type);    // í wèÌíÞÌUISÄ
    void Clear();                       // Sí

public:// [Get, Set] Function
    [[nodiscard]] const int GetImGuiNameNum() { return imguiNameNum_++; }

private:
    std::vector<UI*>    userInterface_;
    std::set<UI*>       removes_;
    std::set<UI*>       generates_;

    int imguiNameNum_ = 0;
};

