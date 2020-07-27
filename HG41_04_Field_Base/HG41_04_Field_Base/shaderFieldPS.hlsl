

#include "shaderNoise.hlsl"

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);


// �萔�o�b�t�@
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
}


// ���C�g�o�b�t�@
struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer LightBuffer : register(b1)
{
    LIGHT Light;
}


//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    // fbm�̃t���N�^���W��
    int octave = 10;
    
    // �ȈՖ@���v�Z
    float dx = turbulance2(inTexCoord + float2(0.0001, 0.0), octave)
            - turbulance2(inTexCoord + float2(-0.0001, 0.0), octave);
    float dz = turbulance2(inTexCoord + float2(0.0, 0.0001), octave)
            - turbulance2(inTexCoord + float2(0.0, -0.0001), octave);
    
    float3 normal = float3(dx * 3000.0, 1.0, -dz * 3000.0);
    normal = normalize(normal);
    
    // Light�x�N�g��
    float3 lightDir = normalize(Light.Direction.xyz);
    
        
    //���C���[�U��
    float3 vy = float3(0.0, 1.0, 0.0);
    float atm = saturate(1.0 - dot(-Light.Direction.xyz, vy));
    float3 rcolor = 1.0 - float3(0.4, 0.8, 1.0) * atm;
    
    // �n�[�t�����o�[�g
    float light = saturate(0.5 - dot(normal, lightDir) * 0.5);
    
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord * 1.0);
    //outDiffuse = float4(1.0, 1.0, 1.0, 1.0);
    outDiffuse *= light * float4(rcolor, 1.0);
    
    // �����t�H�O
    float height = -inWorldPosition.y + 2.5;
    height = saturate(height);
    
    outDiffuse.rgb = outDiffuse.rgb * (1.0 - height) + float3(0.0, 0.2, 0.2) * height;
    
    outDiffuse.a = 1.0;
}
