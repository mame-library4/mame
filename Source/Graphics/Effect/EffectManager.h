#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "Effect.h"
#include <vector>
#include <set>

// エフェクトマネージャー
class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}

public:
    static EffectManager& Instance()
    {
        static EffectManager instance;
        return instance;
    }

    void Initialize();                      // 初期化
    void Finalize();                        // 終了化
    void Update(const float& elapsedTime);  // 更新
    void Render();                          // 描画
    void DrawDebug();                       // ImGui用

    // Effekseerマネージャーの取得
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager_; }

    void Register(Effect* effect);  // 登録
    void Remove(Effect* effect);    // 削除
    void Clear();                   // 全削除

    [[nodiscard]] Effect* GetEffect(const std::string& name);

    void StopEffect(const Effekseer::Handle& handle);
    void SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position);
    void AddPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& addPosition);

private:
    Effekseer::ManagerRef effekseerManager_;
    EffekseerRenderer::RendererRef effekseerRenderer_;

    std::vector<Effect*>    effects_;
    std::set<Effect*>       generates_;
    std::set<Effect*>       removes_;
};



