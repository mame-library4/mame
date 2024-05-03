#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>

// ----- Animationのやり方 -----
// PlayAnimation関数を呼び出す
// PlayAnimation( フレーム経過時間, アニメーションする速さ,
//      アニメーションフレーム数, 縦アニメーションの場合true);

// ----- Dissolveの使い方 ------
// Render引数にディゾルブ用のピクセルシェーダーをセットする。
// (./Resources/Shader/sprite_dissolve_ps.cso)こいつをcreateしたpsShader
// --- Fadeの使い方 ---
// Update関数を呼び出す。
// spriteDissolve.dissolveType  (0: FadeIn, 1: FadeOut)
// Fadeしたいときに isFade をtrueにする ( SetIsFade(true) )

class Sprite
{
public:
    Sprite(ID3D11Device* device, const wchar_t* filename);
    ~Sprite();

private:// Transform 関連 ( 構造体 )
#pragma region [各種Transform] struct
    struct SpriteTransform
    {
    private:
        DirectX::XMFLOAT2 position_ = {};        // 位置
        DirectX::XMFLOAT2 size_ = { 100,100 };   // 大きさ
        DirectX::XMFLOAT4 color_ = { 1,1,1,1 };  // 色
        float angle_ = 0.0f;                     // 角度
        DirectX::XMFLOAT2 texPos_ = {};          // テクスチャ座標
        DirectX::XMFLOAT2 texSize_ = { 100,100 };// テクスチャサイズ

        // ImGui用
        bool isSizeFactor_ = false;

    public:
        // ImGui用
        void DrawDebug();

        // ----- 位置 -----
#pragma region 位置 ( position )
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


        // 引数の位置を画像の中心として、画像の位置を設定する
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

        // 引数の位置を画像の左上としたときの、画像の位置を設定する
        // ※通常の設定の感覚で使いたい場合こっちを使うことをお勧めする
        void SetSpriteCenterPos(float x, float y, float size)
        {
            x += size / 2;
            y += size / 2;
            x -= GetSizeX() / 2;
            y -= GetSizeY() / 2;
            position_ = { x, y };
        }
        
#pragma endregion// 位置 ( position )

        // ----- 大きさ -----
#pragma region 大きさ ( size )
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

#pragma endregion// 大きさ ( size )

        // ----- 色 -----
#pragma region 色 ( color )
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

#pragma endregion// 色 ( color )

        // ----- 角度 -----
#pragma region 角度 ( angle )
        // --- Get ---
        float GetAngle() { return angle_; }

        // --- Set ---
        void SetAngle(const float& angle) { angle_ = angle; }

        // --- Add ---
        void AddAngle(const float& angle) { angle_ += angle; }

#pragma endregion// 角度 ( angle )

        // ----- テクスチャ座標 -----
#pragma region テクスチャ座標 ( texPos )
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
#pragma endregion// テクスチャ座標 ( texPos )

        // ----- テクスチャサイズ -----
#pragma region テクスチャサイズ ( texSize )
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

#pragma endregion// テクスチャサイズ ( texSize )
    };

    // ここは改良の余地あり
    struct SpriteDissolve
    {
    private:
        int maskTextureValue = 0;           // テクスチャ番号
        float dissolveValue = 0.0f;         // ディゾルブ適応量
        float dissolveBlackValue = 0.0f;    // 黒色

        float edgeThreshold = 0.1f; // 縁の閾値
        DirectX::XMFLOAT4 edgeColor = { 1.0f, 1.0f, 0.0f, 1.0f }; // 縁の色

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

#pragma endregion// [各種Transform] struct

public:    
    void Initialize();
    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader = nullptr, const char* type = "");
    void DrawDebug();

    void SetConstantBuffer(const char* type);

    // ---------- Animation ----------
#pragma region [Animation] Function
    // --- 更新 ---
    bool PlayAnimation(
        const float elapsedTime,                // 経過時間
        const float frameSpeed,                 // アニメーション速度
        const float totalAnimationFrame,        // アニメーションフレーム数
        const bool animationVertical = false,   // 縦アニメーションの場合true
        const bool loop = false                 // ループするか
    );
    
    // --- リセット ---
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

    // --- ワールド座標からスクリーン座標に変更後描画 ---
    static DirectX::XMFLOAT2 ConvertToScreenPos(const DirectX::XMFLOAT3 worldPos, bool* isDraw = nullptr); // isDraw：描画するか

    // --- 振動 ---
    void Vibration(const float& elapsedTime, const float& volume, const float& breakTime);

public:// 取得・設定
#pragma region [Get, Set] Function
    // --- Transform ---
    SpriteTransform* GetSpriteTransform() { return &spriteTransform_; }
    SpriteDissolve* GetSpriteDissolve() { return &spriteDissolve_; }
    Emissive* GetEmissive() { return &emissive_; }

    // --- Dissolve ---
    void SetIsFade(bool fade) { isFade_ = fade; }
    bool GetIsFade() { return isFade_; }

#pragma endregion// [Get, Set] Function

private:// 内部処理だけで完結する関数
    void Render(ID3D11DeviceContext* deviceContext, ID3D11PixelShader* psShader);

public:// SkyBox等で使うためpublic
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcord;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return shaderResourceView; }

private:// メンバ変数
    // ---------- Transform ----------
    SpriteTransform spriteTransform_;   // Transform
    SpriteDissolve spriteDissolve_;     // Dissolve
    Emissive emissive_;                 // Emissive

    // ---------- Animation ----------
    float animationTime_ = 0.0f;
    float animationFrame_ = 0.0f;

    // ---------- Dissolve ----------
    bool isFade_ = false;

    // ---------- 振動 ----------
    DirectX::XMFLOAT2 oldVibration_ = {};
    float vibrationBreakTimer_ = 0.0f;

    // ---------- シェーダー ----------
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
    D3D11_TEXTURE2D_DESC texture2dDesc;

    // --- Dissolve ---
    struct DissolveConstants
    {
        DirectX::XMFLOAT4 parameters = {};  // x: ディゾルブ適応量
        // y: 黒色
        // z: 縁の閾値
        // w: 空き
        DirectX::XMFLOAT4 edgeColor = {};   // 縁の色
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> dissolveConstantBuffer = nullptr;

#pragma region ImGui
public:// ----- ImGui用 ----- //
    static int nameNum;
    const char* GetName() { return name_.c_str(); }
    void SetName(const char* n) { name_ = n; }

private:// ----- ImGui用 ----- //
    std::string name_;
#pragma endregion// ImGui
};
