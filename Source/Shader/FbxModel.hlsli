#define POINT_LIGHT_ONE 1

struct VSIn
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    
    // LIM_LIGHT
    float3 normalInView : TEXCOORD2;
    
    // ZELDA
    float3 localPosition : LOCAL_POSITION;
    
    // EMISSIVE
    float2 emissiveTexcoord : TEXCOORD3;
};

// モデル情報
static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 materialColor;
    row_major float4x4 boneTransforms[MAX_BONES];
    
    // EMISSIVE
    float4 emissiveColor;
    float emissiveIntensity;
    float emissiveOptions;
    float2 emissiveScrollDirection;    
    
    float4 sageColor; // ZELDA
};

// カメラ位置、ディレクションライト情報
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    row_major float4x4 lightViewProjection;
};


// ディレクションライト
struct DirectionLight
{
    float4 direction; // ライトの方向
    float4 color; // ライトの色
};

// ポイントライト
struct PointLight
{
    float4 position; // ポイントライトの位置
    float4 color; // ポイントライトの色
    float range; // ポイントライトの影響範囲
    float3 step; // ダミー
};

// スポットライト
struct SpotLight
{
    float4 position; // スポットライトの位置
    float4 color; // スポットライトの色
    float range; // スポットライトの影響範囲
    float3 step; // ダミー
    float3 direction; // スポットライトの射出方向
    float angle; // スポットライトの射出角度
};

// 半球ライト
struct HemisphereLight
{
    float4 groundColor; // 照り返しのライト
    float4 skyColor; // 天球ライト
    float4 groundNormal; // 地面の法線
};

// ライト
cbuffer LIGHT_CONSTANT_BUFFER : register(b5)
{
    DirectionLight directionLig; // ディレクションライト
    
#if POINT_LIGHT_ONE
    PointLight pointLig;            // ポイントライト
#else
    PointLight pointLig[8];
#endif
    
    SpotLight spotLig; // スポットライト
    HemisphereLight hemisphereLig; // 半球ライト
}

cbuffer DISSOLVE_CONSTANT_BUFFER : register(b3)
{
    float4 parameters;
}

///////////////////////////////////
// ---------- 各種関数 ---------- //
///////////////////////////////////

// Lambert拡散反射光を計算する
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;
    
    // 内積の値を0以上の値にする
    t = max(0.0f, t);
    
    // 拡散反射光を計算する
    return lightColor * t;
}

// Phong鏡面反射光を計算する
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(lightDirection, normal);
    
    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = cameraPosition.xyz - worldPos;
    toEye = normalize(toEye);
    
    // 鏡面反射の強さを求める
    float t = dot(refVec, toEye);
    
    // 鏡面反射の強さを0以上にする
    t = max(0.0f, t);
    
    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);
    
    // 鏡面反射光を求める
    return lightColor * t;
}

// ディレクションライトによる反射光を計算
float3 CalcLightFromDirectionLight(PSIn psIn)
{
    // ディレクションライトによるLambert拡散反射光を計算する
    float3 diffuseDirection = CalcLambertDiffuse(
        directionLig.direction.xyz,
        directionLig.color.xyz,
        psIn.worldNormal.xyz
    );

    // ディレクションライトによるPhong鏡面反射光を計算する
    float3 specularDirection = CalcPhongSpecular(
        directionLig.direction.xyz,
        directionLig.color.xyz,
        psIn.worldPosition.xyz,
        psIn.worldNormal.xyz
    );
    
    // 拡散反射光を正規化する
    diffuseDirection /= 3.1415926f;
    
    return diffuseDirection;
    //return diffuseDirection + specularDirection;
}

//float3 ClacLightFromDirectionLight

// ポイントライトによる反射光を計算
#if POINT_LIGHT_ONE
float3 CalcLightFromPointLight(PSIn psIn)
{   
    // このサーフェイスに入射しているポイントライトの光の向きを計算する
    float3 lightDirection = psIn.worldPosition.xyz - pointLig.position.xyz;
        
    // 正規化して大きさ1のベクトルにする
    lightDirection = normalize(lightDirection);

    // 減衰なしのLambert拡散反射光を計算する
    float3 diffusePoint = CalcLambertDiffuse(
        lightDirection,         // ライトの方向
        pointLig.color.xyz, // ライトの色
        psIn.worldNormal.xyz    // サーフェイスの法線
    );
        
    // 減衰なしのPhong鏡面反射光を計算する
    float3 specularPoint = CalcPhongSpecular(
        lightDirection,         // ライトの方向
        pointLig.color.xyz, // ライトのカラー
        psIn.worldPosition.xyz, // サーフェイスのワールド座標
        psIn.worldNormal.xyz    // サーフェイスの法線
    );

    // 距離による影響率を計算する
    {
        // ポイントライトとの距離を計算する
        float distance = length(psIn.worldPosition.xyz - pointLig.position.xyz);
        
        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / pointLig.range * distance;
        
        // 影響力がマイナスにならないように修正をかける
        affect = max(0.0f, affect);
        
        // 影響を指数関数的にする。今回は3乗してる
        affect = pow(affect, 3.0f);
        
        // 拡散反射光と鏡面反射光に影響率を乗算して影響を弱める
        diffusePoint *= affect;
        specularPoint *= affect;
    }
    
    return diffusePoint + specularPoint;
}
#else
float3 CalcLightFromPointLight(PSIn psIn, PointLight p)
{
     // このサーフェイスに入射しているポイントライトの光の向きを計算する
    float3 lightDirection = psIn.worldPosition.xyz - p.position.xyz;
        
    // 正規化して大きさ1のベクトルにする
    lightDirection = normalize(lightDirection);

    // 減衰なしのLambert拡散反射光を計算する
    float3 diffusePoint = CalcLambertDiffuse(
        lightDirection, // ライトの方向
        p.color.xyz, // ライトの色
        psIn.worldNormal.xyz    // サーフェイスの法線
    );
        
    // 減衰なしのPhong鏡面反射光を計算する
    float3 specularPoint = CalcPhongSpecular(
        lightDirection, // ライトの方向
        p.color.xyz, // ライトのカラー
        psIn.worldPosition.xyz, // サーフェイスのワールド座標
        psIn.worldNormal.xyz    // サーフェイスの法線
    );

    // 距離による影響率を計算する
    {
        // ポイントライトとの距離を計算する
        float distance = length(psIn.worldPosition.xyz - p.position.xyz);
        
        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / p.range * distance;
        
        // 影響力がマイナスにならないように修正をかける
        affect = max(0.0f, affect);
        
        // 影響を指数関数的にする。今回は3乗してる
        affect = pow(affect, 3.0f);
        
        // 拡散反射光と鏡面反射光に影響率を乗算して影響を弱める
        diffusePoint *= affect;
        specularPoint *= affect;
    }
        
    return diffusePoint + specularPoint;
}
#endif// POINT_LIGHT_ONE

// スポットライトによる反射光を計算
float3 CalcLightFormSpotLight(PSIn psIn)
{
    // このサーフェイスに入射するスポットライトの光の向きを計算する
    float3 lightDirection = psIn.worldPosition.xyz - spotLig.position.xyz;
    
    // 正規化して大きさ１のベクトルにする
    lightDirection = normalize(lightDirection);
    
    // 減衰なしのLambert拡散反射光を計算する
    float3 diffuseSpotLight = CalcLambertDiffuse(
        lightDirection, // ライトの方向
        spotLig.color.xyz, // ライトの色
        psIn.worldPosition.xyz  // サーフェイスの法線
    );

    // 減衰なしのPhong鏡面反射光を計算する
    float3 specularSpotLight = CalcPhongSpecular(
        lightDirection, // ライトの方向
        spotLig.color.xyz, // ライトのカラー
        psIn.worldPosition.xyz, // サーフェイスのワールド座標
        psIn.worldNormal.xyz    // サーフェイスの法線
    );

    // 距離による減衰率を計算する
    {
        // スポットライトとの距離を計算する
        float3 distance = length(psIn.worldPosition.xyz - spotLig.position.xyz);
        
        // 影響率は距離に比例して小さくなっていく
        float affect = 1.0f - 1.0f / spotLig.range * distance;
        
        // 影響力がマイナスにならないように補正をかける
        affect = max(0.0f, affect);
        
        // 影響の仕方を指数関数的にする。今回は３乗している
        affect = pow(affect, 3.0f);
        
        // 影響率を乗算して反射光を弱める
        diffuseSpotLight *= affect;
        specularSpotLight *= affect;
    }
    
    // 角度による影響率を求める
    {
        // 入射光と射出方向の角度を求める
        float angle = dot(lightDirection, spotLig.direction);
        angle = abs(acos(angle));
       
        // 角度に比例して小さくなっていく影響率を計算する
        float affect = 1.0f - 1.0f / spotLig.angle * angle;
        
        // 影響率がマイナスにならないように補正をかける
        affect = max(0.0f, affect);
        
        // 影響の仕方を指数関数的にする。今回は0.5乗している
        diffuseSpotLight *= affect;
        specularSpotLight *= affect;
    }

    return diffuseSpotLight + specularSpotLight;
}

// リムライト
float3 CalcLimLight(PSIn psIn)
{
    // サーフェイスの法線と光の入射方向に依存するリムの強さを求める
    float power1 = 1.0f - max(0.0f, dot(directionLig.direction, psIn.worldNormal));

    // サーフェイスの法線と視点の方向に依存するリムの強さを求める
    float power2 = 1.0f - max(0.0f, psIn.normalInView * -1.0f);
    
    // 最終的なリムの強さを求める
    float limPower = power1 * power2;
    
    // 強さの変化を指数関数的にする
    limPower = pow(limPower, 1.3f);
    
    return limPower * directionLig.color;
}

// 半球ライト
float3 CalcHemisphereLight(PSIn psIn)
{
    // サーフェイスの法線と地面の法線との内積を計算する
    float t = dot(psIn.worldNormal, hemisphereLig.groundNormal);

    // 内積の結果を０～１の範囲に変換する
    t = (t + 1.0f) / 2.0f;
    
    // 地面色と天球色を補間率ｔで線形補間する
    float3 hemisphereLight = lerp(hemisphereLig.groundColor, hemisphereLig.skyColor, t);
        
    return hemisphereLight;
}