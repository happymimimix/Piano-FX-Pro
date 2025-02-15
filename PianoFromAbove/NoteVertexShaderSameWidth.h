#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer root
// {
//
//   struct RootSignatureData
//   {
//       
//       float4x4 proj;                 // Offset:    0
//       float deflate;                 // Offset:   64
//       float notes_y;                 // Offset:   68
//       float notes_cy;                // Offset:   72
//       float white_cx;                // Offset:   76
//       float timespan;                // Offset:   80
//
//   } root;                            // Offset:    0 Size:    84
//
// }
//
// Resource bind info for fixed
// {
//
//   struct FixedSizeData
//   {
//       
//       float note_x[128];             // Offset:    0
//       float bends[16];               // Offset:  512
//
//   } $Element;                        // Offset:    0 Size:   576
//
// }
//
// Resource bind info for colors
// {
//
//   struct TrackColor
//   {
//       
//       uint colors[3];                // Offset:    0
//
//   } $Element;                        // Offset:    0 Size:    12
//
// }
//
// Resource bind info for note_data
// {
//
//   struct NoteData
//   {
//       
//       uint packed;                   // Offset:    0
//       float pos;                     // Offset:    4
//       float length;                  // Offset:    8
//
//   } $Element;                        // Offset:    0 Size:    12
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// fixed                             texture  struct         r/o      T0             t1      1 
// colors                            texture  struct         r/o      T1             t2      1 
// note_data                         texture  struct         r/o      T2             t3      1 
// root                              cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_VertexID              0   x           0   VERTID    uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float   xyzw
//
vs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][6], immediateIndexed, space=0
dcl_resource_structured T0[1:1], 576, space=0
dcl_resource_structured T1[2:2], 12, space=0
dcl_resource_structured T2[3:3], 12, space=0
dcl_input_sgv v0.x, vertex_id
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xyzw
dcl_output o3.xyzw
dcl_temps 4
ushr r0.x, v0.x, l(2)
and r0.y, v0.x, l(3)
ld_structured r0.xzw, r0.x, l(0), T2[3].xxyz
ubfe r1.x, l(8), l(8), r0.x
ushr r1.y, r0.x, l(16)
bfi r0.x, l(8), l(2), r0.x, l(0)
ld_structured r0.x, l(0), r0.x, T0[1].xxxx
ishl r1.yz, r1.yyxy, l(0, 4, 2, 0)
iadd r1.z, r1.z, l(512)
ld_structured r1.z, l(0), r1.z, T0[1].xxxx
add r2.x, r0.x, r1.z
div r0.x, r0.z, CB0[0][5].x
add r0.x, -r0.x, l(1.000000)
mad r0.x, CB0[0][4].z, r0.x, CB0[0][4].y
round_ne r2.y, r0.x
mul r0.x, CB0[0][4].w, l(0.650000)
mul r0.z, r0.w, CB0[0][4].z
div r0.z, r0.z, CB0[0][5].x
round_ne r0.z, r0.z
max r0.z, r0.z, l(0.000000)
add r0.z, r0.z, l(1.000000)
ieq r1.zw, r0.yyyy, l(0, 0, 1, 2)
or r0.w, r1.w, r1.z
ult r0.y, r0.y, l(2)
and r0.y, r0.y, l(0x3f800000)
mad r0.y, -r0.z, r0.y, r2.y
and r1.z, r0.w, l(0x3f800000)
mad r0.x, r0.x, r1.z, r2.x
iadd r1.x, r1.x, r1.y
ld_structured r1.y, r1.x, l(8), T1[2].xxxx
ieq r1.y, r1.y, l(-1)
mul r3.xyzw, r0.yyyy, CB0[0][1].xyzw
mad r3.xyzw, CB0[0][0].xyzw, r0.xxxx, r3.xyzw
add r3.xyzw, r3.xyzw, CB0[0][3].xyzw
movc o0.xyzw, r1.yyyy, l(0,0,0,0), r3.xyzw
bfi r0.x, l(1), l(2), r0.w, l(0)
ld_structured r0.x, r1.x, r0.x, T1[2].xxxx
ubfe r0.yw, l(0, 8, 0, 8), l(0, 16, 0, 8), r0.xxxx
utof r0.yw, r0.yyyw
mul o1.xy, r0.ywyy, l(0.003922, 0.003922, 0.000000, 0.000000)
and r0.y, r0.x, l(255)
utof r0.y, r0.y
ushr r0.x, r0.x, l(24)
utof r0.x, r0.x
mul o1.zw, r0.yyyx, l(0.000000, 0.000000, 0.003922, 0.003922)
ld_structured r0.x, r1.x, l(8), T1[2].xxxx
ubfe r0.yw, l(0, 8, 0, 8), l(0, 16, 0, 8), r0.xxxx
utof r0.yw, r0.yyyw
mul o3.xy, r0.ywyy, l(0.003922, 0.003922, 0.000000, 0.000000)
and r0.y, r0.x, l(255)
utof r0.y, r0.y
ushr r0.x, r0.x, l(24)
utof r0.x, r0.x
mul o3.zw, r0.yyyx, l(0.000000, 0.000000, 0.003922, 0.003922)
mad o2.z, CB0[0][4].w, l(0.650000), r2.x
add o2.w, -r0.z, r2.y
mov o2.xy, r2.xyxx
ret 
// Approximately 58 instruction slots used
#endif

const BYTE g_pNoteVertexShaderSameWidth[] =
{
     68,  88,  66,  67, 110,  85, 
    136,  11, 229, 104, 208,  88, 
     76,  26,  46,  31, 169, 174, 
     77, 118,   1,   0,   0,   0, 
    216,  14,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     36,   5,   0,   0,  88,   5, 
      0,   0, 228,   5,   0,   0, 
     60,  14,   0,   0,  82,  68, 
     69,  70, 232,   4,   0,   0, 
      4,   0,   0,   0, 248,   0, 
      0,   0,   4,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
    254, 255,   0,   5,   0,   0, 
    192,   4,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  64,   2, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 226,   0, 
      0,   0,   5,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,  12,   0,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 233,   0,   0,   0, 
      5,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
     12,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
    243,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 102, 105, 
    120, 101, 100,   0,  99, 111, 
    108, 111, 114, 115,   0, 110, 
    111, 116, 101,  95, 100,  97, 
    116,  97,   0, 114, 111, 111, 
    116,   0, 243,   0,   0,   0, 
      1,   0,   0,   0,  88,   1, 
      0,   0,  96,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 220,   0,   0,   0, 
      1,   0,   0,   0, 136,   2, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 226,   0,   0,   0, 
      1,   0,   0,   0,  92,   3, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 233,   0,   0,   0, 
      1,   0,   0,   0, 236,   3, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 243,   0,   0,   0, 
      0,   0,   0,   0,  84,   0, 
      0,   0,   2,   0,   0,   0, 
    100,   2,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     82, 111, 111, 116,  83, 105, 
    103, 110,  97, 116, 117, 114, 
    101,  68,  97, 116,  97,   0, 
    112, 114, 111, 106,   0, 102, 
    108, 111,  97, 116,  52, 120, 
     52,   0,   3,   0,   3,   0, 
      4,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 151,   1, 
      0,   0, 100, 101, 102, 108, 
     97, 116, 101,   0, 102, 108, 
    111,  97, 116,   0, 171, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 204,   1,   0,   0, 
    110, 111, 116, 101, 115,  95, 
    121,   0, 110, 111, 116, 101, 
    115,  95,  99, 121,   0, 119, 
    104, 105, 116, 101,  95,  99, 
    120,   0, 116, 105, 109, 101, 
    115, 112,  97, 110,   0, 171, 
    146,   1,   0,   0, 160,   1, 
      0,   0,   0,   0,   0,   0, 
    196,   1,   0,   0, 212,   1, 
      0,   0,  64,   0,   0,   0, 
    248,   1,   0,   0, 212,   1, 
      0,   0,  68,   0,   0,   0, 
      0,   2,   0,   0, 212,   1, 
      0,   0,  72,   0,   0,   0, 
      9,   2,   0,   0, 212,   1, 
      0,   0,  76,   0,   0,   0, 
     18,   2,   0,   0, 212,   1, 
      0,   0,  80,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
     21,   0,   0,   0,   6,   0, 
     28,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,   1,   0,   0, 
    176,   2,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      2,   0,   0,   0,  56,   3, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  36,  69, 
    108, 101, 109, 101, 110, 116, 
      0,  70, 105, 120, 101, 100, 
     83, 105, 122, 101,  68,  97, 
    116,  97,   0, 110, 111, 116, 
    101,  95, 120,   0, 171, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 204,   1,   0,   0, 
     98, 101, 110, 100, 115,   0, 
    171, 171,   0,   0,   3,   0, 
      1,   0,   1,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 204,   1, 
      0,   0, 199,   2,   0,   0, 
    208,   2,   0,   0,   0,   0, 
      0,   0, 244,   2,   0,   0, 
    252,   2,   0,   0,   0,   2, 
      0,   0,   5,   0,   0,   0, 
      1,   0, 144,   0,   0,   0, 
      2,   0,  32,   3,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 185,   2, 
      0,   0, 176,   2,   0,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0,   2,   0,   0,   0, 
    200,   3,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     84, 114,  97,  99, 107,  67, 
    111, 108, 111, 114,   0, 100, 
    119, 111, 114, 100,   0, 171, 
    171, 171,   0,   0,  19,   0, 
      1,   0,   1,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 143,   3, 
      0,   0, 226,   0,   0,   0, 
    152,   3,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      1,   0,   3,   0,   0,   0, 
      1,   0, 188,   3,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 132,   3, 
      0,   0, 176,   2,   0,   0, 
      0,   0,   0,   0,  12,   0, 
      0,   0,   2,   0,   0,   0, 
    156,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     78, 111, 116, 101,  68,  97, 
    116,  97,   0, 112,  97,  99, 
    107, 101, 100,   0,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    143,   3,   0,   0, 112, 111, 
    115,   0,   0,   0,   3,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 204,   1, 
      0,   0, 108, 101, 110, 103, 
    116, 104,   0, 171,  29,   4, 
      0,   0,  36,   4,   0,   0, 
      0,   0,   0,   0,  72,   4, 
      0,   0,  76,   4,   0,   0, 
      4,   0,   0,   0, 112,   4, 
      0,   0,  76,   4,   0,   0, 
      8,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   3,   0, 
      0,   0,   3,   0, 120,   4, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   4,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   1,   1,   0,   0, 
     83,  86,  95,  86, 101, 114, 
    116, 101, 120,  73,  68,   0, 
     79,  83,  71,  78, 132,   0, 
      0,   0,   4,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 116,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 122,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 116,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  83,  72,  69,  88, 
     80,   8,   0,   0,  81,   0, 
      1,   0,  20,   2,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0, 162,   0,   0,   7, 
     70, 126,  48,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  64,   2, 
      0,   0,   0,   0,   0,   0, 
    162,   0,   0,   7,  70, 126, 
     48,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0, 162,   0, 
      0,   7,  70, 126,  48,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
      0,   0,  96,   0,   0,   4, 
     18,  16,  16,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      3,   0,   0,   0, 104,   0, 
      0,   2,   4,   0,   0,   0, 
     85,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   3,   0, 
      0,   0, 167,   0,   0,  10, 
    210,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      6, 121,  32,   0,   2,   0, 
      0,   0,   3,   0,   0,   0, 
    138,   0,   0,   9,  18,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   8,   0, 
      0,   0,   1,  64,   0,   0, 
      8,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     85,   0,   0,   7,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     16,   0,   0,   0, 140,   0, 
      0,  11,  18,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   8,   0,   0,   0, 
      1,  64,   0,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,  10,  18,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   6, 112, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  41,   0, 
      0,  10,  98,   0,  16,   0, 
      1,   0,   0,   0,  86,   4, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,  30,   0,   0,   7, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   2,   0,   0, 
    167,   0,   0,  10,  66,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,   6, 112, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  14,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  10, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,  13,  18,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  26, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  64,   0,   0,   5, 
     34,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   9,  18,   0,  16,   0, 
      0,   0,   0,   0,  58, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
    102, 102,  38,  63,  56,   0, 
      0,   9,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     42, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  14,   0, 
      0,   9,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  64,   0, 
      0,   5,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     52,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  32,   0,   0,  10, 
    194,   0,  16,   0,   1,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     60,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  79,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     50,   0,   0,  10,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   7,  66,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  50,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   2,   0, 
      0,   0,  30,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
    167,   0,   0,  10,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      8,   0,   0,   0,   6, 112, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  32,   0, 
      0,   7,  34,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0, 255, 255, 
    255, 255,  56,   0,   0,   9, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  55,   0, 
      0,  12, 242,  32,  16,   0, 
      0,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0, 140,   0, 
      0,  11,  18,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   2,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
    167,   0,   0,  10,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   6, 112, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0, 138,   0, 
      0,  15, 162,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  86,   0, 
      0,   5, 162,   0,  16,   0, 
      0,   0,   0,   0,  86,  13, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,  10,  50,  32, 
     16,   0,   1,   0,   0,   0, 
    214,   5,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
    129, 128, 128,  59, 129, 128, 
    128,  59,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0, 255,   0, 
      0,   0,  86,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  85,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  24,   0, 
      0,   0,  86,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,  10, 194,  32,  16,   0, 
      1,   0,   0,   0,  86,   1, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    129, 128, 128,  59, 129, 128, 
    128,  59, 167,   0,   0,  10, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   8,   0,   0,   0, 
      6, 112,  32,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
    138,   0,   0,  15, 162,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   0,   0,   5, 162,   0, 
     16,   0,   0,   0,   0,   0, 
     86,  13,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,  10, 
     50,  32,  16,   0,   3,   0, 
      0,   0, 214,   5,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0, 129, 128, 128,  59, 
    129, 128, 128,  59,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
    255,   0,   0,   0,  86,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     85,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     24,   0,   0,   0,  86,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,  10, 194,  32, 
     16,   0,   3,   0,   0,   0, 
     86,   1,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 129, 128, 128,  59, 
    129, 128, 128,  59,  50,   0, 
      0,  11,  66,  32,  16,   0, 
      2,   0,   0,   0,  58, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
    102, 102,  38,  63,  10,   0, 
     16,   0,   2,   0,   0,   0, 
      0,   0,   0,   8, 130,  32, 
     16,   0,   2,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     50,  32,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  58,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  22,   0,   0,   0, 
      5,   0,   0,   0,  11,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
