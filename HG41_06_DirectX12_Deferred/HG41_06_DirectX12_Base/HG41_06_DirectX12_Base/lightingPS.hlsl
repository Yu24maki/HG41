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


Texture2D<float4> texture0 : register(t0);
Texture2D<float4> texture1 : register(t1);
Texture2D<float4> texture2 : register(t2);
Texture2D<float4> texture3 : register(t3);
Texture2D<float4> textureEnv : register(t4);
Texture2D<float4> textureIBL : register(t5);
SamplerState sampler0 : register(s0);


// EnvMap
/*
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
*/

// IBL
/*
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
    
    // IBL
    float2 iblTexCoord;
    iblTexCoord.x = -normal.x * 0.3 + 0.5;
    iblTexCoord.y = -normal.y * 0.3 + 0.5;
    float3 iblColor = textureIBL.Sample(sampler0, iblTexCoord);
    
    color.rgb = iblColor * diffuse.rgb * 1.5;
    color.a = 1.0;
        
    return color;
}
*/

// PBR
float4 main(in PS_INPUT input) : SV_TARGET
{
    // ライトベクトル
    float3 lightDir = float3(1.0f, -1.0f, 1.0f);
    lightDir = normalize(lightDir);
    
    // カメラポジション
    float3 cameraPos = CameraPosition.xyz;
    
    float4 color;
    
    float4 normal = normalize(texture0.Sample(sampler0, input.Texcoord));
    float4 albedo = texture1.Sample(sampler0, input.Texcoord);
    float4 position = texture2.Sample(sampler0, input.Texcoord);
    float4 depth = texture3.Sample(sampler0, input.Texcoord);
   
    float3 eye = normalize(position.xyz - cameraPos);
    float3 ref = normalize(reflect(eye, normal.xyz));

    // IBL
    float2 iblTexCoord;
    iblTexCoord.x = -normal.x * 0.3 + 0.5;
    iblTexCoord.y = -normal.y * 0.3 + 0.5;
    float3 diffuse = albedo * textureIBL.Sample(sampler0, iblTexCoord);

    // 環境マッピング
    float2 envTexCoord;
    envTexCoord.x = -ref.x * 0.3 + 0.5;
    envTexCoord.y = -ref.y * 0.3 + 0.5;
    float3 specular = textureEnv.Sample(sampler0, envTexCoord) * 0.5;
    
    float roughness = 0.01;
    
    //specular = textureEnv.SampleBias(sampler0, envTexCoord, roughness * 9.0) * 0.5;
   
    
    // メタリック係数
    float metallic = 0.01;
    // スペキュラ係数
    float spec = 0.99;
    // フレネル係数
    float3 f0 = lerp(0.08 * spec, albedo.rgb, metallic);
    
    // フレネル反射率
    float3 f = f0 + (1.0 - f0) * pow(1.0 - dot(normal.xyz, ref), 5);
    
    color.rgb = diffuse * (1.0 - metallic) * (1.0 - f) + specular * f;
    color.a = 1.0;
    
    // 明度調整
    color.rgb *= 2.0;
    
    return color;
}
