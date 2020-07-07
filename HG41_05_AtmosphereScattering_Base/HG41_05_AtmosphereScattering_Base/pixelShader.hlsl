

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


// マテリアルバッファ
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    float3 Dummy; //16bit境界用
};

cbuffer MaterialBuffer : register(b1)
{
    MATERIAL Material;
}


// ライトバッファ
struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer LightBuffer : register(b2)
{
    LIGHT Light;
}


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition  : POSITION0,
			in  float4 inNormal		    : NORMAL0,
			in  float2 inTexCoord		: TEXCOORD0,
			in  float4 inDiffuse		: COLOR0,

			out float4 outDiffuse		: SV_Target )
{    
    
    //ライティング
    float3 normal = normalize(inNormal.xyz);
    float light = -dot(Light.Direction.xyz, inNormal.xyz);
    
    float3 diffuse = inDiffuse.rgb * light;
    
    
    
    outDiffuse.rgb = diffuse;
    outDiffuse.a = inDiffuse.a;

}
