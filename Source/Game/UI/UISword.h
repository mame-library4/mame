#pragma once
#include "UI.h"

class UISword : public UI
{
public:
    UISword();
    ~UISword() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    std::unique_ptr<Sprite> swordFrame_;
    std::unique_ptr<Sprite> swordFrameBlack_;

    bool isAllUICreated = false; // 全てのリソースが生成されたか

    bool isSwordFrameActive_ = false;
};

