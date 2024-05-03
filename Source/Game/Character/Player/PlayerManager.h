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
    void Render();
    void DrawDebug();
    void DebugRender(DebugRenderer* debugRenderer);

    void CollisionPlayerVsEnemy();

public:// --- �擾�E�ݒ� ---
    std::unique_ptr<Player>& GetPlayer() { return player_; }
    Transform* GetTransform() { return player_->GetTransform(); }


private:
    std::unique_ptr<Player> player_;

};

