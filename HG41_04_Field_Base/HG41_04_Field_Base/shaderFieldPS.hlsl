

#include "shaderNoise.hlsl"

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);


// 定数バッファ
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
}



//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    int octave = 10;
    float dx = turbulance2(inTexCoord + float2(0.0001, 0.0), octave)
            - turbulance2(inTexCoord + float2(-0.0001, 0.0), octave);
    float dz = turbulance2(inTexCoord + float2(0.0, 0.0001), octave)
            - turbulance2(inTexCoord + float2(0.0, -0.0001), octave);
    
    float3 normal = float3(dx * 3000.0, 1.0, -dz * 3000.0);
    normal = normalize(normal);
    
    float3 lightDir = float3(1.0, -0.5, 1.0);
    lightDir = normalize(lightDir);
    
    float light = saturate(0.5 - dot(normal, lightDir) * 0.5);
    
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord * 1.0);
    outDiffuse *= light;
    outDiffuse.a = 1.0;
}
