#include <common.hlsl>

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
    // Unpack the note data. 
    uint note = packed & 0x7F;
    uint chan = (packed >> 8) & 0x0F;
#ifdef LIMIT_COLORS
    uint track = (packed >> 16) & 0x00FF;
#else
    uint track = (packed >> 16) & 0xFFFF;
#endif
#ifndef EQ_W
    bool sharp = ((1 << (note % 12)) & 0x54A) != 0;
#endif
    // Where's the alpha? Now watch this magic happen! 
    // Note can only be 0~127, one spare bit in the most significant bit. 
    // Channel can only be 0~15, found spare bits in the most significant bit. 
    // Track can be any integer between 0~65535, no spare bits. 
    // We still need two more bits... 
    // The note length is always positive, so let's borrow the sign bit of this float. 
    // Clearing the lowest bit in a 32bit float won't cause any notable precision loss, so let's borrow the lowest bit in the length variable. 
    // Together, this gives us 7 bits to work with. 
    // Where's the enable mapping switch? 
    // Well, we all know that midi notes can never have a velocity smaller than 1, right? 
    // According to the formula I used to calculate alpha: Alpha = Velocity ^ 0x7F
    // We know that it's not possible for alpha to be exactly 127. It must be 126 and below. 
    // So if alpha is exactly equal to 127, we can know for sure that velocity to alpha mapping has been disabled. 
    // This allowed us to fit the note alpha together with the mapping enabled switch all inside only 7 bits. 
    uint length_bits = asuint(note_data[note_index].length);
    uint velalpha =
        ((packed >> 7) & 0x01) | // highest bit in note
        ((packed >> 11) & 0x1E) | // highest 4 bits in channel
        ((length_bits >> 26) & 0x20) | // highest bit in length
        ((length_bits & 0x01) << 6); // lowest bit in length
    // Make sure we clear all the borrowed bits! 
    float note_pos = note_data[note_index].pos;
    float note_length = asfloat(length_bits & 0x7FFFFFFE);
    bool mappingenabled = !!(velalpha ^ 0x7F);
    bool is_right = vertex == 1 || vertex == 2;
    uint maincolor = colors[track * 16 + chan].colors[is_right];
    uint boardercolor = colors[track * 16 + chan].colors[2];
    if (mappingenabled) {
        maincolor = maincolor & 0x00FFFFFF | (velalpha << 24);
        boardercolor = boardercolor & 0x00FFFFFF | (velalpha << 24);
    }

    float x = fixed[0].note_x[note] + fixed[0].bends[chan];
    float y = round(root_notes_y + root_notes_cy * (1.0f - note_pos / root_timespan));
#ifdef EQ_W
    float cx = root_white_cx * 0.65f;
#else
    float cx = sharp ? root_white_cx * 0.65f : root_white_cx;
#endif
    float cy = max(round(root_notes_cy * note_length / root_timespan), 0) + 1;

    float3 position = float3(x, y, 0);
    position.y -= cy * float(vertex < 2);
    position.x += cx * float(is_right);
#ifndef EQ_W
    position.z = !sharp * 0.5;
#endif

    result.position = colors[track * 16 + chan].colors[2] == 0xFFFFFFFF ? float4(0, 0, 0, 0) : mul(root_proj, float4(position, 1));
    result.color = float4(unpack_color(maincolor));
    result.border = float4(unpack_color(boardercolor));
    result.edges = float4(x, y, x + cx, y - cy);

    return result;
}