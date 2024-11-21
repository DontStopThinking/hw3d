struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
    VSOut result;

    result.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    result.color = color;

    return result;
}
