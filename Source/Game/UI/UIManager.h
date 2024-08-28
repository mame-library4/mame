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
        // ----- `æ -----
        UINumber,       // |
        UIFader,        // |
        Max,            // «
    };

public:
    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(UI* ui);
    void Remove(UI* ui);
    void Clear();

public:// [Get, Set] Function
    [[nodiscard]] const int GetImGuiNameNum() { return imguiNameNum_++; }

private:
    std::vector<UI*>    userInterface_;
    std::set<UI*>       removes_;
    std::set<UI*>       generates_;

    int imguiNameNum_ = 0;
};

