#include "common.hlsli"

struct NoteData {
    uint packed;
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

    float x = fixed[0].note_x[note] + fixed[0].bends[chan];
    float y = round(root.notes_y + root.notes_cy * (1.0f - note_data[note_index].pos / root.timespan));
    float cx = root.white_cx * 0.65f;
    float cy = max(round(root.notes_cy * note_data[note_index].length / root.timespan), 0)+1;

    bool is_right = vertex == 1 || vertex == 2;
    float2 position = float2(x, y);
    position.y -= cy * float(vertex < 2);
    position.x += cx * float(is_right);
    
    result.position = colors[track * 16 + chan].colors[2] == 0xFFFFFFFF ? float4(0, 0, 0, 0) : mul(root.proj, float4(position, 0, 1));
    result.color = float4(unpack_color(colors[track * 16 + chan].colors[is_right]));
    result.border = float4(unpack_color(colors[track * 16 + chan].colors[2]));
    result.edges = float4(x, y, x + cx, y - cy);

    return result;
}