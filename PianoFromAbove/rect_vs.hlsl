#include <common.hlsl>

RectPSInput main(float2 position : POSITION, float4 color : COLOR) {
    RectPSInput result;

    result.position = mul(root_proj, float4(position, 0, 1));
    result.color = color;

    return result;
}