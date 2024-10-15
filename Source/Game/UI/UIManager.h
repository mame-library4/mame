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

    // ---------- UI�̎�� ----------
    //   ��̍��ڂ��珇�Ԃɕ`�悳���
    // ------------------------------
    enum class UIType
    {
        // ------- �`�揇 -------
        UINumber,           // |
        UICrosshair,        // |
        UINotifications,    // |
        UIActionGuide,      // |
        UIStamina,          // |
        UIHealth,           // |
        UIFlashOut,         // |
        UIFader,            // |
        Max,                // ��
    };

public:
    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(UI* ui);              // �o�^
    void Remove(UI* ui);                // �폜 �w���UI
    void Remove(const UIType& type);    // �폜 �w��̎�ނ�UI�S��
    void Clear();                       // �S�폜

    // �ЂƂ������݂���UI���擾����
    UI* GetUI(const UIType& type);

public:// [Get, Set] Function
    [[nodiscard]] const int GetImGuiNameNum() { return imguiNameNum_++; }

private:
    std::vector<UI*>    userInterface_;
    std::set<UI*>       removes_;
    std::set<UI*>       generates_;

    int imguiNameNum_ = 0;
};

