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

    enum class HitStopType
    {
        Normal,
        Critical,
    };


public:// --- �擾�E�ݒ� ---
    [[nodiscard]] const bool IsCreatePlayer() const { return player_ != nullptr; }
    std::unique_ptr<Player>& GetPlayer() { return player_; }
    Transform* GetTransform() { return player_->GetTransform(); }

    // ---------- ���萧��p�t���O ----------
    [[nodiscard]] const bool GetUseCollisionDetection() const { return useCollisionDetection_; }
    void SetUseCollisionDetection(const bool& flag) { useCollisionDetection_ = flag; }

    // ---------- �q�b�g�X�g�b�v ----------
    void SetHitStop(const HitStopType& type);

private:
    std::unique_ptr<Player> player_;

    // ---------- ���萧��p�t���O ----------
    bool useCollisionDetection_ = true; // �����o������p

    // ---------- �q�b�g�X�g�b�v ----------
    int     hitStopFrame_           = 0;
    int     currentHitStopFrame_    = 0;
    int     normalHitStopFrame_     = 5;
    int     criticalHitStopFrame_   = 7;
    bool    isHitStopActive_        = false;
};

