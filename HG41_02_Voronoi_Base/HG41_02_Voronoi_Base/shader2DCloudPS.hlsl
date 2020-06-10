
#include "shaderNoise.hlsl"


Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);


// 定数バッファ
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
    float4 Parameter;
}

struct PS_OUT
{
    float4 color : SV_Target;
};

float getDist(in float3 raypos, in float3 center, in float radius)
{
    return length(raypos - center) - radius;
}

float sdBox(float3 p, float3 c,float3 b)
{
    float3 q = abs(p - c) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float3 getNormal(in float3 raypos, in float3 center, in float radius)
{
    float ep = 0.001;
    return normalize(float3(
    getDist(raypos, center, radius) - getDist(float3(raypos.x - ep, raypos.y, raypos.z), center, radius),
    getDist(raypos, center, radius) - getDist(float3(raypos.x, raypos.y - ep, raypos.z), center, radius),
    getDist(raypos, center, radius) - getDist(float3(raypos.x, raypos.y, raypos.z - ep), center, radius)
    ));

}


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main(in float4 inPosition : SV_POSITION,
			in float4 inNormal : NORMAL0,
			in float4 inDiffuse : COLOR0,
			in float2 inTexCoord : TEXCOORD0,

			out PS_OUT output)
{
    float2 tc = float2(inTexCoord.xy * 2.0 - 1.0) * float2(1.0, -1.0);
    float3 rayDir = normalize(float3(tc, 1.0));
    float4 rayRot = mul(float4(rayDir, 0.0), World);
    rayDir = rayRot.xyz;
    
    float3 rayPos = CameraPosition.rgb;
    float cloudHeight = 10.0f;
    
    float4 color = float4(1, 1, 1, 0);
    
    float d = max(cloudHeight - rayPos.y, 0.0f)/ rayDir.y;
   
    if(d>=0)
    {
        rayPos += rayDir * d;
        float3 rp = rayPos;
        for (int j = 0; j < 30; j++)
        {
            float3 fbmSeed = (rp + float3(0.1, 0.02, 0.1) * Parameter.w) * float3(1.0,1.2,1.5);
            color += fbm3(fbmSeed * (1.0), 3) / 5.0;

            rp = rp + rayDir * j * 0.01;
        }
        color.a = color.a * 0.5 + 0.5;
    }
    output.color = color;
    
    ////ライティング
    //float3 lightDir = float3(1.0, -1.0, 1.0);
    //lightDir = normalize(lightDir);
    
    //float light = 0.5 - dot(getNormal(rayPos, sphereCenter, sphereRadius), lightDir) * 0.5;
    //output.color.rgb *= light;
    
}
