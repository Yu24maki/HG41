

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
    
    //ライティング
    float3 normal = normalize(inNormal.xyz);
    float light = -dot(Light.Direction.xyz, inNormal.xyz);
    
    float3 diffuse = inDiffuse.rgb * light;
    
    
    //大気距離
    float dist = distance(inWorldPosition.xyz, CameraPosition.xyz);
    
    //視線ベクトル
    float3 eye = inWorldPosition.xyz - CameraPosition.xyz;
    eye = normalize(eye);
    
    
    //ミー散乱
    float m = saturate(-dot(Light.Direction.xyz, eye));
    m = pow(saturate(m), 50);
    
    diffuse += m * dist * 0.005;
    
    
    //レイリー散乱
    float3 vy = float3(0.0, 1.0, 0.0);
    float atm = saturate(1.0 - dot(-Light.Direction.xyz, vy));
    float3 rcolor = 1.0 - float3(0.4, 0.8, 1.0) * atm;
    
    //diffuse += rcolor;
    
    float ld = 0.5 - dot(Light.Direction.xyz, eye) * 0.5;
    diffuse += rcolor * dist * ld * float3(0.5, 0.8, 1.0) * 0.006;
    
    outDiffuse.rgb = diffuse;
    outDiffuse.a = inDiffuse.a;

}
