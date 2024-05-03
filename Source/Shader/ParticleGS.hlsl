#include "Particle.hlsli"

StructuredBuffer<Particle> particleBuffer : register(t9);

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    const float2 corners[] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f, +1.0f),
        float2(+1.0f, -1.0f),
        float2(+1.0f, +1.0f),
    };
    const float2 texcoords[] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };

    Particle p = particleBuffer[input[0].vertexId];
    
    const float aspectRatio = 1280.0 / 720.0;
    float2 particleScale = float2(particleSize, particleSize * aspectRatio);
    
    [unroll]
    for (uint vertexIndex = 0; vertexIndex < 4;++vertexIndex)
    {
        GS_OUT element;
        
        // Transform to clip space
        element.position = mul(float4(p.position, 1), viewProjection);
        // Make corner position as billboard
        element.position.xy += corners[vertexIndex] * particleScale;
        
        element.color = p.color;
        element.texcoord = texcoords[vertexIndex];
        
        element.scrollTexcoord = texcoords[vertexIndex] + scrollDirection * options.xx;

        
        output.Append(element);
    }
    
    output.RestartStrip();
}