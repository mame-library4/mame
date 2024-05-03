struct PSIn
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 options;
}

cbuffer DISSOLVE_CONSTANT_BUFFER : register(b3)
{
    float4 parameters;  // x:�f�B�]���u�K���ʁA
                        // y
                        // z: ����臒l
                        // w:��
    float4 edgeColor;   // ���̐F
}