//================================================
// 疑似乱数
//
// 引数 シード値(2D) (テクスチャ座標などを利用)
// 戻値 疑似乱数(1D)
//================================================
float random2(in float2 vec)
{
    return frac(sin(dot(vec.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float2 random22(in float2 vec)
{
    vec = float2(dot(vec, float2(127.1, 311.7)), dot(vec, float2(269.5, 183.3)));
    
    return frac(sin(vec) * 43758.5453123);
}

// ボロノイ図
float volonoi2(in float2 vec)
{
    float2 ivec = floor(vec);
    float2 fvec = frac(vec);
    
    float value = 1.0f;
    
    for (int y = -1; y <= 1;y++)
    {
        for (int x = -1; x <= 1;x++)
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
            
            if(value > dist)
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

