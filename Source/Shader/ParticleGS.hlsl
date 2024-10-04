#include "SnowParticle.hlsli"
#include "Particle.hlsli"

StructuredBuffer<ParticleData> particleBuffer : register(t9);

float4x4 matrixScaling(float3 scale)
{
    float4x4 m =
    {
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    return m;
}

float4x4 matrixRotationRollPichYaw(float3 rotation)
{
    float cp, sp;
    float cy, sy;
    float cr, sr;
    sincos(rotation.x, sp, cp);
    sincos(rotation.y, sy, cy);
    sincos(rotation.z, sr, cr);
    
    float4x4 m;
    m._11 = cr * cy + sr * sp * sy;
    m._12 = sr * cp;
    m._13 = sr * sp * cy - cr * sy;
    m._14 = 0.0f;
    
    m._21 = cr * sp * sy - sr * cy;
    m._22 = cr * cp;
    m._23 = sr * sy + cr * sp * cy;
    m._24 = 0.0f;

    m._31 = cp * sy;
    m._32 = -sp;
    m._33 = cp * cy;
    m._34 = 0.0f;

    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 0.0f;
    m._44 = 1.0f;

    return m;
}

float4x4 matrixTranslation(float3 translation)
{
    float4x4 m =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        translation.x, translation.y, translation.z, 1.0f
    };
    
    return m;
}

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    ParticleData particle = particleBuffer[input[0].vertexId];
    
    const float2 corners[] =
    {
        float2(+1.0f, -1.0f),
        float2(+1.0f, +1.0f),
        float2(-1.0f, -1.0f),
        float2(-1.0f, +1.0f),
    };
    const float2 texcoords[] =
    {
        float2(0.0, 1.0),
		float2(0.0, 0.0),
		float2(1.0, 1.0),
		float2(1.0, 0.0),
    };
    
    
    
    const float aspectRatio = 1280.0 / 720.0;
    float particleSize = 1.0f;
    float2 particleScale = float2(particleSize, particleSize * aspectRatio);
    //float2 particleScale = float2(particle.size_.x, particle.size_.y * aspectRatio);
    
    [unroll]
    for (uint vertexIndex = 0; vertexIndex < 4;++vertexIndex)
    {
        GS_OUT element;
        
        // Transform to clip space
        element.position = mul(float4(particle.position_, 1), viewProjection);
        // Make corner position as billboard
        element.position.xy += corners[vertexIndex] * particleScale;
        
        element.color = particle.color_;
        element.texcoord = texcoords[vertexIndex];

        
        output.Append(element);
    }
    
    output.RestartStrip();
}