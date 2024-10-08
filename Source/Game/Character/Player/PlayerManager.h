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



public:// --- �擾�E�ݒ� ---
    std::unique_ptr<Player>& GetPlayer() { return player_; }
    Transform* GetTransform() { return player_->GetTransform(); }

    // ---------- ���萧��p�t���O ----------
    [[nodiscard]] const bool GetUseCollisionDetection() const { return useCollisionDetection_; }
    void SetUseCollisionDetection(const bool& flag) { useCollisionDetection_ = flag; }

    // ---------- �q�b�g�X�g�b�v ----------
    void SetHitStop(const float& hitStopFrame);

private:
    std::unique_ptr<Player> player_;

    // ---------- ���萧��p�t���O ----------
    bool useCollisionDetection_ = true; // �����o������p

    // ---------- �q�b�g�X�g�b�v ----------
    int     hitStopFrame_           = 0;
    int     currentHitStopFrame_    = 0;
    bool    isHitStopActive_        = false;
};

