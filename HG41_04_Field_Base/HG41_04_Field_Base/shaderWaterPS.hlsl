

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
    float4 Parameter;

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
    int octave = 5;
    float dx = fbm2(inTexCoord + float2(0.0001, 0.0), octave, Parameter.x)
            - fbm2(inTexCoord + float2(-0.0001, 0.0), octave, Parameter.x);
    float dz = fbm2(inTexCoord + float2(0.0, 0.0001), octave, Parameter.x)
            - fbm2(inTexCoord + float2(0.0, -0.0001), octave, Parameter.x);
    
    float3 normal = float3(dx * 3000.0, 1.0, -dz * 3000.0);
    normal = normalize(normal);
    
    //float3 lightDir = float3(1.0, -0.5, 1.0);
    //lightDir = normalize(lightDir);
    
    //float light = saturate(0.5 - dot(normal, lightDir) * 0.5);
    
    float3 eyev = inWorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    float fresnel = saturate(1.0 + dot(eyev, normal));
    fresnel = 0.05 + (1.0 - 0.05) * pow(fresnel, 5);
    outDiffuse.rgb = float3(0.0, 0.1, 0.1) * (1.0 - fresnel) + float3(0.8, 0.85, 0.9) * fresnel;
    outDiffuse.a = 1.0;
}
