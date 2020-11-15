cbuffer cbTansMatrix : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float4 CameraPosition;
}

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
Texture2D textureEnv : register(t4);
SamplerState sampler0 : register(s0);


float4 main(in PS_INPUT input) : SV_TARGET
{
    float4 color;
    
    float4 normal = normalize(texture0.Sample(sampler0, input.Texcoord));
    float4 diffuse = texture1.Sample(sampler0, input.Texcoord);
    float4 position = texture2.Sample(sampler0, input.Texcoord);
    float4 depth = texture3.Sample(sampler0, input.Texcoord);
   
    // ライトベクトル
    float3 lightDir = float3(1.0f, -1.0f, 1.0f);
    lightDir = normalize(lightDir);
    
    // ハーフランバート
    float lam = 0.5f + dot(normalize(normal.xyz), -lightDir) * 0.5f;
    
    // カメラポジション
    float3 cameraPos = CameraPosition.xyz;
    float3 cameraDir = normalize(cameraPos - position.xyz);
    float3 ref = normalize(reflect(lightDir, normal.xyz));
    float specular = pow(saturate(dot(ref, cameraDir)), 50);
    
    // 深度フォグ
    float3 fogColor = float3(0.0, 0.0, 0.0);
    float fog = depth.r;

    color.rgb = lam * diffuse + specular;
    color.rgb = color.rgb * (1.0 - fog) + fog * fogColor;
    color.a = 1.0;
    
    float2 envUV;
    float3 refEye = reflect(cameraDir, normal.xyz);
    envUV.x = -refEye.x * 0.3 + 0.5;
    envUV.y = -refEye.y * 0.3 + 0.5;
    float4 envColor = textureEnv.Sample(sampler0, envUV) * 0.5;
    
    return color + envColor;
}