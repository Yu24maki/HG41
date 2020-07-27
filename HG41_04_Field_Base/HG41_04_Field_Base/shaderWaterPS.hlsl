

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


// ライトバッファ
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
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    int octave = 7;
    float dx = turbulance2(inTexCoord * 0.5 + float2(0.0001, 0.0), octave, Parameter.x * 0.5)
            - turbulance2(inTexCoord * 0.5 + float2(-0.0001, 0.0), octave, Parameter.x * 0.5);
    float dz = turbulance2(inTexCoord * 0.5 + float2(0.0, 0.0001), octave, Parameter.x * 0.5)
            - turbulance2(inTexCoord * 0.5 + float2(0.0, -0.0001), octave, Parameter.x * 0.5);
    
    float3 normal = float3(dx * 3000.0, 1.0, -dz * 3000.0);
    normal = normalize(normal);
    
    float3 lightDir = Light.Direction.xyz;
    lightDir = normalize(lightDir);
    
            
    //レイリー散乱
    float3 vy = float3(0.0, 1.0, 0.0);
    float atm = saturate(1.0 - dot(-Light.Direction.xyz, vy));
    float3 rcolor = 1.0 - float3(0.4, 0.8, 1.0) * atm;

    
    float light = saturate(0.5 - dot(normal, lightDir) * 0.5);
    
    float3 eyev = inWorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    float3 ref = reflect(-lightDir, normal);
    float specular = pow(saturate(dot(ref, eyev)), 10);
    
    float fresnel = saturate(1.0 + dot(eyev, normal));
    fresnel = 0.05 + (1.0 - 0.05) * pow(fresnel, 5);
    outDiffuse.rgb = float3(0.0, 0.1, 0.1) * (1.0 - fresnel) + rcolor * fresnel;
    outDiffuse.rgb += float3(1.0, 1.0, 1.0) * specular * rcolor;
    outDiffuse.a = fresnel + specular;
}
