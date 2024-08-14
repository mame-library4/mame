#include "EffectManager.h"
#include "Graphics.h"
#include "Camera.h"
#include "Common.h"

// ----- ������ -----
void EffectManager::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    // Effekseer�����_������
    effekseerRenderer_ = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(),
        graphics.GetDeviceContext(), 2048);

    // Effekseer�}�l�[�W���[����
    effekseerManager_ = Effekseer::Manager::Create(2048);

    // Effekseer�����_���̊e��ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK)
    effekseerManager_->SetSpriteRenderer(effekseerRenderer_->CreateSpriteRenderer());
    effekseerManager_->SetRibbonRenderer(effekseerRenderer_->CreateRibbonRenderer());
    effekseerManager_->SetRingRenderer(effekseerRenderer_->CreateRingRenderer());
    effekseerManager_->SetTrackRenderer(effekseerRenderer_->CreateTrackRenderer());
    effekseerManager_->SetModelRenderer(effekseerRenderer_->CreateModelRenderer());
    // Effekseer���ł̃��[�_�[�̐ݒ�(���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK)
    effekseerManager_->SetTextureLoader(effekseerRenderer_->CreateTextureLoader());
    effekseerManager_->SetModelLoader(effekseerRenderer_->CreateModelLoader());
    effekseerManager_->SetMaterialLoader(effekseerRenderer_->CreateMaterialLoader());

    // Effekseer�����舟���W�n�Ōv�Z����
    effekseerManager_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// ----- �I���� -----
void EffectManager::Finalize()
{
    Clear();
}

// ----- �X�V -----
void EffectManager::Update(float elapsedTime)
{
    // -------------------------
    //  ����
    // -------------------------
    for (Effect* effect : generates_)
    {
        effects_.emplace_back(effect);
    }
    generates_.clear();

    // �G�t�F�N�g�X�V����(�����ɂ̓t���[���̒ʉߎ��Ԃ�n��)
    effekseerManager_->Update(elapsedTime * 60.0f);

    // -------------------------
    //  �j��
    // -------------------------
    for (Effect* effect : removes_)
    {
        auto it = std::find(effects_.begin(), effects_.end(), effect);

        if (it != effects_.end())
        {
            effects_.erase(it);
        }

        SafeDeletePtr(effect);
    }
    removes_.clear();
}

// ----- �`�� -----
void EffectManager::Render()
{
    DirectX::XMFLOAT4X4 view, projection;
    DirectX::XMStoreFloat4x4(&view, Camera::Instance().GetViewMatrix());
    DirectX::XMStoreFloat4x4(&projection, Camera::Instance().GetProjectionMatrix());

    // view,projection Matrix�� EffekseerRenderer�ɐݒ�
    effekseerRenderer_->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
    effekseerRenderer_->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

    // Effekseer�`��J�n
    effekseerRenderer_->BeginRendering();

    // Effekseer�`����s
    // �}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐����삵�A
    // Draw()�֐������s���鏇���Ő���ł�����
    effekseerManager_->Draw();

    // Effekseer�`��I��
    effekseerRenderer_->EndRendering();
}

// ----- ImGui�p -----
void EffectManager::DrawDebug()
{
    if (ImGui::BeginMenu("EffectManager"))
    {
        for (Effect*& effect : effects_)
        {
            effect->DrawDebug();
        }

        ImGui::EndMenu();
    }
}

// ----- �o�^ -----
void EffectManager::Register(Effect* effect)
{
    generates_.insert(effect);
}

// ----- �폜 -----
void EffectManager::Remove(Effect* effect)
{
    removes_.insert(effect);
}

// ----- �S�폜 -----
void EffectManager::Clear()
{
    for (Effect*& effect : effects_)
    {
        SafeDeletePtr(effect);
    }
    effects_.clear();
    effects_.shrink_to_fit();
}

// ----- �G�t�F�N�g�擾 -----
Effect* EffectManager::GetEffect(const std::string& name)
{
    for (Effect*& effect : effects_)
    {
        if (effect->GetName() == name)
        {
            return effect;
        }
    }

    return nullptr;
}

void EffectManager::StopEffect(const Effekseer::Handle& handle)
{
    effekseerManager_->StopEffect(handle);
}

void EffectManager::SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position)
{
    effekseerManager_->SetLocation(handle, position.x, position.y, position.z);
}

void EffectManager::AddPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& addPosition)
{    
    effekseerManager_->AddLocation(handle, Effekseer::Vector3D(addPosition.x, addPosition.y, addPosition.z));
}
