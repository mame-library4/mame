#include "TailParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float3x3 TranposeFloat3x3(float3x3 m)
{
    float3x3 ret;
    ret._11_12_13 = m._11_21_31;
    ret._21_22_23 = m._12_22_32;
    ret._31_32_33 = m._13_23_33;
    return ret;
   

}

float3x3 AngleAxis3X3(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);
    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    
    
    float3x3 ret = float3x3(
        t * x * x + c, t * x * y - s * z, t * x * z + s * y,
        t * x * y + s * z, t * y * y + c, t * y * z - s * x,
        t * x * z - s * y, t * y * z + s * x, t * z * z + c
    );
    return TranposeFloat3x3(ret);
    //return float3x3(
    //    t * x * x + c, t * x * y - s * z, t * x * z + s * y,
    //    t * x * y + s * z, t * y * y + c, t * y * z - s * x,
    //    t * x * z - s * y, t * y * z + s * x, t * z * z + c
    //);

}

float3 GetCylindricalPosition(float particleID, float raidus, float height, float3 vec)
{
#if 0
    // ランダムな距離を生成
    float d = frac(sin(particleID * 12.9898) * 43758.5453);
    d = clamp(d, 0.0f, 1.0f);
    float3 position = normalize(vec) * d * height;
    
    float3 up = float3(0, 1, 0);
    float3 corssVec = normalize(cross(vec, up));
    float a = (int) particleID % 2 == 0 ? 1.0f : -1.0f;
    
    float x = frac(sin(d * 12.9898) * 43758.5453);
    float y = frac(sin(x * 12.9898) * 43758.5453);
    float z = frac(sin(y * 12.9898) * 43758.5453);
    position += corssVec * a * normalize(float3(x, y, z)) * raidus;
    
    
    return position;
    
#else
    // ランダムな角度を生成
    float angle = frac(sin(particleID * 12.9898) * 43758.5453) * 2.0 * 3.14159265359;
    
    // XZ平面上の位置
    float x = cos(angle) * raidus;
    float z = sin(angle) * raidus;
    
    // ランダムな高さを生成
    float y = frac(sin(particleID * 78.233 + 0.1234) * 43758.5453) * height;
    
    float3 axis = cross(float3(0, 1, 0), direction_);
    float UdD = dot(float3(0, 1, 0), direction_);
    float rotation_angle = acos(UdD);
    row_major float3x3 rotation_matrix = AngleAxis3X3(rotation_angle, axis);
    float3 position = mul(float3(x, y, z), rotation_matrix);
    return position;
    
    return float3(x, y, z);
#endif
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    ParticleData p = particleBuffer[id];
    
    //p.tailIndex_ = id % MaxTailNum;
    p.tailIndex_ = 0;
    
    //float f0 = frac(sin(id) * 43758.5453123);
    //float f1 = frac(sin(id * 1.2345) * 43758.5453123);
    
    //p.randomOffset_.x = sin(f0);
    //p.randomOffset_.y = sin(f1) * cos(f0);
    //p.randomOffset_.z = cos(f0);
    
    float3 initialPosition = GetCylindricalPosition(id, radius_, height_, direction_);    
    p.randomOffset_ = initialPosition;
    
    
    
    p.color_ = particleColor_;
    
    particleBuffer[id] = p;
}