//	�~����
static const float PI = 3.141592654f;
//--------------------------------------------
//	�t���l����
//--------------------------------------------
//	F0: �������ˎ��̔��˗�
//	VdotH : �����x�N�g���ƃn�[�t�x�N�g��(�����ւ̃x�N�g���Ǝx�X�ւ̃x�N�g���̒��ԃx�N�g��)
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}

//--------------------------------------------
//	�g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
//	VdotH				: �����ւ̃x�N�g���ƃn�[�t�x�N�g���s�̓���
//	fresnelF0			: �������ˎ��̃t���l�����ˌ�
//	diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
    return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}

//--------------------------------------------
//	�@�����z�֐�
//--------------------------------------------
//	NdotH		: �@���x�N�g���ƃn�[�t�x�N�g��(�����ւ̃x�N�g���Ǝx�X�ւ̃x�N�g���̒��ԃx�N�g��)�̓���
//	roughness	: �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
    return a / (PI * b * b);
}

//--------------------------------------------
//	�􉽌������̎Z�o
//--------------------------------------------
//	NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//	NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ�
//	roughness	: �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = NdotL / (NdotL * (1.0f - r) + r);
    float masking = NdotV / (NdotV * (1.0f - r) + r);
    return shadowing * masking;
}

//--------------------------------------------
//	���ʔ���BRDF(�N�b�N�E�g�����X�̃}�C�N���t�@���Z�b�g���f��)
//--------------------------------------------
//	NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//	NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//	NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//	NdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//	fransnelF0	: �������ˎ��̃t���l�����ːF
//	roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	C��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, NdotH);
	
    return D * G * F / (NdotL * NdotV * 4.0f);
}

//--------------------------------------------
//	���ڌ��̕����x�[�X�����_�����O
//--------------------------------------------
//	diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//	F0					: �������ˎ��̃t���l�����ːF
//	normal				: �@���x�N�g��(���K���ς�)
//	eyeVector			: ���_�Ɍ������x�N�g��(���K���ς�)
//	lightVector			: �����Ɍ������x�N�g��(���K���ς�)
//	roughness			: �e��
void DirectBDRF(float3 diffuseReflectance, float3 F0, float3 normal, float3 eyeVector, float3 lightVector, float3 lightColor, float roughness,
				out float3 outDiffuse, out float3 outSpecular)
{
    float3 N = normal;
    float3 L = -lightVector;
    float3 V = -eyeVector;
    float3 H = normalize(L + V);
	
    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));
	
    float3 irradiance = lightColor * NdotL;
	
	//	�g�U����BRDF
    outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;
	
	//	���ʔ���BRDF
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
	
}

//--------------------------------------------
//	���b�N�A�b�v�e�[�u������GGX�����擾
//--------------------------------------------
//	brdfSamplePoiint : �T���v�����O�|�C���g
//	lutGgxMap : GGX���b�N�A�b�v�e�[�u��
//	state : �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleLutGGX(float2 brdfSamplePoint, Texture2D lutGgxMap, SamplerState state)
{
    return lutGgxMap.Sample(state, brdfSamplePoint);
}

//--------------------------------------------
//	�L���[�u�}�b�v����Ɠx���擾
//--------------------------------------------
//	v : �T���v�����O����
//	diffuseIemCubeMap : ���O�v�Z�g�U����IBL�L���[�u�}�b�v
//	state : �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleDiffuseIEM(float3 v, TextureCube diffuseIemCubeMap, SamplerState state)
{
    return diffuseIemCubeMap.Sample(state, v);
}

//--------------------------------------------
//	�L���[�u�}�b�v������ˋP�x���擾
//--------------------------------------------
//	v							: �T���v�����O����
//	roughness					: �e��
//	specularPmremCubeCubeMap	: ���O�v�Z���ʔ���IBL�L���[�u�}�b�v
//	state						: �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleSpecularPMREM(float3 v, float roughness, TextureCube specularPmremCubeMap, SamplerState state)
{
	//	�~�b�v�}�b�v�ɂ���đe����\�����邽�߁A�i�K���Z�o
    uint width, height, mipMaps;
    specularPmremCubeMap.GetDimensions(0, width, height, mipMaps);
    float lod = roughness * float(mipMaps - 1);
	
    return specularPmremCubeMap.SampleLevel(state, v, lod);
}

//--------------------------------------------
//	�e�����l�������t���l�����̋ߎ���
//--------------------------------------------
//	F0			: �������ˎ��̔��˗�
//	VdotN		: �����x�N�g���Ɩ@���x�N�g���Ƃ̓���
//	roughness	: �e��
float3 CalcFresnelRoughness(float3 f0, float NdotV, float roughness)
{
    return f0 + (max((float3) (1.0f - roughness), f0) - f0) * pow(saturate(1.0f - NdotV), 5.0f);
}

//--------------------------------------------
//	�g�U����IBL
//--------------------------------------------
//	normal				: �@��(���K���ς�)
//	eyeVector			: �����x�N�g��(���K���ς�)
//	roughness			: �e��
//	diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//	F0					: �������ˎ��̃t���l�����ˌ�
//	diffuseIemCubeMap	: ���O�v�Z�g�U����IBL�L���[�u�}�b�v
//	state				: �Q�Ǝ��̃T���v���[�X�e�[�g
float3 DiffuseIBL(float3 normal, float3 eyeVector, float roughness, float3 diffuseReflectance, float3 f0, TextureCube diffuseIemCubeMap, SamplerState state)
{
    float3 N = normal;
    float3 V = -eyeVector;
	
	//	�Ԑڊg�U���ˌ��̔��˗��v�Z
    float NdotV = max(0.0001f, dot(N, V));
    float3 kD = 1.0f - CalcFresnelRoughness(f0, NdotV, roughness);
	
    float3 irradiance = SampleDiffuseIEM(normal, diffuseIemCubeMap, state).rgb;
    
    return diffuseReflectance * irradiance * kD;
}

//--------------------------------------------
//	���ʔ���IBL
//--------------------------------------------
//  normal                  : �@���x�N�g��(���K���ς�)
//  eyeVector               : �����x�N�g��(���K���ς�)
//  roughness               : �e��
//  F0                      : �������ˎ��̃t���l�����ˌ�
//  lutGgxMap               : GGX���b�N�A�b�v�e�[�u��
//  specularPmremCubeMap    : ���O�v�Z���ʔ���IBL�L���[�u�}�b�v
//  state                   : �Q�Ǝ��̃T���v���[�X�e�[�g
float3 SpecularIBL(float3 normal, float3 eyeVector, float roughness, float3 f0, Texture2D lutGgxMap, TextureCube specularPmremCubeMap, SamplerState state)
{
    float3 N = normal;
    float3 V = -eyeVector;
    
    float NdotV = max(0.0001f, dot(N, V));
    float3 R = normalize(reflect(-V, N));
    float3 specularLight = SampleSpecularPMREM(R, roughness, specularPmremCubeMap, state).rgb;
    
    float2 brdfSamplePoint = saturate(float2(NdotV, roughness));
    float2 enyBrdf = SampleLutGGX(brdfSamplePoint, lutGgxMap, state).rg;
    
    return specularLight * (f0 * enyBrdf.x + enyBrdf.y);
}