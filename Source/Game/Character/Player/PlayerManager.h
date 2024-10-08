#pragma once
#include "Player.h"

class PlayerManager
{
private:
    PlayerManager() {}
    ~PlayerManager() {}

public:
    static PlayerManager& Instance()
    {
        static PlayerManager instance;
        return instance;
    }

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader = nullptr);
    void CastShadow();
    void RenderTrail();
    void DrawDebug();
    void DebugRender(DebugRenderer* debugRenderer);



public:// --- 取得・設定 ---
    std::unique_ptr<Player>& GetPlayer() { return player_; }
    Transform* GetTransform() { return player_->GetTransform(); }

    // ---------- 判定制御用フラグ ----------
    [[nodiscard]] const bool GetUseCollisionDetection() const { return useCollisionDetection_; }
    void SetUseCollisionDetection(const bool& flag) { useCollisionDetection_ = flag; }

    // ---------- ヒットストップ ----------
    void SetHitStop(const float& hitStopFrame);

private:
    std::unique_ptr<Player> player_;

    // ---------- 判定制御用フラグ ----------
    bool useCollisionDetection_ = true; // 押し出し判定用

    // ---------- ヒットストップ ----------
    int     hitStopFrame_           = 0;
    int     currentHitStopFrame_    = 0;
    bool    isHitStopActive_        = false;
};

