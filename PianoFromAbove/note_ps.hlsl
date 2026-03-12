#include "common.hlsl"

ConstantBuffer<RootSignatureData> root : register(b0);

float4 main(NotePSInput input) : SV_TARGET {
    return (abs(input.position.x - input.edges.x) <= root_deflate ||
            abs(input.position.x - input.edges.z) <= root_deflate ||
            abs(input.position.y - input.edges.y) <= root_deflate ||
            abs(input.position.y - input.edges.w) <= root_deflate) ? input.border : input.color;
}