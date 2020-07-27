

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


    float cloud = fbm2(inTexCoord * 0.8, 20, Parameter.x * 0.2);
    cloud *= 2.0;
    cloud = saturate(cloud);
    cloud = cloud * 0.9 + 0.05;
    outDiffuse.rgb = g_Texture.Sample(g_SamplerState, float2(1.0 - cloud, 0.0));
    outDiffuse.a = saturate(cloud);
    if(cloud<0.05)
    {
        outDiffuse.a = 0.0;
    }
    
    
}
