#pragma once
#include <string>
#include <vector>
#include <d3d11.h>

class BaseScene
{
public:
    BaseScene() {}
    virtual ~BaseScene() {}

    virtual void CreateResource()                   = 0; // リソース生成
    virtual void Initialize()                       = 0; // 初期化
    virtual void Finalize()                         = 0; // 終了化
    virtual void Update(const float& elapsedTime)   = 0; // 更新処理

    virtual void ShadowRender()                     = 0; 
    virtual void DeferredRender()                   = 0;
    virtual void ForwardRender()                    = 0;

    // テスト用
    virtual void Render() = 0;
    
    virtual void DrawDebug()                        = 0; // ImGui用

public:
    bool IsReady()const { return ready; }
    void SetReady() { ready = true; }

private:
    bool ready = false; // 準備完了管理フラグ
};

