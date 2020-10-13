struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Texcoord : TEXCOORD;
    float4 Diffuse : DIFFUSE;
};


Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


float4 main(in PS_INPUT input) : SV_TARGET
{
    float3 lightDir = float3(1.0f, -1.0f, 1.0f);
    lightDir = normalize(lightDir);
    
    float lam = dot(normalize(input.Normal.xyz), -lightDir);
    
    return lam * input.Diffuse * texture0.Sample(sampler0, input.Texcoord);
}