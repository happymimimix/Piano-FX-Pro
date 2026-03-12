cbuffer RootConstants : register(b0) {
    float4x4 rootproj;
    float rootdeflate;
    float rootnotes_y;
    float rootnotes_cy;
    float rootwhite_cx;
    float roottimespan;
};

struct RectPSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct NotePSInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 edges : TEXCOORD0; // left, top, right, bottom
    float4 border : COLOR1;
};

struct BackgroundPSInput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};