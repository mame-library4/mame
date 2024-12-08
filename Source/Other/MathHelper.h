#pragma once
#include <DirectXMath.h>
#include <algorithm>

// =============================================
//                  éZèpââéZéq
// =============================================
#pragma region XMFLOAT4
// ----- float4 * float -----
inline const DirectX::XMFLOAT4 operator*(
    const DirectX::XMFLOAT4& float4,
    const float& f)
{
    const DirectX::XMFLOAT4 result =
    {
        float4.x * f,
        float4.y * f,
        float4.z * f,
        float4.w * f,
    };

    return result;
}

// ----- float4 / float -----
inline const DirectX::XMFLOAT4 operator/(
    const DirectX::XMFLOAT4& float4,
    const float& f)
{
    const DirectX::XMFLOAT4 result =
    {
        float4.x / f,
        float4.y / f,
        float4.z / f,
        float4.w / f,
    };

    return result;
}

#pragma endregion XMFLOAT4

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
    const float& f)
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
    const float& f)
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
    const float& f)
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
    const float& f)
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

// ----- float2 += float2 -----
inline const DirectX::XMFLOAT2 operator +=(
    DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    float2_1.x += float2_2.x;
    float2_1.y += float2_2.y;

    return float2_1;
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

// ----- float2 -= float2 -----
inline const DirectX::XMFLOAT2 operator-=(
    DirectX::XMFLOAT2& float2_1,
    const DirectX::XMFLOAT2& float2_2)
{
    float2_1.x -= float2_2.x;
    float2_1.y -= float2_2.y;

    return float2_1;
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
    const float& f)
{
    const DirectX::XMFLOAT2 result =
    {
        float2.x / f,
        float2.y / f,
    };

    return result;
}

#pragma endregion XMFLOAT2


// =============================================
//                  éZèpä÷êî
// =============================================
#pragma region XMFLOAT3

// ----- XMFLOAT3ìØémÇÃì‡êœÇåvéZ -----
inline const float XMFloat3Dot(
    const DirectX::XMFLOAT3& v1,
    const DirectX::XMFLOAT3& v2)
{
    return ((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
}

// ----- XMFLOAT3ìØémÇÃãóó£ÇåvéZ -----
inline const float XMFloat3Length(const DirectX::XMFLOAT3& v)
{
    return ::sqrtf(XMFloat3Dot(v, v));
}

// ----- XMFLOAT3ìØémÇÃãóó£ÇÃìÒèÊÇåvéZ -----
inline const float XMFloat3LengthSq(const DirectX::XMFLOAT3& v)
{
    return XMFloat3Dot(v, v);
}

// ----- XMFLOAT3ÇÃê≥ãKâª ( íPà ÉxÉNÉgÉãâª ) -----
inline const DirectX::XMFLOAT3 XMFloat3Normalize(const DirectX::XMFLOAT3& v)
{
    const float length = XMFloat3Length(v);

    if (length <= 0.0f) return DirectX::XMFLOAT3(0, 0, 0);

    return (v / length);
}

// ----- XMFLOAT3ìØémÇÃäOêœÇåvéZ ( èáî‘Ç…íçà” ) -----
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
// ----- XMFLOAT2ìØémÇÃì‡êœÇåvéZ -----
inline const float XMFloat2Dot(
    const DirectX::XMFLOAT2& v1,
    const DirectX::XMFLOAT2& v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y);
}

// ----- XMFLOAT2ìØémÇÃãóó£ÇåvéZ -----
inline const float XMFloat2Length(const DirectX::XMFLOAT2& v)
{
    return ::sqrtf(XMFloat2Dot(v, v));
}

// ----- XMFLOAT2ÇÃê≥ãKâª ( íPà ÉxÉNÉgÉãâª ) -----
inline const DirectX::XMFLOAT2 XMFloat2Normalize(const DirectX::XMFLOAT2& v)
{
    const float length = XMFloat2Length(v);

    if (length <= 0.0f) return DirectX::XMFLOAT2(0, 0);

    return (v / length);
}

// ----- XMFLOAT2ìØémÇÃäOêœÇåvéZ ( èáî‘Ç…íçà” ) -----
inline const float XMFloat2Cross(
    const DirectX::XMFLOAT2& v1,
    const DirectX::XMFLOAT2& v2)
{
    return (v1.y * v2.x) - (v1.x * v2.y);
}

#pragma endregion XMFLOAT2


inline const DirectX::XMFLOAT3 XMFloat3Lerp(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const float& timer)
{
    DirectX::XMFLOAT3 result = 
    {
        start.x + timer * (end.x - start.x),
        start.y + timer * (end.y - start.y),
        start.z + timer * (end.z - start.z)
    };

    return result;
}

inline const DirectX::XMFLOAT2 XMFloat2Lerp(
    const DirectX::XMFLOAT2& start,
    const DirectX::XMFLOAT2& end,
    const float& timer)
{
    DirectX::XMFLOAT2 result =
    {
        start.x + timer * (end.x - start.x),
        start.y + timer * (end.y - start.y),
    };

    return result;
}

inline const float XMFloatLerp(
    const float& start,
    const float& end,
    const float& timer)
{
    return start + timer * (end - start);
}

// Percentage of value between start and end.(0.0f~1.0f)
inline const float XMFloatInverseLerp(
    const float& start,
    const float& end,
    const float& value)
{
    float t = (value - start) / (end - start);
    return std::clamp(t, 0.0f, 1.0f);
}

inline const int XMIntLerp(
    const int& start,
    const int& end,
    const float& timer)
{
    return (int)(start + timer * (end - start));
}

inline const float XMFloatRandomRange(
    const float& min,
    const float& max)
{
    // 0.0 ~ 1.0ÇÃä‘Ç‹Ç≈ÇÃÉâÉìÉ_ÉÄíl
    float value = static_cast<float>(rand()) / RAND_MAX;
    
    // min ~ max Ç‹Ç≈ÇÃÉâÉìÉ_ÉÄílÇ…ïœä∑
    return min + (max - min) * value;
}