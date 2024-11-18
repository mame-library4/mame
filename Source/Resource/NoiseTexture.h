#pragma once

#include <d3d11.h>
#include <wrl.h>

// ノイズ画像を保持しておくクラス
class NoiseTexture
{
private:
    NoiseTexture();
    ~NoiseTexture() {}

public:
    // インスタンス取得
    static NoiseTexture& Instance()
    {
        static NoiseTexture instance;
        return instance;
    }
    void PSSetShaderResourceView(const int& slot, const int& num);

private:
    static const int maxNoiseTexture = 4;  // 最大枚数
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noiseTexture[maxNoiseTexture];
};

