struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

// NOTE(sbalse): The constant buffer
cbuffer CBuf
{
    matrix transform;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
    VSOut result;

    result.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
    result.color = color;

    return result;
}
