#pragma once
#include <DirectXMath.h>

// ----- Zp‰‰Zq -----
#pragma region XMFLOAT3
// ----- float3 + float3 -----
inline const DirectX::XMFLOAT3 operator+(
    const DirectX::XMFLOAT3& float3_1,
    const DirectX::XMFLOAT3& float3_2)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3_1.x + float3_2.x,
        float3_1.y + float3_2.y,
        float3_1.z + float3_2.z,
    };

    return result;
}

// ----- float3 + float -----
inline const DirectX::XMFLOAT3 operator+(
    const DirectX::XMFLOAT3& float3,
    const float f)
{
    const DirectX::XMFLOAT3 result =
    {
        float3.x + f,
        float3.y + f,
        float3.z + f,
    };

    return result;
}

// ----- float3 - float3 -----
inline const DirectX::XMFLOAT3 operator-(
    const DirectX::XMFLOAT3& float3_1,
    const DirectX::XMFLOAT3& float3_2)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3_1.x - float3_2.x,
        float3_1.y - float3_2.y,
        float3_1.z - float3_2.z,
    };

    return result;
}

// ----- float3 - float -----
inline const DirectX::XMFLOAT3 operator-(
    const DirectX::XMFLOAT3& float3,
    const float f)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3.x - f,
        float3.y - f,
        float3.z - f,
    };

    return result;
}

// ----- float3 * float3 -----
inline const DirectX::XMFLOAT3 operator*(
    const DirectX::XMFLOAT3& float3_1,
    const DirectX::XMFLOAT3& float3_2)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3_1.x * float3_2.x,
        float3_1.y * float3_2.y,
        float3_1.z * float3_2.z,
    };

    return result;
}

// ----- float3 * float -----
inline const DirectX::XMFLOAT3 operator*(
    const DirectX::XMFLOAT3& float3,
    const float f)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3.x * f,
        float3.y * f,
        float3.z * f,
    };

    return result;
}

// ----- float3 / float3 -----
inline const DirectX::XMFLOAT3 operator/(
    const DirectX::XMFLOAT3& float3_1,
    const DirectX::XMFLOAT3& float3_2)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3_1.x / float3_2.x,
        float3_1.y / float3_2.y,
        float3_1.z / float3_2.z,
    };

    return result;
}

// ----- float3 / float -----
inline const DirectX::XMFLOAT3 operator/(
    const DirectX::XMFLOAT3& float3,
    const float f)
{
    const DirectX::XMFLOAT3 result = 
    {
        float3.x / f,
        float3.y / f,
        float3.z / f,
    };

    return result;
}

#pragma endregion XMFLOAT3

// ----- ZpŠÖ” -----
#pragma region XMFLOAT3

// ----- XMFLOAT3“¯m‚Ì“àÏ‚ğŒvZ -----
inline const float XMFloat3Dot(
    const DirectX::XMFLOAT3& v1,
    const DirectX::XMFLOAT3& v2)
{
    return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

// ----- XMFLOAT3“¯m‚Ì‹——£‚ğŒvZ -----
inline const float XMFloat3Length(const DirectX::XMFLOAT3& v)
{
    return ::sqrtf(XMFloat3Dot(v, v));
}

// ----- XMFLOAT3“¯m‚Ì‹——£‚Ì“ñæ‚ğŒvZ -----
inline const float XMFloat3LengthSq(const DirectX::XMFLOAT3& v)
{
    return XMFloat3Dot(v, v);
}

// ----- XMFLOAT3‚Ì³‹K‰» ( ’PˆÊƒxƒNƒgƒ‹‰» ) -----
inline const DirectX::XMFLOAT3 XMFloat3Normalize(const DirectX::XMFLOAT3& v)
{
    const float length = XMFloat3Length(v);

    if (length <= 0.0f) return DirectX::XMFLOAT3(0, 0, 0);

    return (v / length);
}

// ----- XMFLOAT3“¯m‚ÌŠOÏ‚ğŒvZ ( ‡”Ô‚É’ˆÓ ) -----
inline const DirectX::XMFLOAT3 XMFloat3Cross(
    const DirectX::XMFLOAT3& v1,
    const DirectX::XMFLOAT3& v2)
{
    const DirectX::XMFLOAT3 cross =
    {
        (v1.y * v2.z) - (v1.z * v2.y),
        (v1.z * v2.x) - (v1.x * v2.z),
        (v1.x * v2.y) - (v1.y * v2.x),
    };

    return cross;
}

#pragma endregion XMFLOAT3


#pragma region XMFLOAT2
// ----- float2 + float2 -----
inline const DirectX::XMFLOAT2 operator+(
    const DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    const DirectX::XMFLOAT2 result =
    {
        float2_1.x + float2_2.x,
        float2_1.y + float2_2.y,
    };

    return result;
}

// ----- float2 - float2 -----
inline const DirectX::XMFLOAT2 operator-(
    const DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    const DirectX::XMFLOAT2 result =
    {
        float2_1.x - float2_2.x,
        float2_1.y - float2_2.y,
    };

    return result;
}

// ----- float2 * float2 -----
inline const DirectX::XMFLOAT2 operator*(
    const DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    const DirectX::XMFLOAT2 result =
    {
        float2_1.x * float2_2.x,
        float2_1.y * float2_2.y,
    };

    return result;
}

// ----- float2 * float -----
inline const DirectX::XMFLOAT2 operator*(
    const DirectX::XMFLOAT2& float2,
    const float& f)
{
    const DirectX::XMFLOAT2 result =
    {
        float2.x * f,
        float2.y * f,
    };

    return result;
}

// ----- float2 / float2 -----
inline const DirectX::XMFLOAT2 operator/(
    const DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    const DirectX::XMFLOAT2 result =
    {
        float2_1.x / float2_2.x,
        float2_1.y / float2_2.y,
    };

    return result;
}

// ----- float2 / float -----
inline const DirectX::XMFLOAT2 operator/(
    const DirectX::XMFLOAT2& float2,
    const float f)
{
    const DirectX::XMFLOAT2 result = 
    {
        float2.x / f,
        float2.y / f,
    };

    return result;
}

#pragma endregion XMFLOAT2

#pragma region XMFLOAT2
// ----- XMFLOAT2“¯m‚Ì“àÏ‚ğŒvZ -----
inline const float XMFloat2Dot(
    const DirectX::XMFLOAT2& v1,
    const DirectX::XMFLOAT2& v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

// ----- XMFLOAT2“¯m‚Ì‹——£‚ğŒvZ -----
inline const float XMFloat2Length(const DirectX::XMFLOAT2& v)
{
    return ::sqrtf(XMFloat2Dot(v, v));
}

// ----- XMFLOAT2‚Ì³‹K‰» ( ’PˆÊƒxƒNƒgƒ‹‰» ) -----
inline const DirectX::XMFLOAT2 XMFloat2Normalize(const DirectX::XMFLOAT2& v)
{
    const float length = XMFloat2Length(v);

    if (length <= 0.0f) return DirectX::XMFLOAT2(0, 0);

    return (v / length);
}

// ----- XMFLOAT2“¯m‚ÌŠOÏ‚ğŒvZ ( ‡”Ô‚É’ˆÓ ) -----
inline const float XMFloat2Cross(
    const DirectX::XMFLOAT2& v1,
    const DirectX::XMFLOAT2& v2)
{
    return (v1.y * v2.x) - (v1.x * v2.y);
}

#pragma endregion XMFLOAT2