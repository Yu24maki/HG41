


Texture2D<float4> texture0 : register(t0);
Texture2D<float4> texture1 : register(t1);
Texture2D<float4> texture2 : register(t2);
Texture2D<float4> texture3 : register(t3);
Texture2D<float4> textureEnv : register(t4);
SamplerState sampler0 : register(s0);


struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Diffuse  : COLOR;
};

cbuffer cbTansMatrix : register(b0)
{
    float4x4 WVP;
    float4x4 World;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 lightDirection = float3(0.0, -1.0, 0.0);
    lightDirection = normalize(lightDirection);
    
    float3 cameraPosition = float3(0.0, 0.0, -5.0);

    float4 normal = texture0.Sample(sampler0, input.TexCoord);
    float4 color = texture1.Sample(sampler0, input.TexCoord);
    float4 position = texture2.Sample(sampler0, input.TexCoord);
    float4 depth = texture3.Sample(sampler0, input.TexCoord);
        
 
    //ハーフランバート
    float light = saturate(0.5 - dot(normal.xyz, lightDirection) * 0.5);  
    color.rgb *= light;    
    
    // レイマーチングシャドウ
    //{
    //    float4 rayPos;
    //    rayPos = position;
    //    rayPos.w = 1.0;
        
    //    float4 dir;
    //    dir.xyz = -lightDirection;
    //    dir.w = 0.0;
    //    dir = normalize(dir) * 0.1;
        
    //    for (int i = 0; i < 50;i++)
    //    {
    //        rayPos += dir;
            
    //        float4 sp = mul(rayPos, World);
    //        sp.xy = sp.xy / sp.w * 0.5 + 0.5;
    //        sp.y = 1 - sp.y;
    //        if (sp.y < 0.0)
    //            break;
    //        if (sp.x < 0.0)
    //            break;
            
    //        float depth = texture2.Sample(sampler0, sp.xy).w;
            
    //        if(depth + 0.01 < sp.z / 20.0)
    //        {
    //            color.rgb *= 0.5;
    //            break;
    //        }
    //    }

    //}
    
    // レイマーチングリフレクション
    {
        float4 rayPos;
        rayPos = position;
        rayPos.w = 1.0;
        
        float4 dir;
        dir.xyz = position.xyz - cameraPosition;
        dir.xyz = reflect(dir.xyz, normal.xyz);
        dir.w = 0.0;
        dir = normalize(dir) * 0.1;
        
        for (int i = 0; i < 50; i++)
        {
            rayPos += dir;
            
            float4 sp = mul(rayPos, World);
            sp.xy = sp.xy / sp.w * 0.5 + 0.5;
            sp.y = 1 - sp.y;
            
            if (sp.y < 0.0)
                break;
            if (sp.z > 6.0)
                break;
            
            float depth = texture2.SampleLevel(sampler0, sp.xy, 0).w;
            
            if (depth + 0.01 < sp.z / 20.0)
            {
                color.rgb += texture1.SampleLevel(sampler0, sp.xy, 0) * 0.1;
                break;
            }
        }

    }

    return color;
}