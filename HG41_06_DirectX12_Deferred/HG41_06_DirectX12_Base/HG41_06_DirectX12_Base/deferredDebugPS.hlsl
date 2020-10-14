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
   
    float4 color[4];
    color[0] = normal;
    color[1] = diffuse;
    color[2] = position;
    color[3] = depth;
    
    return color[(int)input.Texcoord.y];
}