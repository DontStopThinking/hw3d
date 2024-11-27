cbuffer CBuf2
{
    float4 faceColors[6];
};

float4 main(uint triangleId : SV_PrimitiveID) : SV_TARGET
{
    // NOTE(sbalse): There are two triangles for every face in the cube. So we divide the lookup index by 2.
    return faceColors[triangleId / 2];
}
