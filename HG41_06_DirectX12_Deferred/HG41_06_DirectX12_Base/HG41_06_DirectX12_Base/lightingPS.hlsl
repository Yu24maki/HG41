struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Texcoord : TEXCOORD;
    float4 Diffuse : DIFFUSE;
};


Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);
Texture2D texture3 : register(t3);
SamplerState sampler0 : register(s0);


float4 main(in PS_INPUT input) : SV_TARGET
{
    float4 normal = normalize(texture0.Sample(sampler0, input.Texcoord));
    float4 diffuse = texture1.Sample(sampler0, input.Texcoord);
    float4 position = texture2.Sample(sampler0, input.Texcoord);
    float4 depth = texture3.Sample(sampler0, input.Texcoord);
   
    float3 lightDir = float3(1.0f, -1.0f, 1.0f);
    lightDir = normalize(lightDir);
    
    float lam = 0.5f + dot(normalize(normal.xyz), -lightDir) * 0.5f;
    
    //return normal;
    //return depth;
    return lam * diffuse;
}