cbuffer RootConstants : register(b0) {
    float4x4 root_proj;
    float root_deflate;
    float root_notes_y;
    float root_notes_cy;
    float root_white_cx;
    float root_timespan;
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