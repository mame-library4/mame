#include "TailParticle.hlsli"

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 randomOffset_;
    float3 velocity_;
    float  size_;
    int tailIndex_;
    int state_;
};

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
}

float3 GetCylindricalPosition(float particleID, float raidus, float height, float3 vec)
{
    // ランダムな角度を生成
    float angle = frac(sin(particleID * 12.9898) * 43758.5453) * 2.0 * 3.14159265359;
    
    // XZ平面上の位置
    float x = cos(angle) * raidus;
    float z = sin(angle) * raidus;
    
    // ランダムな高さを生成
    float y = frac(sin(particleID * 78.233 + 0.1234) * 43758.5453) * height;
    
    float3 axis = cross(float3(0, 1, 0), vec);
    float UdD = dot(float3(0, 1, 0), vec);
    float rotation_angle = acos(UdD);
    row_major float3x3 rotation_matrix = AngleAxis3X3(rotation_angle, axis);
    float3 position = mul(float3(x, y, z), rotation_matrix);
    return position;
    
    return float3(x, y, z);
}