#include <common.hlsl>

BackgroundPSInput main(uint id : SV_VertexID) {
    BackgroundPSInput result;

    result.uv = float2((id << 1) & 2, id & 2);
    result.position = float4(result.uv * float2(2, -2) + float2(-1, 1), 0, 1);

    return result;
}