// NOTE(sbalse): The transformation constant buffer
cbuffer TransformConstantBuffer
{
    matrix Transform;
};

float4 main(float3 pos : POSITION) : SV_Position
{
    return mul(float4(pos, 1.0f), Transform);
}
