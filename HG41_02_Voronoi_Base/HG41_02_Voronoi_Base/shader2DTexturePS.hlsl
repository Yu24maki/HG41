
#include "shaderNoise.hlsl"


Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);



//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{

    outDiffuse.rgb = fmod(floor(inTexCoord.y + inTexCoord.x), 2);
    outDiffuse.rgb = fmod(fmod(floor(inTexCoord.y * 2), 2) + fmod(floor(inTexCoord.x * 2), 2), 2);
    outDiffuse.rgb = random2(inTexCoord);
    outDiffuse.a = 1.0;

}
