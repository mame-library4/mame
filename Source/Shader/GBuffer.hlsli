struct PSGBufferOut
{
    float4 baseColor_       : SV_TARGET0;
    float4 emissiveColor_   : SV_TARGET1;
    float4 normal_          : SV_TARGET2;
    float4 parameter_       : SV_TARGET3;
    float  depth_           : SV_TARGET4;
};

struct GBufferData
{
    float4 baseColor_;          // �x�[�X�J���[
    float3 emissiveColor_;      // ���Ȕ��s�F
    float3 worldNormal_;        // ���[���h�@��
    float3 worldPosition_;      // ���[���h���W
    float depth_;               // �[�x
    float roughness_;           // �e��
    float metalness_;           // ������
    float occlusionFactor_;     // �Օ��l
    float occlusionStrength_;   // �Օ����x
};

// GBuffer�e�N�X�`���󂯓n���p�\����
struct PSGBufferTextures
{
    Texture2D<float4> baseColor_;
    Texture2D<float4> emissiveColor_;
    Texture2D<float4> normal_;
    Texture2D<float4> parameter_;
    Texture2D<float> depth_;
    SamplerState state_;
};

PSGBufferOut EncodeGBuffer(in GBufferData data, matrix viewProjection)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    
    // �x�[�X�J���[�� baseColor RT0 �ɏo��
    ret.baseColor_ = data.baseColor_;
    
    // ���Ȕ��s�F�� emissiveColor RT1 �ɏo��
    ret.emissiveColor_.rgb = data.emissiveColor_;
    ret.emissiveColor_.a = 1;
    
    // �@���� normal RT2 �ɏo��
    ret.normal_.rgb = data.worldNormal_;
    ret.normal_.a = 1;
    
    // �e��/������/�Օ��� parameter RT3 �ɏo��
    ret.parameter_.r = data.occlusionFactor_;
    ret.parameter_.g = data.roughness_;
    ret.parameter_.b = data.metalness_;
    ret.parameter_.a = data.occlusionStrength_;
    
    // �[�x�� depth RT4 �ɏo��
    float4 position = mul(float4(data.worldPosition_, 1.0f), viewProjection);
    ret.depth_ = position.z / position.w;
    return ret;
}

// �s�N�Z���V�F�[�_�[�̏o�͗p�\���̂���GBufferData���ɕϊ�
GBufferData DecodeGBuffer(PSGBufferTextures textures, float2 uv, matrix inverseViewProjection)
{
    // �e�e�N�X�`����������擾
    float4 baseColor = textures.baseColor_.Sample(textures.state_, uv);
    float4 emissiveColor = textures.emissiveColor_.Sample(textures.state_, uv);
    float4 normal = textures.normal_.Sample(textures.state_, uv);
    float4 parameter = textures.parameter_.Sample(textures.state_, uv);
    float depth = textures.depth_.Sample(textures.state_, uv);
    
    GBufferData ret = (GBufferData) 0;
    
    ret.baseColor_ = baseColor;
    ret.emissiveColor_ = emissiveColor;
    ret.worldNormal_ = normal.rgb;
    ret.occlusionFactor_ = parameter.r;
    ret.roughness_ = parameter.g;
    ret.metalness_ = parameter.b;
    ret.occlusionStrength_ = parameter.a;
    ret.depth_ = depth;
    float4 position = float4(uv.x * 2.0f - 1.0f, uv.y * -2.0f + 1.0f, depth, 1);
    position = mul(position, inverseViewProjection);
    ret.worldPosition_ = position.xyz / position.w;
    return ret;
}