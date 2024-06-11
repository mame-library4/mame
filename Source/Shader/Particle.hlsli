cbuffer PARTICLE_CONSTANTS : register(b9)
{
    float3 emitterPosition; // emitter: �����f�q
    float particleSize;
    float4 particleColor;
    
    float deltaTime;    
    float   animationSpeed;
    float2  size;
    
    float2  texSize;    // �摜�T�C�Y
    
    bool animationLoopFlag;
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
};

struct Particle
{
    float4 color;
    float3 position;
    float age;
    float3 velocity;
    int state;
    
    float2  size;           // �摜�S�̂̑傫��
    float2  texPos;         // �؂蔲���n�܂�
    float   animationTime;
    float3 dummy;
};

#define NUMTHREADS_X 16