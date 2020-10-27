cbuffer cbTansMatrix : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4 CameraPosition;
}


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Texcoord : TEXCOORD;
};


struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Texcoord : TEXCOORD;
    float4 Diffuse : DIFFUSE;
};


void main(in VS_INPUT input, out PS_INPUT output)
{
    float4 position = float4(input.Position, 1.0f);
    output.Position = mul(position, WVP);
    
    float4 normal = float4(input.Normal, 0.0f);
    output.Normal = mul(normal, World);
    
    output.Texcoord = input.Texcoord;
    
    output.Diffuse.rgb = float3(1.0f, 1.0f, 1.0f);
    output.Diffuse.a = 1.0f;
}