float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

uint Hash(uint x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

float3 CalculateSpherePosition(float id)
{
    float theta = (Hash(id * 2) % 10000) / 10000.0f * 6.28318f; // 0 ~ 2*PI
    float phi = (Hash(id * 2 + 1) % 10000) / 10000.0f * 3.14159f; // 0 ~ PI
    
    float3 result = 0;
    
    result.x = sin(phi) * cos(theta);
    result.y = cos(phi);
    result.z = sin(phi) * sin(theta);
    
    return result;
}