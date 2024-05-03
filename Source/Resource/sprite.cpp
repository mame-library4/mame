#include "sprite.h"
#include "../Other/misc.h"
#include <sstream>

#include "../../../External/DirectXTK-main/Inc/WICTextureLoader.h"

#include "texture.h"
#include "../Graphics/shader.h"
#include "../Graphics/Graphics.h"

#include "NoiseTexture.h"

#include "../Other/Easing.h"

//---ImGui名前かぶり防止用---//
int Sprite::nameNum = 0;
//---ImGui名前かぶり防止用---//

// コンストラクタ
Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };
    Graphics& graphics = Graphics::Instance();

    // Animation
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;

    // 頂点情報のセット
    Vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    // 定数バッファ生成
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

    // 入力レイアウトオブジェクトの生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // シェーダー関連
    CreateVsFromCso(device, "./Resources/Shader/sprite_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    CreatePsFromCso(device, "./Resources/Shader/sprite_ps.cso", pixelShader.GetAddressOf());

    // テクスチャのロード
    LoadTextureFromFile(device, filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

    // 画像サイズを設定
    GetSpriteTransform()->SetSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));
    GetSpriteTransform()->SetTexSize(DirectX::XMFLOAT2(texture2dDesc.Width, texture2dDesc.Height));

    //---ImGui名前かぶり防止用---//
    std::string name = "Sprite" + std::to_string(nameNum++);
    SetName(name.c_str());
    //---ImGui名前かぶり防止用---//
}

// デストラクタ
Sprite::~Sprite()
{
    --nameNum;
}

// 初期化
void Sprite::Initialize()
{
    animationTime_ = 0.0f;
    animationFrame_ = 0.0f;
}

void Sprite::Update(const float& elapsedTime)
{
    // ディゾルブ更新
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

    // いい感じにFadeになるように設定してる
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

// アニメーション関数
bool Sprite::PlayAnimation(const float elapsedTime, const float frameSpeed, const float totalAnimationFrame, const bool animationVertical, const bool loop)
{
    animationTime_ += frameSpeed * elapsedTime;

    const int frame = static_cast<int>(animationTime_); // 小数点切り捨て
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

            // ----- 一応この処理を書いておく -----
            DirectX::XMFLOAT2 texPos = GetSpriteTransform()->GetTexPos();
            const DirectX::XMFLOAT2 texSize = GetSpriteTransform()->GetTexSize();

            if (animationVertical) texPos.y = texSize.y * animationFrame_;
            else                   texPos.x = texSize.x * animationFrame_;
            // ----- 一応この処理を書いておく -----

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
    //ビューポート
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

// 描画
void Sprite::Render(ID3D11PixelShader* psShader, const char* type)
{
    Graphics& graphics = Graphics::Instance();
    Shader* shader = graphics.GetShader();
    
    // 何をセットするのかによって分ける
    SetConstantBuffer(type);

    // spriteDissolve
    NoiseTexture::Instance().SetConstantBuffers(1);

    // 定数バッファの更新
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

    // 描画
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


// 描画する本体の関数
void Sprite::Render(ID3D11DeviceContext* deviceContext, ID3D11PixelShader* psShader)
{
    HRESULT hr{ S_OK };

    // スクリーン（ビューポート）のサイズを取得する
    D3D11_VIEWPORT viewport{};
    UINT numViewports{ 1 };
    deviceContext->RSGetViewports(&numViewports, &viewport);

    // 短形の角頂点の位置（スクリーン座標系）を計算する
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

    // 矩形回転
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
    // 回転の中心を短形の中心点にした場合
    float cx = GetSpriteTransform()->GetPosX() + GetSpriteTransform()->GetSizeX() * 0.5f;
    float cy = GetSpriteTransform()->GetPosY() + GetSpriteTransform()->GetSizeY() * 0.5f;
    rotate(x0, y0, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x1, y1, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x2, y2, cx, cy, GetSpriteTransform()->GetAngle());
    rotate(x3, y3, cx, cy, GetSpriteTransform()->GetAngle());

    // スクリーン座標系からNDCへの座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
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
    //---これより下に何かいても意味ない---//
}

// debug用関数
void Sprite::DrawDebug()
{
    if (ImGui::BeginMenu(GetName()))
    {
        GetSpriteTransform()->DrawDebug();

        GetSpriteDissolve()->DrawDebug();

        GetEmissive()->DrawDebug();

        ImGui::DragFloat(u8"アニメーションフレーム", &animationFrame_);

        if (ImGui::Button("Fade"))SetIsFade(true);

        ImGui::EndMenu();
    }
}

// 振動
void Sprite::Vibration(const float& elapsedTime, const float& volume, const float& breakTime)
{
    if (volume <= 0) return; // 振動値がない場合は処理しない
    
    // 振動の間の時間を空けるための処理
    if (vibrationBreakTimer_ >= 0.0f)
    {   // breakTimeが残っているときは処理しない
        vibrationBreakTimer_ -= elapsedTime;
        return;
    }

    // breakTimeを設定
    vibrationBreakTimer_ = breakTime;

    // 前回の結果をリセットする
    GetSpriteTransform()->SubtractPos(oldVibration_);

    // 振動の幅、方向を算出する
    DirectX::XMFLOAT2 vibration = { (rand() % 100 - 50.0f), (rand() % 100 - 50.0f) };
    DirectX::XMVECTOR Vibration = DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&vibration));

    float vibrationVolume = volume * 1000 * elapsedTime;

    Vibration = DirectX::XMVectorScale(Vibration, vibrationVolume);
    DirectX::XMStoreFloat2(&vibration, Vibration);

    // 振動の値を入れる。
    GetSpriteTransform()->AddPos(vibration);

    // 現在の振動値を保存しておく
    oldVibration_ = vibration;
}

// SpriteTransform
void Sprite::SpriteTransform::DrawDebug()
{
    if (ImGui::TreeNode("spriteTransform"))
    {
        ImGui::DragFloat2(u8"位置", &position_.x); // test u8
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
