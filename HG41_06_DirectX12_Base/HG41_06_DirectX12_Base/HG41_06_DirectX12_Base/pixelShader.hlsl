struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Texcoord : TEXCOORD;
    float4 Diffuse : DIFFUSE;
};


float4 main(in PS_INPUT input) : SV_TARGET
{
    return input.Diffuse;
}