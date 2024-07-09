#pragma once
#include "UI.h"
#include <vector>
#include <set>

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

    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(UI* ui);
    void Remove(UI* ui);
    void Clear();

private:
    std::vector<UI*>    userInterface_;
    std::set<UI*>       removes_;
    std::set<UI*>       generates_;
};

