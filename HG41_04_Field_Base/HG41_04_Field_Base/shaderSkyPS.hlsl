

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


    float cloud = fbm2(inTexCoord * 0.8, 20, Parameter.x * 0.2);
    cloud *= 2.0;
    cloud = saturate(cloud);
    cloud = cloud * 0.9 + 0.05;
    outDiffuse.rgb = g_Texture.Sample(g_SamplerState, float2(1.0 - cloud, 0.0));
    outDiffuse.a = saturate(cloud);
    
    //レイリー散乱
    float3 vy = float3(0.0, 1.0, 0.0);
    float atm = saturate(1.0 - dot(-Light.Direction.xyz, vy));
    float3 rcolor = 1.0 - float3(0.4, 0.8, 1.0) * atm;
    
    outDiffuse.rgb *= rcolor;
    
    if(cloud<0.05)
    {
        outDiffuse.a = 0.0;
    }
    
    
}
