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
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// root                              cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xy  
// COLOR                    0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][5], immediateIndexed, space=0
dcl_input_ps_siv linear noperspective v0.xy, position
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyzw
dcl_input_ps linear v3.xyzw
dcl_output o0.xyzw
dcl_temps 1
add r0.xyzw, v0.xxyy, -v2.xzyw
ge r0.xyzw, CB0[0][4].xxxx, |r0.xyzw|
or r0.x, r0.y, r0.x
or r0.x, r0.z, r0.x
or r0.x, r0.w, r0.x
movc o0.xyzw, r0.xxxx, v3.xyzw, v1.xyzw
ret 
// Approximately 7 instruction slots used
#endif

const BYTE g_pNotePixelShader[] =
{
     68,  88,  66,  67, 155, 168, 
     11,  94,   7,   1,  85, 102, 
     17, 214,  72, 184, 182, 114, 
      3, 103,   1,   0,   0,   0, 
    176,   4,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     24,   2,   0,   0, 164,   2, 
      0,   0, 216,   2,   0,   0, 
     20,   4,   0,   0,  82,  68, 
     69,  70, 220,   1,   0,   0, 
      1,   0,   0,   0, 108,   0, 
      0,   0,   1,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
    255, 255,   0,   5,   0,   0, 
    180,   1,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    100,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 114, 111, 
    111, 116,   0, 171, 171, 171, 
    100,   0,   0,   0,   1,   0, 
      0,   0, 132,   0,   0,   0, 
     96,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    100,   0,   0,   0,   0,   0, 
      0,   0,  84,   0,   0,   0, 
      2,   0,   0,   0, 144,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  82, 111, 
    111, 116,  83, 105, 103, 110, 
     97, 116, 117, 114, 101,  68, 
     97, 116,  97,   0, 112, 114, 
    111, 106,   0, 102, 108, 111, 
     97, 116,  52, 120,  52,   0, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 195,   0,   0,   0, 
    100, 101, 102, 108,  97, 116, 
    101,   0, 102, 108, 111,  97, 
    116,   0, 171, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    248,   0,   0,   0, 110, 111, 
    116, 101, 115,  95, 121,   0, 
    110, 111, 116, 101, 115,  95, 
     99, 121,   0, 119, 104, 105, 
    116, 101,  95,  99, 120,   0, 
    116, 105, 109, 101, 115, 112, 
     97, 110,   0, 171, 190,   0, 
      0,   0, 204,   0,   0,   0, 
      0,   0,   0,   0, 240,   0, 
      0,   0,   0,   1,   0,   0, 
     64,   0,   0,   0,  36,   1, 
      0,   0,   0,   1,   0,   0, 
     68,   0,   0,   0,  44,   1, 
      0,   0,   0,   1,   0,   0, 
     72,   0,   0,   0,  53,   1, 
      0,   0,   0,   1,   0,   0, 
     76,   0,   0,   0,  62,   1, 
      0,   0,   0,   1,   0,   0, 
     80,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  21,   0, 
      0,   0,   6,   0,  72,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    172,   0,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
    132,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   3,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,  15,   0,   0, 
    116,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,  15,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     65,  82,  71,  69,  84,   0, 
    171, 171,  83,  72,  69,  88, 
     52,   1,   0,   0,  81,   0, 
      0,   0,  77,   0,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0, 100,  32,   0,   4, 
     50,  16,  16,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   3,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   1,   0, 
      0,   0,   0,   0,   0,   8, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6,  21,  16,   0, 
      0,   0,   0,   0, 134,  29, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  29,   0, 
      0,  10, 242,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  70,  14,  16, 128, 
    129,   0,   0,   0,   0,   0, 
      0,   0,  60,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     60,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  60,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  55,   0,   0,   9, 
    242,  32,  16,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   3,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 148,   0, 
      0,   0,   7,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
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
      0,   0,   0,   0,   0,   0
};
