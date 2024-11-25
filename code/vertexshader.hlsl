struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

// NOTE(sbalse): The constant buffer
cbuffer CBuf
{
    // NOTE(sbalse): Chilli says multiplying a row major matrix on the GPU is slightly slower than a column
    // major matrix. In the future, we will transpose it first on the CPU side.
    row_major matrix transform;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
    VSOut result;

    result.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
    result.color = color;

    return result;
}
