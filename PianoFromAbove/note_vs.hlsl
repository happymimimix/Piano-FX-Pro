#include "common.hlsl"

struct NoteData {
    uint packed;
    min16int alphainfo;
    float pos;
    float length;
};

struct TrackColor {
    uint colors[3]; // primary, dark, darker
};

struct FixedSizeData {
    float note_x[128];
    float bends[16];
};

ConstantBuffer<RootSignatureData> root : register(b0);
StructuredBuffer<FixedSizeData> fixed : register(t1);
StructuredBuffer<TrackColor> colors : register(t2);
StructuredBuffer<NoteData> note_data : register(t3);

float4 unpack_color(uint col) {
    return float4(float((col >> 16) & 0xFF) / 255.0, float((col >> 8) & 0xFF) / 255.0, float(col & 0xFF) / 255.0, float((col >> 24) & 0xFF) / 255.0);
}

NotePSInput main(uint id : SV_VertexID) {
    NotePSInput result;
    uint note_index = id / 4;
    uint vertex = id % 4;
    
    uint packed = note_data[note_index].packed;
    uint note = packed & 0xFF;
    uint chan = (packed >> 8) & 0xFF;
    uint track = (packed >> 16) & 0xFFFF;
#ifndef EQ_W
    bool sharp = ((1 << (note % 12)) & 0x54A) != 0;
#endif
    uint velalpha = note_data[note_index].alphainfo & 0xFF00 >> 8;
    bool mappingenabled = note_data[note_index].alphainfo & 0x00FF;
    bool is_right = vertex == 1 || vertex == 2;
    uint maincolor = colors[track * 16 + chan].colors[is_right];
    uint boardercolor = colors[track * 16 + chan].colors[2];
    if (mappingenabled) {
        maincolor = maincolor & 0x00FFFFFF | (velalpha << 24);
        boardercolor = boardercolor & 0x00FFFFFF | (velalpha << 24);
    }

    float x = fixed[0].note_x[note] + fixed[0].bends[chan];
    float y = round(root.notes_y + root.notes_cy * (1.0f - note_data[note_index].pos / root.timespan));
#ifdef EQ_W
    float cx = root.white_cx * 0.65f;
#else
    float cx = sharp ? root.white_cx * 0.65f : root.white_cx;
#endif
    float cy = max(round(root.notes_cy * note_data[note_index].length / root.timespan), 0)+1;

    float3 position = float3(x, y, 0);
    position.y -= cy * float(vertex < 2);
    position.x += cx * float(is_right);
#ifndef EQ_W
    position.z = !sharp * 0.5;
#endif
    
    result.position = colors[track * 16 + chan].colors[2] == 0xFFFFFFFF ? float4(0, 0, 0, 0) : mul(root.proj, float4(position, 1));
    result.color = float4(unpack_color(maincolor));
    result.border = float4(unpack_color(boardercolor));
    result.edges = float4(x, y, x + cx, y - cy);

    return result;
}