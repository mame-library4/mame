#include "SnowParticle.hlsli"
#include "Particle.hlsli"

StructuredBuffer<ParticleData> particleBuffer : register(t9);

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
    //float2 particleScale = float2(particleSize, particleSize * aspectRatio);
    float2 particleScale = float2(particle.size_.x, particle.size_.y * aspectRatio);
    
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