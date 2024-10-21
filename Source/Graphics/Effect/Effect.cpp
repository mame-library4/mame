#include "../Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

// ----- �R���X�g���N�^ -----
Effect::Effect(const char* filename, const std::string& effectName)
    : name_(effectName)
{
    // �G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
    // ���}���`�X���b�h��Effect���쐬�����DeviceContext�𓯎��A�N�Z�X����
    // �t���[�Y����\��������̂Ŕr�����䂷��
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    // Effekseer�̃��\�[�X��ǂݍ���
    // Effekseer��UTF-16�̃t�@�C���p�X�ȊO�͑Ή����Ă��Ȃ����ߕ����R�[�h���K�v
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    // Effekseer::Manager���擾
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    // Effekseer�G�t�F�N�g��ǂݍ���
    effekseerEffect_ = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);

    EffectManager::Instance().Register(this);
}


// ----- �Đ� -----
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, const float& scale, const float& speed)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
    
    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect_, position.x, position.y, position.z);
    
    effekseerManager->SetScale(handle, scale, scale, scale);
    effekseerManager->SetSpeed(handle, speed);

    return handle;
}

void Effect::Stop(const Effekseer::Handle& handle)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->StopEffect(handle);
}

// ----- �ʒu�ݒ� -----
void Effect::SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    Effekseer::Vector3D pos = Effekseer::Vector3D(position.x, position.y, position.z);
    effekseerManager->SetLocation(handle, pos);
}

// ----- �傫���ݒ� -----
void Effect::SetScale(const Effekseer::Handle& handle, const float& scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetScale(handle, scale, scale, scale);
}

void Effect::DrawDebug()
{
    if (ImGui::TreeNode(name_.c_str()))
    {
        if (ImGui::Button("Play"))
        {
            Play(position_, scale_, speed_);
        }

        ImGui::DragFloat3("Position", &position_.x);
        ImGui::DragFloat("Scale", &scale_);

        DirectX::XMFLOAT3 rotate = {};
        rotate.x = DirectX::XMConvertToDegrees(rotate_.x);
        rotate.y = DirectX::XMConvertToDegrees(rotate_.y);
        rotate.z = DirectX::XMConvertToDegrees(rotate_.z);

        ImGui::DragFloat3("Rotate", &rotate.x);

        rotate_.x = DirectX::XMConvertToRadians(rotate.x);
        rotate_.y = DirectX::XMConvertToRadians(rotate.y);
        rotate_.z = DirectX::XMConvertToRadians(rotate.z);

        ImGui::ColorEdit4("Color", &color_.x);

        ImGui::DragFloat("Speed", &speed_);

        ImGui::TreePop();
    }
}