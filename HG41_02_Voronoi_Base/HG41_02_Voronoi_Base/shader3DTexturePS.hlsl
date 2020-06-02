

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
void main(  in float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in float4 inLocalPosition   : POSITION1,
			in float4 inNormal			: NORMAL0,
			in float4 inDiffuse		    : COLOR0,
			in float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    // 非整数ブラウン運動
    float color = fbm3(inLocalPosition.xyz * 10.0, 5);
    color = acos(color * 20);
    
    outDiffuse.b = color * 0.5 + 0.5;
    outDiffuse.r = outDiffuse.b * 0.6;
    outDiffuse.g = outDiffuse.b * 1.0;

    //outDiffuse.rgb = color;
    
    
    
    //ライティング
    float3 lightDir = float3(1.0, -1.0, 1.0);
    lightDir = normalize(lightDir);
    
    float light = 0.5 - dot(inNormal.xyz, lightDir) * 0.5;
    outDiffuse.rgb *= light;
        
    
    
	outDiffuse.a = 1.0;
  
}
