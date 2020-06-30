

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
void main(  in float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in float4 inLocalPosition   : POSITION1,
			in float4 inNormal			: NORMAL0,
			in float4 inDiffuse		    : COLOR0,
			in float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    // 非整数ブラウン運動
    float color = fbm3(inLocalPosition.xyz * 10.0, 5, Parameter.w * 1.2);
    color = acos(color * 20);
    color = saturate(color);
    
    outDiffuse.b = color * 0.5 + 0.5;
    outDiffuse.r = outDiffuse.b * 0.6;
    outDiffuse.g = outDiffuse.b * 1.0;
    
    if (outDiffuse.b < 0.8)
    {
        outDiffuse.rgb = volonoi_color3D(inLocalPosition.xyz * 5.0 + Parameter.w * 0.3) * 0.4;
    }
    // 木目
    //float color = fbm3(float3(inLocalPosition.x, inLocalPosition.y, inLocalPosition.z * 0.1), 3);
    //color = saturate(sin(color * 100) * 0.5 + 0.5);
    //color = color * color;
    
    //outDiffuse.rgb = float3(0.5, 0.2, 0.1) * color + float3(0.9, 0.7, 0.4) * (1.0 - color);
    

    
    //ライティング
    float3 lightDir = float3(1.0, -1.0, 1.0);
    lightDir = normalize(lightDir);
    
    float light = 0.5 - dot(inNormal.xyz, lightDir) * 0.5;
    //outDiffuse.rgb *= light;
        
    
    
	outDiffuse.a = 1.0;
  
}
