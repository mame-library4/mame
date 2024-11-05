#include "Particle.hlsli"
#include "RockSmokeParticle.hlsli"

StructuredBuffer<ParticleData> particleBuffer : register(t0);

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
    ParticleData p = particleBuffer[input[0].vertexId];
    
    const float2 corners[] =
    {
        float2(-1.0f, -1.0f),
		float2(-1.0f, +1.0f),
		float2(+1.0f, -1.0f),
		float2(+1.0f, +1.0f),
    };
    const float2 texcoords[] =
    {        
        float2(p.texPos_.x,                 p.texPos_.y + p.texSize_.y), // ç∂â∫
		float2(p.texPos_.x,                 p.texPos_.y),                // ç∂è„
		float2(p.texPos_.x + p.texSize_.x,  p.texPos_.y + p.texSize_.y), // âEâ∫
		float2(p.texPos_.x + p.texSize_.x,  p.texPos_.y),                // âEè„
    };	

    const float aspect_ratio = 1280.0 / 720.0;
    float2 particle_scale = float2(p.size_, p.size_ * aspect_ratio);

	[unroll]
    for (uint vertex_index = 0; vertex_index < 4; ++vertex_index)
    {
        GS_OUT element;

		// Transform to clip space
        element.position = mul(float4(p.position_, 1), viewProjection);
		// Make corner position as billboard		
        element.position.xy += corners[vertex_index] * particle_scale;

        element.color = p.color_;
        element.texcoord = texcoords[vertex_index];
		
        output.Append(element);
    }

    output.RestartStrip();
}
