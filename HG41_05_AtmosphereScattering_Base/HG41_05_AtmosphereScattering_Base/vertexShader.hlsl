

//*****************************************************************************
// 定数バッファ
//*****************************************************************************



cbuffer ConstantBuffer : register(b0)
{
    matrix  World;
    matrix  View;
    matrix  Projection;
    float4  CameraPosition;
}


// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	float3		Dummy;//16bit境界用
};

cbuffer MaterialBuffer : register( b1 )
{
	MATERIAL	Material;
}


// ライトバッファ
struct LIGHT
{
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
};

cbuffer LightBuffer : register( b2 )
{
	LIGHT		Light;
}



//=============================================================================
// 頂点シェーダ
//=============================================================================
void main( in  float4 inPosition		: POSITION0,
			in  float4 inNormal		: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outPosition	: SV_POSITION,
			out float4 outWorldPosition   : POSITION0,
			out float4 outNormal		: NORMAL0,
			out float2 outTexCoord	: TEXCOORD0,
			out float4 outDiffuse		: COLOR0 )
{

	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul(inPosition, wvp);
    outWorldPosition = mul(inPosition, World);
	outNormal = inNormal;
	outTexCoord = inTexCoord;

	float4 worldNormal, normal;
	normal = float4(inNormal.xyz, 0.0);
	worldNormal = mul(normal, World);
	worldNormal = normalize(worldNormal);

    outDiffuse = inDiffuse;

}

