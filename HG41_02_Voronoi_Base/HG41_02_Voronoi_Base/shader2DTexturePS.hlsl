
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
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{
    // 斜めストライプ
    //outDiffuse.rgb = fmod(floor(inTexCoord.y + inTexCoord.x), 2);
    // チェック
    //outDiffuse.rgb = fmod(fmod(floor(inTexCoord.y * 2), 2) + fmod(floor(inTexCoord.x * 2), 2), 2);
    // ホワイトノイズ
    //outDiffuse.rgb = random2(inTexCoord);
    // ボロノイ図（色分け）
    //outDiffuse.rgb = volonoi_color(inTexCoord);
    // バリューノイズ
    //outDiffuse.rgb = valueNoise2D(inTexCoord * 1.0);
    // パーリンノイズ
    //outDiffuse.rgb = perlinNoise2D(inTexCoord * 1.0) * 0.5 + 0.5;
    
    //// 非整数ブラウン運動
    //float3 color = fbm2(inTexCoord * 1.0, 5);
    //color = acos(color * 20);
    
    
    //outDiffuse.b = color * 0.5 + 0.5;
    //outDiffuse.r = outDiffuse.b * 0.6;
    //outDiffuse.g = outDiffuse.b * 1.0;
    
    float2 seed = inTexCoord * Parameter.z + float2(Parameter.x, Parameter.y);

    float color = mandelbrot(seed);
    
    outDiffuse.rgb = color;
    
    outDiffuse.a = 1.0;

}
