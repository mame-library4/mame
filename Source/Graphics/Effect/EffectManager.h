#pragma once
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "Effect.h"
#include <vector>
#include <set>

// �G�t�F�N�g�}�l�[�W���[
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

    void Initialize();              // ������
    void Finalize();                // �I����
    void Update(float elapsedTime); // �X�V
    void Render();                  // �`��
    void DrawDebug();               // ImGui�p

    // Effekseer�}�l�[�W���[�̎擾
    Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager_; }

    void Register(Effect* effect);  // �o�^
    void Remove(Effect* effect);    // �폜
    void Clear();                   // �S�폜

    [[nodiscard]] Effect* GetEffect(const std::string& name);

private:
    Effekseer::ManagerRef effekseerManager_;
    EffekseerRenderer::RendererRef effekseerRenderer_;

    std::vector<Effect*>    effects_;
    std::set<Effect*>       removes_;
    std::set<Effect*>       generates_;
};



