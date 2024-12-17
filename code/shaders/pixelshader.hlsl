// NOTE(sbalse): The cube face colors constant buffer.
cbuffer FaceColorsConstantBuffer
{
    float4 FaceColors[6];
};

float4 main(uint triangleId : SV_PrimitiveID) : SV_TARGET
{
    // NOTE(sbalse): There are two triangles for every face in the cube. So we divide the lookup index by 2.
    return FaceColors[triangleId / 2];
}
