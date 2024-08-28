#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include "MathHelper.h"

// ----- Animation�̂��� -----
// PlayAnimation�֐����Ăяo��
// PlayAnimation( �t���[���o�ߎ���, �A�j���[�V�������鑬��,
//      �A�j���[�V�����t���[����, �c�A�j���[�V�����̏ꍇtrue);

class Sprite
{
public:
    struct Transform
    {
    public:
        // --------------- Position ---------------
        [[nodiscard]] const DirectX::XMFLOAT2 GetPosition() const { return position_; }
        [[nodiscard]] const float GetPositionX() const { return position_.x; }
        [[nodiscard]] const float GetPositionY() const { return position_.y; }
        void SetPosition(const DirectX::XMFLOAT2& position) { position_ = position; };
        void SetPosition(const float& x, const float& y) { position_ = { x, y }; }
        void SetPositionX(const float& x) { position_.x = x; }
        void SetPositionY(const float& y) { position_.y = y; }
        // �����Ŏw�肵���ʒu���摜�̒��S�Ƃ��Ĉʒu��ݒ肷��
        void SetPosition(DirectX::XMFLOAT2 position, const DirectX::XMFLOAT2& size)
        {
            position += size / 2;
            position -= size_ / 2;
            position_ = position;
        }
        void AddPosition(const DirectX::XMFLOAT2& position) { position_ += position; }
        void AddPosition(const float& x, const float& y) { position_ += { x, y }; }
        void AddPositionX(const float& x) { position_.x += x; }
        void AddPositionY(const float& y) { position_.y += y; }
        void SubtractPosition(const DirectX::XMFLOAT2& position) { position_ -= position; }
        void SubtractPosition(const float& x, const float& y) { position_ -= { x, y }; }
        void SubtractPositionX(const float& x) { position_.x -= x; }
        void SubtractPositionY(const float& y) { position_.y -= y; }

        // --------------- Size ---------------
        [[nodiscard]] const DirectX::XMFLOAT2 GetSize() const { return size_; }
        [[nodiscard]] const float GetSizeX() const { return size_.x; }
        [[nodiscard]] const float GetSizeY() const { return size_.y; }
        void SetSize(const DirectX::XMFLOAT2& size) { size_ = size; }
        void SetSize(const float& x, const float& y) { size_ = { x, y }; }
        void SetSize(const float& size) { size_ = { size, size }; }
        void SetSizeX(const float& x) { size_.x = x; }
        void SetSizeY(const float& y) { size_.y = y; }

        // --------------- Color ---------------
        [[nodiscard]] const DirectX::XMFLOAT4 GetColor() const { return color_; }
        [[nodiscard]] const float GetColorR() const { return color_.x; }
        [[nodiscard]] const float GetColorG() const { return color_.y; }
        [[nodiscard]] const float GetColorB() const { return color_.z; }
        [[nodiscard]] const float GetColorA() const { return color_.w; }
        void SetColor(const DirectX::XMFLOAT4& color) { color_ = color_; }
        void SetColor(const DirectX::XMFLOAT3& color) { color_ = { color.x, color.y, color.z, color_.w }; }
        void SetColor(const float& r, const float& g, const float& b, const float& a) { color_ = { r, g, b, a }; }
        void SetColor(const float& r, const float& g, const float& b) { color_ = { r, g, b, color_.w }; }
        void SetColorR(const float& r) { color_.x = r; }
        void SetColorG(const float& g) { color_.y = g; }
        void SetColorB(const float& b) { color_.z = b; }
        void SetColorA(const float& a) { color_.w = a; }
        void SetColorWhite() { color_ = { 1, 1, 1, color_.w }; }
        void SetColorBlack() { color_ = { 0, 0, 0, color_.w }; }

        // --------------- Angle ---------------
        [[nodiscard]] const float GetAngle() const { return angle_; }
        void SetAngle(const float& angle) { angle_ = angle; }
        void AddAngle(const float& angle) { angle_ += angle; }
        void SubtractAngle(const float& angle) { angle_ -= angle; }

        // --------------- TexPos ---------------
        [[nodiscard]] const DirectX::XMFLOAT2 GetTexPos() const { return texPos_; }
        [[nodiscard]] const float GetTexPosX() const { return texPos_.x; }
        [[nodiscard]] const float GetTexPosY() const { return texPos_.y; }
        void SetTexPos(const DirectX::XMFLOAT2& texPos) { texPos_ = texPos_; }
        void SetTexPos(const float& x, const float& y) { texPos_ = { x, y }; }
        void SetTexPosX(const float& x) { texPos_.x = x; }
        void SetTexPosY(const float& y) { texPos_.y = y; }

        // --------------- TexSize ---------------
        [[nodiscard]] const DirectX::XMFLOAT2 GetTexSize() const { return texSize_; }
        [[nodiscard]] const float GetTexSizeX() const { return texSize_.x; }
        [[nodiscard]] const float GetTexSizeY() const { return texSize_.y; }
        void SetTexSize(const DirectX::XMFLOAT2& texSize) { texSize_ = texSize; }
        void SetTexSize(const float& x, const float& y) { texSize_ = { x, y }; }
        void SetTexSize(const float& texSize) { texSize_ = { texSize, texSize }; }
        void SetTexSizeX(const float& x) { texSize_.x = x; }
        void SetTexSizeY(const float& y) { texSize_.y = y; }

        // --------------- OriginalSize ---------------
        [[nodiscard]] const DirectX::XMFLOAT2 GetOriginalSize() const { return originalSize_; }
        void SetOriginalSize(const DirectX::XMFLOAT2& originalSize) { originalSize_ = originalSize; }

        void DrawDebug();

    private:
        DirectX::XMFLOAT2   position_       = {};               // �ʒu
        DirectX::XMFLOAT2   size_           = {};               // �傫��
        DirectX::XMFLOAT4   color_          = { 1, 1, 1, 1 };   // �F
        float               angle_          = 0.0f;             // �p�x
        DirectX::XMFLOAT2   texPos_         = {};               // �e�N�X�`�����W
        DirectX::XMFLOAT2   texSize_        = {};               // �e�N�X�`���T�C�Y
        DirectX::XMFLOAT2   originalSize_   = {};               // �I���W�i���T�C�Y
        bool                isSizeFactor_   = false;          // ImGui�p
    };

public:
    Sprite(const wchar_t* filename);
    ~Sprite();

    void Initialize();                                  // ������
    void Update(const float& elapsedTime);              // �X�V
    void Render(ID3D11PixelShader* psShader = nullptr); // �`��
    void DrawDebug();                                   // ImGui�p

    // ---------- Animation ----------
    bool PlayAnimation(
        const float elapsedTime,                // �o�ߎ���
        const float frameSpeed,                 // �A�j���[�V�������x
        const float totalAnimationFrame,        // �A�j���[�V�����t���[����
        const bool animationVertical = false,   // �c�A�j���[�V�����̏ꍇtrue
        const bool loop = false                 // ���[�v���邩
    );
    
    // --- ���Z�b�g ---
    void ResetAnimation();


    // --- ���[���h���W����X�N���[�����W�ɕύX��`�� ---
    static DirectX::XMFLOAT2 ConvertToScreenPos(const DirectX::XMFLOAT3 worldPos, bool* isDraw = nullptr); // isDraw�F�`�悷�邩

    // --- �U�� ---
    void Vibration(const float& elapsedTime, const float& volume, const float& breakTime);

    struct Vertex
    {
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT4 color_;
        DirectX::XMFLOAT2 texcoord_;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return shaderResourceView_; }

public:// �擾�E�ݒ�
    // --- Transform ---
    Transform* GetTransform() { return &transform_; }

    const char* GetName() { return name_.c_str(); }
    void SetName(const char* n) { name_ = n; }

private:
    void Rotate(float& x, float& y, const float& centerX, const float& centerY, const float& angle);


private:
    // ---------- Transform ----------
    Transform transform_;

    // ---------- Animation ----------
    float   animationTime_ = 0.0f;
    float   animationFrame_ = 0.0f;
    //bool    animationLoopFlag_ = false;
    //bool    animationEndFlag_ = false;

    // ---------- �U�� ----------
    DirectX::XMFLOAT2 oldVibration_ = {};
    float vibrationBreakTimer_ = 0.0f;

    // ---------- �V�F�[�_�[ ----------
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>           inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView_;

    // ---------- ImGui�p ----------
    static int  nameNum_;
    std::string name_;
};
