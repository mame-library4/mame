#include "sprite.h"
#include "../Other/misc.h"
#include <sstream>

#include "../../../External/DirectXTK-main/Inc/WICTextureLoader.h"

#include "texture.h"
#include "../Graphics/shader.h"
#include "../Graphics/Graphics.h"

#include "NoiseTexture.h"

#include "../Other/Easing.h"

//---ImGui���O���Ԃ�h�~�p---//
int Sprite::nameNum = 0;
//---ImGui���O���Ԃ�h�~�p---//

// �R���X�g���N�^
Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };
    Graphics& graphics = Graphics::Instance();

    // Animation
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;

    // ���_���̃Z�b�g
    Vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    // �萔�o�b�t�@����
    {
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = sizeof(vertices);
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subresource_data{};
        subresource_data.pSysMem = vertices;
        subresource_data.SysMemPitch = 0;
        subresource_data.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufferDesc, &subresource_data, vertexBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // spriteDissolve
        bufferDesc.ByteWidth = sizeof(DissolveConstants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        hr = graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, dissolveConstantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // �V�F�[�_�[�֘A
    CreateVsFromCso(device, "./Resources/Shader/sprite_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    CreatePsFromCso(device, "./Resources/Shader/sprite_ps.cso", pixelShader.GetAddressOf());

    // �e�N�X�`���̃��[�h
    LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

    // �摜�T�C�Y��ݒ�
    GetSpriteTransform()->SetSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));
    GetSpriteTransform()->SetTexSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));

    //---ImGui���O���Ԃ�h�~�p---//
    std::string name = "Sprite" + std::to_string(nameNum++);
    SetName(name.c_str());
    //---ImGui���O���Ԃ�h�~�p---//
}

// �f�X�g���N�^
Sprite::~Sprite()
{
    --nameNum;
}

// ������
void Sprite::Initialize()
{
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;
}

void Sprite::Update(const float& elapsedTime)
{
    // �f�B�]���u�X�V
    UpdateSpriteDissolve(elapsedTime);
}

void Sprite::UpdateSpriteDissolve(const float& elapsedTime)
{
    if (isFade_)
    {
        if (GetSpriteDissolve()->GetDissolveType() == 0)
        {
            if (FadeIn(elapsedTime))SetIsFade(false);
        }
        else
        {
            if (FadeOut(elapsedTime))SetIsFade(false);
        }
    }

    // ����������Fade�ɂȂ�悤�ɐݒ肵�Ă�
    GetSpriteDissolve()->SetDissolveValue(GetSpriteDissolve()->GetDissolveBlackValue() - GetSpriteDissolve()->GetDelay());
}

bool Sprite::FadeIn(const float& elapsedTime)
{
    GetSpriteDissolve()->SubtractDissolveBlackValue(elapsedTime);

    return GetSpriteDissolve()->GetDissolveBlackValue() < 0.0f;
}

bool Sprite::FadeOut(const float& elapsedTime)
{
    GetSpriteDissolve()->AddDissolveBlackValue(elapsedTime);

    return GetSpriteDissolve()->GetDissolveBlackValue() > 2.0f;
}

// �A�j���[�V�����֐�
bool Sprite::PlayAnimation(const float elapsedTime, const float frameSpeed, const float totalAnimationFrame, const bool animationVertical, const bool loop)
{
    animationTime_ += frameSpeed * elapsedTime;

    const int frame = static_cast<int>(animationTime_); // �����_�؂�̂�
    animationFrame_ = static_cast<float>(frame);

    if (loop)
    {
        if (animationFrame_ > totalAnimationFrame)
        {
            animationFrame_ = 0.0f;
            animationTime_ = 0.0f;
        }
    }
    else
    {
        if (animationFrame_ >= totalAnimationFrame)
        {
            animationFrame_ = totalAnimationFrame - 1;

            // ----- �ꉞ���̏����������Ă��� -----
            DirectX::XMFLOAT2 texPos = GetSpriteTransform()->GetTexPos();
            const DirectX::XMFLOAT2 texSize = GetSpriteTransform()->GetTexSize();

            if (animationVertical) texPos.y = texSize.y * animationFrame_;
            else                   texPos.x = texSize.x * animationFrame_;
            // ----- �ꉞ���̏����������Ă��� -----

            return true;
        }
    }

    DirectX::XMFLOAT2 texPos = GetSpriteTransform()->GetTexPos();
    const DirectX::XMFLOAT2 texSize = GetSpriteTransform()->GetTexSize();

    if (animationVertical) texPos.y = texSize.y * animationFrame_;
    else                   texPos.x = texSize.x * animationFrame_;

    GetSpriteTransform()->SetTexPos(texPos);

    return false;
}

DirectX::XMFLOAT2 Sprite::ConvertToScreenPos(
    const DirectX::XMFLOAT3 worldPos,
    bool* isDraw)
{
    auto* dc = Graphics::Instance().GetDeviceContext();
    //�r���[�|�[�g
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    dc->RSGetViewports(&numViewports, &viewport);

    DirectX::XMMATRIX View = Camera::Instance().GetViewMatrix();
    DirectX::XMMATRIX Projection = Camera::Instance().GetProjectionMatrix();
    DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

    DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPos);

    DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
        WorldPosition,
        viewport.TopLeftX, viewport.TopLeftY,
        viewport.Width, viewport.Height,
        viewport.MinDepth, viewport.MaxDepth,
        Projection, View, World
    );

    DirectX::XMFLOAT2 screenPosition;
    DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

    const float screenPositionZ = DirectX::XMVectorGetZ(ScreenPosition);
    if (isDraw != nullptr) { (*isDraw) = (screenPositionZ >= 1.0f) ? false : true; }

    return screenPosition;
}

// �`��
void Sprite::Render(ID3D11PixelShader* psShader, const char* type)
{
    Graphics& graphics = Graphics::Instance();
    Shader* shader = graphics.GetShader();
    
    // �����Z�b�g����̂��ɂ���ĕ�����
    SetConstantBuffer(type);

    // spriteDissolve
    NoiseTexture::Instance().SetConstantBuffers(1);

    // �萔�o�b�t�@�̍X�V
    {
        DissolveConstants dissolve{};
        dissolve.parameters.x = GetSpriteDissolve()->GetDissolveValue();
        dissolve.parameters.y = GetSpriteDissolve()->GetDissolveBlackValue();
        dissolve.parameters.z = GetSpriteDissolve()->GetEdgeThreshold();
        dissolve.edgeColor = GetSpriteDissolve()->GetEdgeColor();
        graphics.GetDeviceContext()->UpdateSubresource(dissolveConstantBuffer.Get(), 0, 0, &dissolve, 0, 0);
        graphics.GetDeviceContext()->VSSetConstantBuffers(3, 1, dissolveConstantBuffer.GetAddressOf());
        graphics.GetDeviceContext()->PSSetConstantBuffers(3, 1, dissolveConstantBuffer.GetAddressOf());
    }

    // �`��
    Render(graphics.GetDeviceContext(), psShader);
}

void Sprite::SetConstantBuffer(const char* type)
{
    Shader* shader = Graphics::Instance().GetShader();

    if (type == "Emissive")
    {
        shader->SetEmissiveColor(GetEmissive()->GetEmissiveColor());
        shader->SetEmissiveIntensity(GetEmissive()->GetEmissiveIntensity());
        shader->UpdateEmissiveConstants(6);
    }

    if (type == "Dissolve")
    {
        shader->SetDissolveIntensity(GetSpriteDissolve()->GetDissolveValue());
        shader->UpdateDissolveConstants(3);
    }
}


// �`�悷��{�̂̊֐�
void Sprite::Render(ID3D11DeviceContext* deviceContext, ID3D11PixelShader* psShader)
{
    HRESULT hr{ S_OK };

    // �X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT numViewports{ 1 };
    deviceContext->RSGetViewports(&numViewports, &viewport);

    // �Z�`�̊p���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
    // (x0, y0) *----* (x1, y1)
    //          |   /|
    //          |  / |
    //          | /  |
    //          |/   |
    // (x2, y2) *----* (x3, y3)

    // left-top
    float x0{ GetSpriteTransform()->GetPosX() };
    float y0{ GetSpriteTransform()->GetPosY() };
    // right-top
    float x1{ GetSpriteTransform()->GetPosX() + GetSpriteTransform()->GetSizeX() };
    float y1{ GetSpriteTransform()->GetPosY() };
    // left-bottom
    float x2{ GetSpriteTransform()->GetPosX() };
    float y2{ GetSpriteTransform()->GetPosY() + GetSpriteTransform()->GetSizeY() };
    // right-bottom
    float x3{ GetSpriteTransform()->GetPosX() + GetSpriteTransform()->GetSizeX() };
    float y3{ GetSpriteTransform()->GetPosY() + GetSpriteTransform()->GetSizeY() };

    // ��`��]
    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    {
        x -= cx;
        y -= cy;

        float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
        float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    };
    // ��]�̒��S��Z�`�̒��S�_�ɂ����ꍇ
    float cx = GetSpriteTransform()->GetPosX() + GetSpriteTransform()->GetSizeX() * 0.5f;
    float cy = GetSpriteTransform()->GetPosY() + GetSpriteTransform()->GetSizeY() * 0.5f;
    rotate(x0, y0, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x1, y1, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x2, y2, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x3, y3, cx, cy, GetSpriteTransform()->GetAngle());

    // �X�N���[�����W�n����NDC�ւ̍��W�ϊ��������Ȃ�
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0, 0 };
        vertices[1].position = { x1, y1, 0 };
        vertices[2].position = { x2, y2, 0 };
        vertices[3].position = { x3, y3, 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = GetSpriteTransform()->GetColor();

        vertices[0].texcord = { GetSpriteTransform()->GetTexPosX() / texture2dDesc.Width, GetSpriteTransform()->GetTexPosY() / texture2dDesc.Height };
        vertices[1].texcord = { (GetSpriteTransform()->GetTexPosX() + GetSpriteTransform()->GetTexSizeX()) / texture2dDesc.Width,GetSpriteTransform()->GetTexPosY() / texture2dDesc.Height };
        vertices[2].texcord = { GetSpriteTransform()->GetTexPosX() / texture2dDesc.Width, (GetSpriteTransform()->GetTexPosY() + GetSpriteTransform()->GetTexSizeY()) / texture2dDesc.Height };
        vertices[3].texcord = { (GetSpriteTransform()->GetTexPosX() + GetSpriteTransform()->GetTexSizeX()) / texture2dDesc.Width, (GetSpriteTransform()->GetTexPosY() + GetSpriteTransform()->GetTexSizeY()) / texture2dDesc.Height };
    }
    deviceContext->Unmap(vertexBuffer.Get(), 0);

    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    deviceContext->IASetInputLayout(inputLayout.Get());

    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

    deviceContext->Draw(4, 0);
    //---�����艺�ɉ������Ă��Ӗ��Ȃ�---//
}

// debug�p�֐�
void Sprite::DrawDebug()
{
    if (ImGui::BeginMenu(GetName()))
    {
        GetSpriteTransform()->DrawDebug();

        GetSpriteDissolve()->DrawDebug();

        GetEmissive()->DrawDebug();

        ImGui::DragFloat(u8"�A�j���[�V�����t���[��", &animationFrame_);

        if (ImGui::Button("Fade"))SetIsFade(true);

        ImGui::EndMenu();
    }
}

// �U��
void Sprite::Vibration(const float& elapsedTime, const float& volume, const float& breakTime)
{
    if (volume <= 0) return; // �U���l���Ȃ��ꍇ�͏������Ȃ�
    
    // �U���̊Ԃ̎��Ԃ��󂯂邽�߂̏���
    if (vibrationBreakTimer_ >= 0.0f)
    {   // breakTime���c���Ă���Ƃ��͏������Ȃ�
        vibrationBreakTimer_ -= elapsedTime;
        return;
    }

    // breakTime��ݒ�
    vibrationBreakTimer_ = breakTime;

    // �O��̌��ʂ����Z�b�g����
    GetSpriteTransform()->SubtractPos(oldVibration_);

    // �U���̕��A�������Z�o����
    DirectX::XMFLOAT2 vibration = { (rand() % 100 - 50.0f), (rand() % 100 - 50.0f) };
    DirectX::XMVECTOR Vibration = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vibration));

    float vibrationVolume = volume * 1000 * elapsedTime;

    Vibration = DirectX::XMVectorScale(Vibration, vibrationVolume);
    DirectX::XMStoreFloat2(&vibration, Vibration);

    // �U���̒l������B
    GetSpriteTransform()->AddPos(vibration);

    // ���݂̐U���l��ۑ����Ă���
    oldVibration_ = vibration;
}

// SpriteTransform
void Sprite::SpriteTransform::DrawDebug()
{
    if (ImGui::TreeNode("spriteTransform"))
    {
        ImGui::DragFloat2(u8"�ʒu", &position_.x); // test u8
        ImGui::DragFloat2("size", &size_.x);

        if (ImGui::Button("isSizeFactor"))
        {
            isSizeFactor_ = !isSizeFactor_;
        }
        if (isSizeFactor_)
        {
            float sizeFactor = size_.x;
            ImGui::DragFloat("sizeFactor", &sizeFactor);
            SetSize(sizeFactor);
        }

        ImGui::ColorEdit4("color", &color_.x);
        ImGui::DragFloat("angle", &angle_);
        ImGui::DragFloat2("texPos", &texPos_.x);
        ImGui::DragFloat2("texSize", &texSize_.x);
        ImGui::TreePop();
    }
}

// SpriteDissolve
void Sprite::SpriteDissolve::DrawDebug()
{
    if (ImGui::TreeNode("spriteDissolve"))
    {
        ImGui::SliderInt("textureValue", &maskTextureValue, 0, 7);
        ImGui::SliderFloat("dissolveValue", &dissolveValue, 0.0f, 2.0f);
        ImGui::SliderFloat("dissolveBlackValue", &dissolveBlackValue, 0.0f, 2.0f);
        ImGui::SliderFloat("edgeThreshold", &edgeThreshold, 0.0f, 1.0f);
        ImGui::ColorEdit4("edgeColor", &edgeColor.x);
        ImGui::SliderFloat("delay", &delay, 0.0f, 1.0f);
        ImGui::SliderInt("dissolveType", &dissolveType, 0, 1);
        ImGui::TreePop();
    }
}

void Sprite::Emissive::DrawDebug()
{
    if (ImGui::TreeNode("Emissive"))
    {
        ImGui::ColorEdit4("emissiveColor", &emissiveColor_.x);
        ImGui::DragFloat("emissiveIntensity", &emissiveIntensity_);

        ImGui::TreePop();
    }
}
