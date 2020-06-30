//================================================
// 疑似乱数
//
// 引数 シード値(2D) (テクスチャ座標などを利用)
// 戻値 疑似乱数(1D)
//================================================
float random2(in float2 vec)
{
    return frac(sin(dot(vec.xy, float2(12.9898, 78.233))) * 4.37585453123);
}

float random3(in float3 vec)
{
    return frac(sin(dot(vec.xyz, float3(12.9898, 78.233, 47.2311))) * 4.37585453123);

}

float2 random22(in float2 vec)
{
    vec = float2(dot(vec, float2(127.1, 311.7)), dot(vec, float2(269.5, 183.3)));
    
    return frac(sin(vec) * 43758.5453123);
}

float3 random33(in float3 vec)
{
    vec = float3(dot(vec, float3(127.1, 311.7, 245.4)),
                dot(vec, float3(269.5, 183.3, 131.2)),
                dot(vec, float3(522.3, 243.1, 532.4)));
    
    return frac(sin(vec) * 43758.5453123);
}

// ボロノイ図
float volonoi2(in float2 vec)
{
    float2 ivec = floor(vec);
    float2 fvec = frac(vec);
    
    float value = 1.0f;
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 offset = float2(x, y);
            
            float2 position;
            position = random22(ivec + offset);
            
            float dist = distance(position + offset, fvec);
            
            value = min(value, dist);
        }

    }

    return value;
}
   
// 色分け
float3 volonoi_color(in float2 vec)
{
    float3 color = float3(1.0f, 1.0f, 1.0f);
    float2 pos = float2(1.0f, 1.0f);
    
    float2 ivec = floor(vec);
    float2 fvec = frac(vec);
    
    float value = 1.0f;
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 offset = float2(x, y);
            
            float2 position;
            position = random22(ivec + offset);
            
            float dist = distance(position + offset, fvec);
            
            if (value > dist)
            {
                value = min(value, dist);
                pos = position;
            }
        }

    }
    color = float3(frac(pos.r), frac(pos.g), frac(pos.r + pos.g));
    return color;
}

// バリューノイズ2D
float valueNoise2D(in float2 vec)
{
    float2 ivec = floor(vec);
    float2 fvec = frac(vec);
    
    float a = random2(ivec + float2(0.0f, 0.0f));
    float b = random2(ivec + float2(1.0f, 0.0f));
    float c = random2(ivec + float2(0.0f, 1.0f));
    float d = random2(ivec + float2(1.0f, 1.0f));
    
    fvec = smoothstep(0.0f, 1.0f, fvec);

    return lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
}

// バリューノイズ3D
float valueNoise3D(in float3 vec)
{
    float3 ivec = floor(vec);
    float3 fvec = frac(vec);
    
    float a = random3(ivec + float3(0.0f, 0.0f, 0.0f));
    float b = random3(ivec + float3(1.0f, 0.0f, 0.0f));
    float c = random3(ivec + float3(0.0f, 1.0f, 0.0f));
    float d = random3(ivec + float3(1.0f, 1.0f, 0.0f));
    float e = random3(ivec + float3(0.0f, 0.0f, 1.0f));
    float f = random3(ivec + float3(1.0f, 0.0f, 1.0f));
    float g = random3(ivec + float3(0.0f, 1.0f, 1.0f));
    float h = random3(ivec + float3(1.0f, 1.0f, 1.0f));
    
    fvec = smoothstep(0.0f, 1.0f, fvec);
    
    float v1 = lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
    float v2 = lerp(lerp(e, f, fvec.x), lerp(g, h, fvec.x), fvec.y);
    float v3 = lerp(v1, v2, fvec.z);

    return v3;
}

// パーリンノイズ2D
float perlinNoise2D(in float2 vec)
{
    float2 ivec = floor(vec);
    float2 fvec = frac(vec);
    
    float a = dot(random22(ivec + float2(0.0f, 0.0f)) * 2.0 - 1.0, fvec - float2(0.0f, 0.0f));
    float b = dot(random22(ivec + float2(1.0f, 0.0f)) * 2.0 - 1.0, fvec - float2(1.0f, 0.0f));
    float c = dot(random22(ivec + float2(0.0f, 1.0f)) * 2.0 - 1.0, fvec - float2(0.0f, 1.0f));
    float d = dot(random22(ivec + float2(1.0f, 1.0f)) * 2.0 - 1.0, fvec - float2(1.0f, 1.0f));
    
    fvec = smoothstep(0.0f, 1.0f, fvec);

    return lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
}

// パーリンノイズ3D
float perlinNoise3D(in float3 vec)
{
    float3 ivec = floor(vec);
    float3 fvec = frac(vec);
    
    float a = dot(random33(ivec + float3(0.0f, 0.0f, 0.0f)) * 2.0 - 1.0, fvec - float3(0.0f, 0.0f, 0.0f));
    float b = dot(random33(ivec + float3(1.0f, 0.0f, 0.0f)) * 2.0 - 1.0, fvec - float3(1.0f, 0.0f, 0.0f));
    float c = dot(random33(ivec + float3(0.0f, 1.0f, 0.0f)) * 2.0 - 1.0, fvec - float3(0.0f, 1.0f, 0.0f));
    float d = dot(random33(ivec + float3(1.0f, 1.0f, 0.0f)) * 2.0 - 1.0, fvec - float3(1.0f, 1.0f, 0.0f));
    float e = dot(random33(ivec + float3(0.0f, 0.0f, 1.0f)) * 2.0 - 1.0, fvec - float3(0.0f, 0.0f, 1.0f));
    float f = dot(random33(ivec + float3(1.0f, 0.0f, 1.0f)) * 2.0 - 1.0, fvec - float3(1.0f, 0.0f, 1.0f));
    float g = dot(random33(ivec + float3(0.0f, 1.0f, 1.0f)) * 2.0 - 1.0, fvec - float3(0.0f, 1.0f, 1.0f));
    float h = dot(random33(ivec + float3(1.0f, 1.0f, 1.0f)) * 2.0 - 1.0, fvec - float3(1.0f, 1.0f, 1.0f));
    
    fvec = smoothstep(0.0f, 1.0f, fvec);

    float v1 = lerp(lerp(a, b, fvec.x), lerp(c, d, fvec.x), fvec.y);
    float v2 = lerp(lerp(e, f, fvec.x), lerp(g, h, fvec.x), fvec.y);
    float v3 = lerp(v1, v2, fvec.z);

    return v3;
}

// 非整数ブラウン運動2D
float fbm2(in float2 vec, int octave, float offset = 0.0)
{
    float value = 0.0;
    float amplitude = 1.0;
    
    for (int i = 0; i < octave; i++)
    {
        value += amplitude * perlinNoise2D(vec + offset);
        vec *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// 非整数ブラウン運動3D
float fbm3(in float3 vec, int octave)
{
    float value = 0.0;
    float amplitude = 1.0;
    
    for (int i = 0; i < octave; i++)
    {
        value += amplitude * perlinNoise3D(vec);
        vec *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// fbm2の絶対値版
float turbulance2(in float2 vec, int octave, float offset = 0.0)
{
    float value = 0.0;
    float amplitude = 1.0;
    
    for (int i = 0; i < octave; i++)
    {
        value -= amplitude * abs(perlinNoise2D(vec + offset));
        vec *= 2.0;
        amplitude *= 0.5;
    }

    return abs(value);
}
