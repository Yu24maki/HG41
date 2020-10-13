struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 Texcoord : TEXCOORD;
    float4 Diffuse : DIFFUSE;
};

struct PS_OUT
{
    float4 Normal : SV_TARGET0;
    float4 Diffuse : SV_TARGET1;
};


Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


void main(in PS_INPUT input, out PS_OUT output)
{
    output.Normal = input.Normal;
    output.Diffuse = input.Diffuse * texture0.Sample(sampler0, input.Texcoord);
}