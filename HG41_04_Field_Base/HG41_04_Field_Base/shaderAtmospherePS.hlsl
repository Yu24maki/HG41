

//*****************************************************************************
// 定数バッファ
//*****************************************************************************


cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 CameraPosition;
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
void main(in float4 inPosition : SV_POSITION,
            in float4 inWorldPosition : POSITION0,
			in float4 inNormal : NORMAL0,
			in float2 inTexCoord : TEXCOORD0,
			in float4 inDiffuse : COLOR0,

			out float4 outDiffuse : SV_Target)
{

    float3 diffuse = 0;
    
    float2 tc = float2(inTexCoord.xy * 2.0 - 1.0) * float2(1.0, -1.0);
    tc.x *= 16.0 / 9.0;
    float3 rayDir = normalize(float3(tc, 1.0));
    float4 rayRot = mul(float4(rayDir, 0.0), World);
    rayDir = rayRot.xyz;

    
    //大気距離
    float dxz = 900.0;
    float dy = 100.0;
    float la = atan2(-Light.Direction.y, length(-Light.Direction.xz));
    float dist = length(float2(dxz * cos(la), dy * sin(la)));
    
    //視線ベクトル
    float3 eye = rayDir;
    eye = normalize(eye);
    
    
    //ミー散乱
    float m = saturate(-dot(Light.Direction.xyz, eye));
    m = pow(m, 70);
    
    diffuse += m * dist * 0.0005;
    
    
    //レイリー散乱
    float3 vy = float3(0.0, 1.0, 0.0);
    float atm = saturate(1.0 - dot(-Light.Direction.xyz, vy));
    float3 rcolor = 1.0 - float3(0.5, 0.8, 1.0) * atm;
    
    //diffuse += rcolor;
    
    float ld = 0.5 - dot(Light.Direction.xyz, eye) * 0.5;
    diffuse += rcolor * dist * ld * float3(0.5, 0.8, 1.0) * 0.002;
    
    outDiffuse.rgb = diffuse;
    //outDiffuse.rgb = float3(0.2, 0.5, 0.8);
    outDiffuse.a = 1.0f;

}
