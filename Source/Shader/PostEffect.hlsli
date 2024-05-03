cbuffer POST_EFFECT_CONSTANT : register(b13)
{
    float4 shiftSize;
    
    float4 noiseColor;
    float noiseTimer;
    float scanLineTimer;
    
    float bokehAperture;
    float bokehFocus;    
    
    float vignetteValue;
    float3 dummydummy;
};


// noise
float WhiteNoise(float2 coord)
{
    return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}