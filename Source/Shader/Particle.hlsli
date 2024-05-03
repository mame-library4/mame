cbuffer PARTICLE_CONSTANTS : register(b9)
{
    float3 emitterPosition; // emitter: 発光素子
    float particleSize;
    float4 particleColor;
    float time;
    float deltaTime;
    
    float2 scrollDirection; // スクロール方向
    float options;          // タイム
    
    float2 dissolveParameters; // x : ディゾルブ適応量、 y : 縁の閾値
    float something; // ダミー
    
    float4 edgeColor;   // 縁の色
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
};

struct VS_OUT
{
    uint vertexId : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;    
    
    float2 scrollTexcoord : TEXCOORD;
};

struct Particle
{
    float4 color;
    float3 position;
    float3 velocity;
    float age;
    int state;
};

#define NUMTHREADS_X 16