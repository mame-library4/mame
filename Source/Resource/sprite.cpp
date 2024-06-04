#include "Sprite.h"
#include "Texture.h"
#include "Misc.h"
#include "Graphics.h"
#include "Camera.h"

// ----- ImGui�p -----
int Sprite::nameNum_ = 0;

// ----- �R���X�g���N�^ -----
Sprite::Sprite(const wchar_t* filename)
{
    HRESULT result = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

    // ���_���̃Z�b�g
    Vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // �V�F�[�_�[
    Graphics::Instance().CreateVsFromCso("./Resources/Shader/sprite_vs.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/sprite_ps.cso", pixelShader_.GetAddressOf());

    // �e�N�X�`���̃��[�h
    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    Texture::Instance().LoadTexture(filename, shaderResourceView_.GetAddressOf(), &texture2dDesc);

    // �摜�T�C�Y��ݒ�
    GetTransform()->SetSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));
    GetTransform()->SetTexSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));
    GetTransform()->SetOriginalSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));

    // ImGui���O�ݒ�
    std::string name = "Sprite" + std::to_string(nameNum_++);
    SetName(name.c_str());
}

// ----- �f�X�g���N�^ -----
Sprite::~Sprite()
{
    --nameNum_;
}

// ----- ������ -----
void Sprite::Initialize()
{
    // animation
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;
}

// ----- �X�V -----
void Sprite::Update(const float& elapsedTime)
{
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
            DirectX::XMFLOAT2 texPos = GetTransform()->GetTexPos();
            const DirectX::XMFLOAT2 texSize = GetTransform()->GetTexSize();

            if (animationVertical) texPos.y = texSize.y * animationFrame_;
            else                   texPos.x = texSize.x * animationFrame_;
            // ----- �ꉞ���̏����������Ă��� -----

            return true;
        }
    }

    DirectX::XMFLOAT2 texPos = GetTransform()->GetTexPos();
    const DirectX::XMFLOAT2 texSize = GetTransform()->GetTexSize();

    if (animationVertical) texPos.y = texSize.y * animationFrame_;
    else                   texPos.x = texSize.x * animationFrame_;

    GetTransform()->SetTexPos(texPos);

    return false;
}

void Sprite::ResetAnimation()
{
    transform_.SetTexPos(DirectX::XMFLOAT2(0, 0));
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;
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
void Sprite::Render(ID3D11PixelShader* psShader)
{
    HRESULT result = S_OK;
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();
    D3D11_VIEWPORT viewport = {};
    UINT numViewports = 1;
    
    // �X�N���[���T�C�Y���擾
    deviceContext->RSGetViewports(&numViewports, &viewport);

    // �Z�`�̊p���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
    // (x0, y0) *----* (x1, y1)
    //          |   /|
    //          |  / |
    //          | /  |
    //          |/   |
    // (x2, y2) *----* (x3, y3)

    // left-top
    float x0 = GetTransform()->GetPositionX();
    float y0 = GetTransform()->GetPositionY();
    // right-top
    float x1 = GetTransform()->GetPositionX() + GetTransform()->GetSizeX();
    float y1 = GetTransform()->GetPositionY();
    // left-bottom
    float x2 = GetTransform()->GetPositionX();
    float y2 = GetTransform()->GetPositionY() + GetTransform()->GetSizeY();
    // right-bottom
    float x3 = GetTransform()->GetPositionX() + GetTransform()->GetSizeX();
    float y3 = GetTransform()->GetPositionY() + GetTransform()->GetSizeY();

    // ��]���� ( ��]�̒��S��Z�`�̒��S�_�ɂ����ꍇ )
    float cx = GetTransform()->GetPositionX() + GetTransform()->GetSizeX() * 0.5f;
    float cy = GetTransform()->GetPositionY() + GetTransform()->GetSizeY() * 0.5f;
    Rotate(x0, y0, cx, cy, GetTransform()->GetAngle());
    Rotate(x1, y1, cx, cy, GetTransform()->GetAngle());
    Rotate(x2, y2, cx, cy, GetTransform()->GetAngle());
    Rotate(x3, y3, cx, cy, GetTransform()->GetAngle());

    // �X�N���[�����W�n���� NDC�ւ̍��W�ϊ��������Ȃ�
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
    D3D11_MAPPED_SUBRESOURCE mappedSubresouce = {};
    result = deviceContext->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresouce);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    Vertex* vertices = reinterpret_cast<Vertex*>(mappedSubresouce.pData);
    if (vertices != nullptr)
    {
        vertices[0].position_ = { x0, y0, 0 };
        vertices[1].position_ = { x1, y1, 0 };
        vertices[2].position_ = { x2, y2, 0 };
        vertices[3].position_ = { x3, y3, 0 };
        vertices[0].color_ = vertices[1].color_ = vertices[2].color_ = vertices[3].color_ = GetTransform()->GetColor();

        const DirectX::XMFLOAT2 texPos = GetTransform()->GetTexPos();
        const DirectX::XMFLOAT2 texSize = GetTransform()->GetTexSize();
        const DirectX::XMFLOAT2 originSize = GetTransform()->GetOriginalSize();

        vertices[0].texcoord_ = { texPos.x / originSize.x, texPos.y / originSize.y };
        vertices[1].texcoord_ = { (texPos.x + texSize.x) / originSize.x, texPos.y / originSize.y };
        vertices[2].texcoord_ = { texPos.x / originSize.x, (texPos.y + texSize.y) / originSize.y };
        vertices[3].texcoord_ = { (texPos.x + texSize.x) / originSize.x, (texPos.y + texSize.y) / originSize.y };
    }
    deviceContext->Unmap(vertexBuffer_.Get(), 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    deviceContext->IASetInputLayout(inputLayout_.Get());

    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    psShader ? deviceContext->PSSetShader(psShader, nullptr, 0) : deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);

    deviceContext->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

    deviceContext->Draw(4, 0);
}

// debug�p�֐�
void Sprite::DrawDebug()
{
    if (ImGui::BeginMenu(GetName()))
    {
        GetTransform()->DrawDebug();

        ImGui::DragFloat("AnimationFrame", &animationFrame_);


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
    GetTransform()->SubtractPosition(oldVibration_);

    // �U���̕��A�������Z�o����
    DirectX::XMFLOAT2 vibration = { (rand() % 100 - 50.0f), (rand() % 100 - 50.0f) };
    DirectX::XMVECTOR Vibration = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vibration));

    float vibrationVolume = volume * 1000 * elapsedTime;

    Vibration = DirectX::XMVectorScale(Vibration, vibrationVolume);
    DirectX::XMStoreFloat2(&vibration, Vibration);

    // �U���̒l������B
    GetTransform()->AddPosition(vibration);

    // ���݂̐U���l��ۑ����Ă���
    oldVibration_ = vibration;
}

void Sprite::Rotate(float& x, float& y, const float& centerX, const float& centerY, const float& angle)
{
    // �w��̈ʒu�ɓ�����
    x -= centerX;
    y -= centerY;

    // ��]����
    float cos = cosf(DirectX::XMConvertToRadians(angle));
    float sin = sinf(DirectX::XMConvertToRadians(angle));
    float tx = x;
    float ty = y;
    x = cos * tx + -sin * ty;
    y = sin * tx + cos * ty;

    // ���̈ʒu�ɖ߂�
    x += centerX;
    y += centerY;
}

void Sprite::Transform::DrawDebug()
{
    if (ImGui::TreeNode("Transform"))
    {
        ImGui::DragFloat2("Position", &position_.x);
        ImGui::Checkbox("SizeFactor", &isSizeFactor_);
        if (isSizeFactor_)
        {
            float size = GetSizeX();
            ImGui::DragFloat("Size", &size);
            SetSize(size);
        }
        else
        {
            ImGui::DragFloat2("Size", &size_.x);
        }
        ImGui::ColorEdit4("color", &color_.x);
        ImGui::DragFloat("angle", &angle_);
        ImGui::DragFloat2("texPos", &texPos_.x);
        ImGui::DragFloat2("texSize", &texSize_.x);
        ImGui::TreePop();
    }
}
