struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    uint4 joints[2] : JOINTS;
    float4 weights[2] : WEIGHTS;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 wPosition : POSITION;
    float4 wNormal : NORMAL;
    float4 wTangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

cbuffer PRIMITIVE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    int material;
    bool hasTangent;
    int skin;
    int step_;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    row_major float4x4 lightViewProjection;
};


struct TextureInfo
{
    int index;
    int texcoord;
};

struct NormalTextureInfo
{
    int index;
    int texcoord;
    float scale;
};

struct OcclusionTextureInfo
{
    int index;
    int texcoord;
    float strength;
};

struct PbrMetallicRoughness
{
    float4 baseColorFactor;
    TextureInfo baseColorTexture;
    float metallicFactor;
    float roughnessFactor;
    TextureInfo metallicRoughnessTexture;
};

struct MaterialConstants
{
    float3 emissiveFactor;
    int alphaMode; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
    float alphaCutoff;
    bool doubleSided;

    PbrMetallicRoughness pbrMetallicRoughness;

    NormalTextureInfo normalTexture;
    OcclusionTextureInfo occlusionTexture;
    TextureInfo emissiveTexture;
};
StructuredBuffer<MaterialConstants> materials :register(t0);

static const uint PRIMITIVE_MAX_JOINTS = 512;
cbuffer PRIMITIVE_JOINT_CONSTANTS : register(b2)
{
    row_major float4x4 jointMatrices[PRIMITIVE_MAX_JOINTS];
}