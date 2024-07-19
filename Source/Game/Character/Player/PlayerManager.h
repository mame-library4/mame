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
    void RenderTrail();
    void DrawDebug();
    void DebugRender(DebugRenderer* debugRenderer);

    void CollisionPlayerVsEnemy();

public:// --- 取得・設定 ---
    std::unique_ptr<Player>& GetPlayer() { return player_; }
    Transform* GetTransform() { return player_->GetTransform(); }

    // ---------- 判定制御用フラグ ----------
    [[nodiscard]] const bool GetUseCollisionDetection() const { return useCollisionDetection_; }
    void SetUseCollisionDetection(const bool& flag) { useCollisionDetection_ = flag; }

private:
    std::unique_ptr<Player> player_;

    // ---------- 判定制御用フラグ ----------
    bool useCollisionDetection_ = true; // 押し出し判定用

    bool isSkip_ = false;
    int skipNum_ = 0;
};

