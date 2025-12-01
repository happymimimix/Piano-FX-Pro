#include "common.hlsl"

float4 main(RectPSInput input) : SV_TARGET {
    return input.color;
}