#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>

// ----- Animation�̂��� -----
// PlayAnimation�֐����Ăяo��
// PlayAnimation( �t���[���o�ߎ���, �A�j���[�V�������鑬��,
//      �A�j���[�V�����t���[����, �c�A�j���[�V�����̏ꍇtrue);

// ----- Dissolve�̎g���� ------
// Render�����Ƀf�B�]���u�p�̃s�N�Z���V�F�[�_�[���Z�b�g����B
// (./Resources/Shader/sprite_dissolve_ps.cso)������create����psShader
// --- Fade�̎g���� ---
// Update�֐����Ăяo���B
// spriteDissolve.dissolveType  (0: FadeIn, 1: FadeOut)
// Fade�������Ƃ��� isFade ��true�ɂ��� ( SetIsFade(true) )

class Sprite
{
public:
    Sprite(ID3D11Device* device, const wchar_t* filename);
    ~Sprite();

private:// Transform �֘A ( �\���� )
#pragma region [�e��Transform] struct
    struct SpriteTransform
    {
    private:
        DirectX::XMFLOAT2 position_ = {};        // �ʒu
        DirectX::XMFLOAT2 size_ = { 100,100 };   // �傫��
        DirectX::XMFLOAT4 color_ = { 1,1,1,1 };  // �F
        float angle_ = 0.0f;                     // �p�x
        DirectX::XMFLOAT2 texPos_ = {};          // �e�N�X�`�����W
        DirectX::XMFLOAT2 texSize_ = { 100,100 };// �e�N�X�`���T�C�Y

        // ImGui�p
        bool isSizeFactor_ = false;

    public:
        // ImGui�p
        void DrawDebug();

        // ----- �ʒu -----
#pragma region �ʒu ( position )
        // --- Get ---
        DirectX::XMFLOAT2 GetPos() { return position_; }
        float GetPosX() { return position_.x; }
        float GetPosY() { return position_.y; }

        // --- Set ---
        void SetPos(const DirectX::XMFLOAT2& pos) { position_ = pos; }
        void SetPos(const float& x, const float& y) 
        {
            position_.x = x;
            position_.y = y;
        }
        void SetPosX(const float& posX) { position_.x = posX; }
        void SetPosY(const float& posY) { position_.y = posY; }

        // --- Add ---
        void AddPos(const DirectX::XMFLOAT2& pos)
        {
            position_.x += pos.x;
            position_.y += pos.y;
        }
        void AddPos(const float& posX, const float& posY)
        {
            position_.x += posX;
            position_.y += posY;
        }
        void AddPosX(const float& posX) { position_.x += posX; }
        void AddPosY(const float& posY) { position_.y += posY; }
        
        // --- Subtract ---
        void SubtractPos(const DirectX::XMFLOAT2& pos)
        {
            position_.x -= pos.x;
            position_.y -= pos.y;
        }
        void SubtractPos(const float& posX, const float& posY)
        {
            position_.x -= posX;
            position_.y -= posY;
        }
        void SubtractPosX(const float& posX) { position_.x -= posX; }
        void SubtractPosY(const float& posY) { position_.y -= posY; }


        // �����̈ʒu���摜�̒��S�Ƃ��āA�摜�̈ʒu��ݒ肷��
        void SetSpriteCenterPos(DirectX::XMFLOAT2 pos)
        {
            pos.x -= GetSizeX() / 2;
            pos.y -= GetSizeY() / 2;
            position_ = pos;
        }
        void SetSpriteCenterPos(float x, float y)
        {
            x -= GetSizeX() / 2;
            y -= GetSizeY() / 2;
            position_ = { x, y };
        }

        // �����̈ʒu���摜�̍���Ƃ����Ƃ��́A�摜�̈ʒu��ݒ肷��
        // ���ʏ�̐ݒ�̊��o�Ŏg�������ꍇ���������g�����Ƃ������߂���
        void SetSpriteCenterPos(float x, float y, float size)
        {
            x += size / 2;
            y += size / 2;
            x -= GetSizeX() / 2;
            y -= GetSizeY() / 2;
            position_ = { x, y };
        }
        
#pragma endregion// �ʒu ( position )

        // ----- �傫�� -----
#pragma region �傫�� ( size )
        // --- Get ---
        DirectX::XMFLOAT2 GetSize() { return size_; }
        float GetSizeX() { return size_.x; }
        float GetSizeY() { return size_.y; }

        // --- Set ---
        void SetSize(const DirectX::XMFLOAT2& size) { size_ = size; }
        void SetSize(const float& x, const float& y)
        {
            size_.x = x;
            size_.y = y;
        }
        void SetSizeX(const float& sizeX) { size_.x = sizeX; }
        void SetSizeY(const float& sizeY) { size_.y = sizeY; }
        void SetSize(const float& size) { size_ = { size,size }; }

#pragma endregion// �傫�� ( size )

        // ----- �F -----
#pragma region �F ( color )
        // --- Get ---
        DirectX::XMFLOAT4 GetColor() { return color_; }
        float GetColorR() { return color_.x; }
        float GetColorG() { return color_.y; }
        float GetColorB() { return color_.z; }
        float GetColorA() { return color_.w; }

        // --- Set ---
        void SetColor(const DirectX::XMFLOAT4& color) { color_ = color; }
        void SetColor(const float& r, const float& g, const float& b, const float& a)
        {
            color_.x = r;
            color_.y = g;
            color_.z = b;
            color_.w = a;
        }
        void SetColor(const float& r, const float& g, const float& b)
        {
            color_.x = r;
            color_.y = g;
            color_.z = b;
        }
        void SetColorR(const float& r) { color_.x = r; }
        void SetColorG(const float& g) { color_.y = g; }
        void SetColorB(const float& b) { color_.z = b; }
        void SetColorA(const float& a) { color_.w = a; }
        void SetColorWhite() { color_ = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, color_.w); }
        void SetColorBlack() { color_ = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, color_.w); }

#pragma endregion// �F ( color )

        // ----- �p�x -----
#pragma region �p�x ( angle )
        // --- Get ---
        float GetAngle() { return angle_; }

        // --- Set ---
        void SetAngle(const float& angle) { angle_ = angle; }

        // --- Add ---
        void AddAngle(const float& angle) { angle_ += angle; }

#pragma endregion// �p�x ( angle )

        // ----- �e�N�X�`�����W -----
#pragma region �e�N�X�`�����W ( texPos )
        // --- Get ---
        DirectX::XMFLOAT2 GetTexPos() { return texPos_; }
        float GetTexPosX() { return texPos_.x; }
        float GetTexPosY() { return texPos_.y; }

        // --- Set ---
        void SetTexPos(const DirectX::XMFLOAT2& texPos) { texPos_ = texPos; }
        void SetTexPos(const float& texPosX, const float& texPosY)
        {
            texPos_.x = texPosX;
            texPos_.y = texPosY;
        }
        void SetTexPosX(const float& texPosX) { texPos_.x = texPosX; }
        void SetTexPosY(const float& texPosY) { texPos_.y = texPosY; }
#pragma endregion// �e�N�X�`�����W ( texPos )

        // ----- �e�N�X�`���T�C�Y -----
#pragma region �e�N�X�`���T�C�Y ( texSize )
        // --- Get ---
        DirectX::XMFLOAT2 GetTexSize() { return texSize_; }
        float GetTexSizeX() { return texSize_.x; }
        float GetTexSizeY() { return texSize_.y; }

        // --- Set ---
        void SetTexSize(const DirectX::XMFLOAT2& texSize) { texSize_ = texSize; }
        void SetTexSize(const float& texSizeX, const float& texSizeY)
        {
            texSize_.x = texSizeX;
            texSize_.y = texSizeY;
        }
        void SetTexSizeX(const float& texSizeX) { texSize_.x = texSizeX; }
        void SetTexSizeY(const float& texSizeY) { texSize_.y = texSizeY; }

#pragma endregion// �e�N�X�`���T�C�Y ( texSize )
    };

    // �����͉��ǂ̗]�n����
    struct SpriteDissolve
    {
    private:
        int maskTextureValue = 0;           // �e�N�X�`���ԍ�
        float dissolveValue = 0.0f;         // �f�B�]���u�K����
        float dissolveBlackValue = 0.0f;    // ���F

        float edgeThreshold = 0.1f; // ����臒l
        DirectX::XMFLOAT4 edgeColor = { 1.0f, 1.0f, 0.0f, 1.0f }; // ���̐F

        float delay = 0.4f;
        int dissolveType = 0;

    public:
        void DrawDebug();

        void AddDissolveBlackValue(float value) { dissolveBlackValue += value; }
        void SubtractDissolveBlackValue(float value) { dissolveBlackValue -= value; }

        void SetMaskTextureValue(int value) { maskTextureValue = value; }
        void SetDissolveValue(float value) { dissolveValue = value; }
        void AddDissolveValue(float value) { dissolveValue += value; }
        void SetDissolveBlackValue(float value) { dissolveBlackValue = value; }
        void SetEdgeThreshold(float threshold) { edgeThreshold = threshold; }
        void SetEdgeColor(DirectX::XMFLOAT4 color) { edgeColor = color; }
        void SetDelay(float d) { delay = d; }
        void SetDissolveType(int type) { dissolveType = type; } // (0:FadeIn,1:FadeOut)

        int GetMaskTextureValue() { return maskTextureValue; }
        float GetDissolveValue() { return dissolveValue; }
        float GetDissolveBlackValue() { return dissolveBlackValue; }
        float GetEdgeThreshold() { return edgeThreshold; }
        DirectX::XMFLOAT4 GetEdgeColor() { return edgeColor; }
        float GetDelay() { return delay; }
        int GetDissolveType() { return dissolveType; }
    };

    struct Emissive
    {
    private:
        DirectX::XMFLOAT4 emissiveColor_ = { 1.0f, 1.0f, 1.0f,1.0f };
        float emissiveIntensity_ = 1.0f;
    public:
        void DrawDebug();

        void SetEmissiveColor(DirectX::XMFLOAT4 color) { emissiveColor_ = color; }
        DirectX::XMFLOAT4 GetEmissiveColor() { return emissiveColor_; }

        void SetEmissiveIntensity(float intensity) { emissiveIntensity_ = intensity; }
        float GetEmissiveIntensity() { return emissiveIntensity_; }
    };

#pragma endregion// [�e��Transform] struct

public:    
    void Initialize();
    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader = nullptr, const char* type = "");
    void DrawDebug();

    void SetConstantBuffer(const char* type);

    // ---------- Animation ----------
#pragma region [Animation] Function
    // --- �X�V ---
    bool PlayAnimation(
        const float elapsedTime,                // �o�ߎ���
        const float frameSpeed,                 // �A�j���[�V�������x
        const float totalAnimationFrame,        // �A�j���[�V�����t���[����
        const bool animationVertical = false,   // �c�A�j���[�V�����̏ꍇtrue
        const bool loop = false                 // ���[�v���邩
    );
    
    // --- ���Z�b�g ---
    void ResetAnimation()
    {
        spriteTransform_.SetTexPos(DirectX::XMFLOAT2(0, 0));
        animationTime_ = 0.0f;
        animationFrame_ = 0.0f;
    }
#pragma endregion// [Animation] Function

    // ---------- Dissolve ----------
#pragma region [Dissolve] Function
    void UpdateSpriteDissolve(const float& elapsedTime);
    bool FadeIn(const float& elapsedTime);
    bool FadeOut(const float& elapsedTime);
#pragma endregion// [Dissolve] Function

    // --- ���[���h���W����X�N���[�����W�ɕύX��`�� ---
    static DirectX::XMFLOAT2 ConvertToScreenPos(const DirectX::XMFLOAT3 worldPos, bool* isDraw = nullptr); // isDraw�F�`�悷�邩

    // --- �U�� ---
    void Vibration(const float& elapsedTime, const float& volume, const float& breakTime);

public:// �擾�E�ݒ�
#pragma region [Get, Set] Function
    // --- Transform ---
    SpriteTransform* GetSpriteTransform() { return &spriteTransform_; }
    SpriteDissolve* GetSpriteDissolve() { return &spriteDissolve_; }
    Emissive* GetEmissive() { return &emissive_; }

    // --- Dissolve ---
    void SetIsFade(bool fade) { isFade_ = fade; }
    bool GetIsFade() { return isFade_; }

#pragma endregion// [Get, Set] Function

private:// �������������Ŋ�������֐�
    void Render(ID3D11DeviceContext* deviceContext, ID3D11PixelShader* psShader);

public:// SkyBox���Ŏg������public
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcord;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return shaderResourceView; }

private:// �����o�ϐ�
    // ---------- Transform ----------
    SpriteTransform spriteTransform_;   // Transform
    SpriteDissolve spriteDissolve_;     // Dissolve
    Emissive emissive_;                 // Emissive

    // ---------- Animation ----------
    float animationTime_ = 0.0f;
    float animationFrame_ = 0.0f;

    // ---------- Dissolve ----------
    bool isFade_ = false;

    // ---------- �U�� ----------
    DirectX::XMFLOAT2 oldVibration_ = {};
    float vibrationBreakTimer_ = 0.0f;

    // ---------- �V�F�[�_�[ ----------
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    D3D11_TEXTURE2D_DESC texture2dDesc;

    // --- Dissolve ---
    struct DissolveConstants
    {
        DirectX::XMFLOAT4 parameters = {};  // x: �f�B�]���u�K����
        // y: ���F
        // z: ����臒l
        // w: ��
        DirectX::XMFLOAT4 edgeColor = {};   // ���̐F
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> dissolveConstantBuffer = nullptr;

#pragma region ImGui
public:// ----- ImGui�p ----- //
    static int nameNum;
    const char* GetName() { return name_.c_str(); }
    void SetName(const char* n) { name_ = n; }

private:// ----- ImGui�p ----- //
    std::string name_;
#pragma endregion// ImGui
};
