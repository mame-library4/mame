#pragma once

#include <d3d11.h>
#include <wrl.h>

// �m�C�Y�摜��ێ����Ă����N���X
class NoiseTexture
{
private:
    NoiseTexture();
    ~NoiseTexture() {}

public:
    // �C���X�^���X�擾
    static NoiseTexture& Instance()
    {
        static NoiseTexture instance;
        return instance;
    }

    // constantBuffer set
    void SetConstantBuffers(int slot);

private:
    static const int maxNoiseTexture = 1;  // �ő喇��
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noiseTexture[maxNoiseTexture];
};

