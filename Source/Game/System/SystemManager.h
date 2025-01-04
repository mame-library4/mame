#pragma once
class SystemManager
{
private:
    SystemManager() {}
    ~SystemManager() {}

public:
    static SystemManager& Instance()
    {
        static SystemManager instance;
        return instance;
    }

    [[nodiscard]] const bool Update();
    void DrawDebug(); // ImGui用

public:
#pragma region ---------- [Set,Get] Function ----------
    // ---------- スロー ----------
    [[nodiscard]] const float GetAllSlowSpeed() const { return allSlowSpeed_; }
    void SetAllSlowSpeed(const float& speed) { allSlowSpeed_ = speed; }
    [[nodiscard]] const float GetPlayerSlowSpeed() const { return playerSlowSpeed_; }
    void SetPlayerSlowSpeed(const float& speed) { playerSlowSpeed_ = speed; }

#pragma endregion ---------- [Set,Get] Function ----------

private:
    // ---------- ポーズの更新処理 ----------
    [[nodiscard]] const bool UpdatePauseState();

    // ---------- Player役職変更 ----------
    void UpdatePlayerRole();

private:
    bool isPaused_ = false; // ポーズ中か

    // ---------- スロー ----------
    float allSlowSpeed_     = 1.0f; // スロー速度 (プレイヤー以外)
    float playerSlowSpeed_  = 1.0f; // スロー速度 (プレイヤーのみ)
};

