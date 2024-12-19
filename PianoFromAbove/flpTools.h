#pragma once
#include <PFXSTUDIO_Global_Imports.h>

void Make_FL_Event(vector<uint8_t>& FLdt_Data, uint8_t Value, vector<uint8_t> Data) {
    FLdt_Data.push_back(Value);
    if (Value < 1 << 6) { // int8
        Data.resize(1, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 1);
    }
    else if (Value < 1 << 7) { // int16
        Data.resize(2, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 2);
    }
    else if (Value < (1 << 8) - (1 << 6)) { // int32
        Data.resize(3, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 4);
    }
    else if (Value < (1 << 8) - (1 << 5)) { // text
        size_t DataSize = Data.size();
        while (DataSize >= 0x80) {
            FLdt_Data.push_back(static_cast<uint8_t>((DataSize & 0x7F) | 0x80));
            DataSize >>= 7;
        }
        FLdt_Data.push_back(static_cast<uint8_t>(DataSize));
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.end());
    }
    else { // data
        size_t DataSize = Data.size();
        while (DataSize >= 0x80) {
            FLdt_Data.push_back(static_cast<uint8_t>((DataSize & 0x7F) | 0x80));
            DataSize >>= 7;
        }
        FLdt_Data.push_back(static_cast<uint8_t>(DataSize));
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.end());
    }
}

vector<uint8_t> TO_BYTES_BIG_UINT8(uint64_t value, uint8_t length) {
    vector<uint8_t> bytes(length, 0);
    for (int i = length - 1; i >= 0; --i) {
        bytes[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
    return bytes;
}

vector<uint8_t> TO_BYTES_LITTLE_UINT8(uint64_t value, uint8_t length) {
    vector<uint8_t> bytes(length, 0);
    for (int i = 0; i < length; ++i) {
        bytes[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
    return bytes;
}

void CreateFLP(wstring Path, uint16_t PPQ) {
    ofstream Output_FLP(Path, ios::binary);
    vector<uint8_t> FLhd_Data;
    FLhd_Data.push_back(0x00);
    FLhd_Data.push_back(0x00);
    for (uint8_t NUM : TO_BYTES_LITTLE_UINT8(0, 2))
    {
        FLhd_Data.push_back(NUM);
    }
    for (uint8_t NUM : TO_BYTES_LITTLE_UINT8(PPQ, 2))
    {
        FLhd_Data.push_back(NUM);
    }
    vector<uint8_t> FLdt_Data;
    Make_FL_Event(FLdt_Data, 199, vector<uint8_t>{'1', '2', '.', '3', '.', '0', 0});
    Make_FL_Event(FLdt_Data, 93, vector<uint8_t>{0x00});
    Make_FL_Event(FLdt_Data, 66, vector<uint8_t>{0xFF, 0xFF});
    Make_FL_Event(FLdt_Data, 67, vector<uint8_t>{0x01});
    Make_FL_Event(FLdt_Data, 9, vector<uint8_t>{0x01});
    Make_FL_Event(FLdt_Data, 11, vector<uint8_t>{0x00});
    Make_FL_Event(FLdt_Data, 80, vector<uint8_t>{0x00});
    Make_FL_Event(FLdt_Data, 24, vector<uint8_t>{0x00});
    Make_FL_Event(FLdt_Data, 17, vector<uint8_t>{0x04});
    Make_FL_Event(FLdt_Data, 18, vector<uint8_t>{0x04});
    Make_FL_Event(FLdt_Data, 23, vector<uint8_t>{0x01});
    Make_FL_Event(FLdt_Data, 10, vector<uint8_t>{0x00});

    uint8_t ID_Plugin_New[24];
    for (uint8_t i = 0; i < 24; i++) ID_Plugin_New[i] = 0;
    ID_Plugin_New[8] = 2;
    ID_Plugin_New[16] = 16;
    uint8_t ID_Plugin_Parameters[965];
    for (uint16_t i = 0; i < 965; i++) ID_Plugin_Parameters[i] = 0;
    ID_Plugin_Parameters[0] = 4;
    for (uint8_t i = 8; i < 20; ++i) ID_Plugin_Parameters[i] = 255;
    ID_Plugin_Parameters[28] = 1; //Send MIDI pan/vol/pitch
    ID_Plugin_Parameters[30] = 1; //Instrument
    ID_Plugin_Parameters[323] = 1;
    ID_Plugin_Parameters[326] = 128;

    ID_Plugin_Parameters[335] = 0x7F; ID_Plugin_Parameters[336] = 0x00; ID_Plugin_Parameters[337] = 0x00; ID_Plugin_Parameters[338] = 0x00; ID_Plugin_Parameters[339] = 0xFF; ID_Plugin_Parameters[340] = 0xFF; ID_Plugin_Parameters[341] = 0xFF; ID_Plugin_Parameters[342] = 0xFF; ID_Plugin_Parameters[343] = 0x10; ID_Plugin_Parameters[344] = 0x4D; ID_Plugin_Parameters[345] = 0x6F; ID_Plugin_Parameters[346] = 0x64; ID_Plugin_Parameters[347] = 0x75; ID_Plugin_Parameters[348] = 0x6C; ID_Plugin_Parameters[349] = 0x61; ID_Plugin_Parameters[350] = 0x74; ID_Plugin_Parameters[351] = 0x69; ID_Plugin_Parameters[352] = 0x6F; ID_Plugin_Parameters[353] = 0x6E; ID_Plugin_Parameters[354] = 0x20; ID_Plugin_Parameters[355] = 0x77; ID_Plugin_Parameters[356] = 0x68; ID_Plugin_Parameters[357] = 0x65; ID_Plugin_Parameters[358] = 0x65; ID_Plugin_Parameters[359] = 0x6C; ID_Plugin_Parameters[360] = 0x03; ID_Plugin_Parameters[361] = 0x4D; ID_Plugin_Parameters[362] = 0x6F; ID_Plugin_Parameters[363] = 0x64; ID_Plugin_Parameters[364] = 0x01; ID_Plugin_Parameters[365] = 0x02; ID_Plugin_Parameters[366] = 0x00; ID_Plugin_Parameters[367] = 0x00; ID_Plugin_Parameters[368] = 0x80; ID_Plugin_Parameters[369] = 0x00; ID_Plugin_Parameters[370] = 0x00; ID_Plugin_Parameters[371] = 0x00; ID_Plugin_Parameters[372] = 0x00; ID_Plugin_Parameters[373] = 0x00; ID_Plugin_Parameters[374] = 0x00; ID_Plugin_Parameters[375] = 0x00; ID_Plugin_Parameters[376] = 0x00; ID_Plugin_Parameters[377] = 0x7F; ID_Plugin_Parameters[378] = 0x00; ID_Plugin_Parameters[379] = 0x00; ID_Plugin_Parameters[380] = 0x00; ID_Plugin_Parameters[381] = 0xFF; ID_Plugin_Parameters[382] = 0xFF; ID_Plugin_Parameters[383] = 0xFF; ID_Plugin_Parameters[384] = 0xFF; ID_Plugin_Parameters[385] = 0x0E; ID_Plugin_Parameters[386] = 0x42; ID_Plugin_Parameters[387] = 0x72; ID_Plugin_Parameters[388] = 0x65; ID_Plugin_Parameters[389] = 0x61; ID_Plugin_Parameters[390] = 0x74; ID_Plugin_Parameters[391] = 0x68; ID_Plugin_Parameters[392] = 0x20; ID_Plugin_Parameters[393] = 0x63; ID_Plugin_Parameters[394] = 0x6F; ID_Plugin_Parameters[395] = 0x6E; ID_Plugin_Parameters[396] = 0x74; ID_Plugin_Parameters[397] = 0x72; ID_Plugin_Parameters[398] = 0x6F; ID_Plugin_Parameters[399] = 0x6C; ID_Plugin_Parameters[400] = 0x06; ID_Plugin_Parameters[401] = 0x42; ID_Plugin_Parameters[402] = 0x72; ID_Plugin_Parameters[403] = 0x65; ID_Plugin_Parameters[404] = 0x61; ID_Plugin_Parameters[405] = 0x74; ID_Plugin_Parameters[406] = 0x68; ID_Plugin_Parameters[407] = 0x02; ID_Plugin_Parameters[408] = 0x04; ID_Plugin_Parameters[409] = 0x00; ID_Plugin_Parameters[410] = 0x00; ID_Plugin_Parameters[411] = 0x80; ID_Plugin_Parameters[412] = 0x00; ID_Plugin_Parameters[413] = 0x00; ID_Plugin_Parameters[414] = 0x00; ID_Plugin_Parameters[415] = 0x00; ID_Plugin_Parameters[416] = 0x00; ID_Plugin_Parameters[417] = 0x00; ID_Plugin_Parameters[418] = 0x00; ID_Plugin_Parameters[419] = 0x00; ID_Plugin_Parameters[420] = 0x7F; ID_Plugin_Parameters[421] = 0x00; ID_Plugin_Parameters[422] = 0x00; ID_Plugin_Parameters[423] = 0x00; ID_Plugin_Parameters[424] = 0xFF; ID_Plugin_Parameters[425] = 0xFF; ID_Plugin_Parameters[426] = 0xFF; ID_Plugin_Parameters[427] = 0xFF; ID_Plugin_Parameters[428] = 0x0F; ID_Plugin_Parameters[429] = 0x46; ID_Plugin_Parameters[430] = 0x6F; ID_Plugin_Parameters[431] = 0x6F; ID_Plugin_Parameters[432] = 0x74; ID_Plugin_Parameters[433] = 0x20; ID_Plugin_Parameters[434] = 0x63; ID_Plugin_Parameters[435] = 0x6F; ID_Plugin_Parameters[436] = 0x6E; ID_Plugin_Parameters[437] = 0x74; ID_Plugin_Parameters[438] = 0x72; ID_Plugin_Parameters[439] = 0x6F; ID_Plugin_Parameters[440] = 0x6C; ID_Plugin_Parameters[441] = 0x6C; ID_Plugin_Parameters[442] = 0x65; ID_Plugin_Parameters[443] = 0x72; ID_Plugin_Parameters[444] = 0x04; ID_Plugin_Parameters[445] = 0x46; ID_Plugin_Parameters[446] = 0x6F; ID_Plugin_Parameters[447] = 0x6F; ID_Plugin_Parameters[448] = 0x74; ID_Plugin_Parameters[449] = 0x03; ID_Plugin_Parameters[450] = 0x05; ID_Plugin_Parameters[451] = 0x00; ID_Plugin_Parameters[452] = 0x00; ID_Plugin_Parameters[453] = 0x80; ID_Plugin_Parameters[454] = 0x00; ID_Plugin_Parameters[455] = 0x00; ID_Plugin_Parameters[456] = 0x00; ID_Plugin_Parameters[457] = 0x00; ID_Plugin_Parameters[458] = 0x00; ID_Plugin_Parameters[459] = 0x00; ID_Plugin_Parameters[460] = 0x00; ID_Plugin_Parameters[461] = 0x00; ID_Plugin_Parameters[462] = 0x7F; ID_Plugin_Parameters[463] = 0x00; ID_Plugin_Parameters[464] = 0x00; ID_Plugin_Parameters[465] = 0x00; ID_Plugin_Parameters[466] = 0xFF; ID_Plugin_Parameters[467] = 0xFF; ID_Plugin_Parameters[468] = 0xFF; ID_Plugin_Parameters[469] = 0xFF; ID_Plugin_Parameters[470] = 0x0F; ID_Plugin_Parameters[471] = 0x50; ID_Plugin_Parameters[472] = 0x6F; ID_Plugin_Parameters[473] = 0x72; ID_Plugin_Parameters[474] = 0x74; ID_Plugin_Parameters[475] = 0x61; ID_Plugin_Parameters[476] = 0x6D; ID_Plugin_Parameters[477] = 0x65; ID_Plugin_Parameters[478] = 0x6E; ID_Plugin_Parameters[479] = 0x74; ID_Plugin_Parameters[480] = 0x6F; ID_Plugin_Parameters[481] = 0x20; ID_Plugin_Parameters[482] = 0x74; ID_Plugin_Parameters[483] = 0x69; ID_Plugin_Parameters[484] = 0x6D; ID_Plugin_Parameters[485] = 0x65; ID_Plugin_Parameters[486] = 0x0A; ID_Plugin_Parameters[487] = 0x50; ID_Plugin_Parameters[488] = 0x6F; ID_Plugin_Parameters[489] = 0x72; ID_Plugin_Parameters[490] = 0x74; ID_Plugin_Parameters[491] = 0x2E; ID_Plugin_Parameters[492] = 0x20; ID_Plugin_Parameters[493] = 0x54; ID_Plugin_Parameters[494] = 0x69; ID_Plugin_Parameters[495] = 0x6D; ID_Plugin_Parameters[496] = 0x65; ID_Plugin_Parameters[497] = 0x04; ID_Plugin_Parameters[498] = 0x5B; ID_Plugin_Parameters[499] = 0x00; ID_Plugin_Parameters[500] = 0x00; ID_Plugin_Parameters[501] = 0x80; ID_Plugin_Parameters[502] = 0x00; ID_Plugin_Parameters[503] = 0x00; ID_Plugin_Parameters[504] = 0x00; ID_Plugin_Parameters[505] = 0x00; ID_Plugin_Parameters[506] = 0x00; ID_Plugin_Parameters[507] = 0x00; ID_Plugin_Parameters[508] = 0x00; ID_Plugin_Parameters[509] = 0x00; ID_Plugin_Parameters[510] = 0x7F; ID_Plugin_Parameters[511] = 0x00; ID_Plugin_Parameters[512] = 0x00; ID_Plugin_Parameters[513] = 0x00; ID_Plugin_Parameters[514] = 0xFF; ID_Plugin_Parameters[515] = 0xFF; ID_Plugin_Parameters[516] = 0xFF; ID_Plugin_Parameters[517] = 0xFF; ID_Plugin_Parameters[518] = 0x11; ID_Plugin_Parameters[519] = 0x52; ID_Plugin_Parameters[520] = 0x65; ID_Plugin_Parameters[521] = 0x76; ID_Plugin_Parameters[522] = 0x65; ID_Plugin_Parameters[523] = 0x72; ID_Plugin_Parameters[524] = 0x62; ID_Plugin_Parameters[525] = 0x20; ID_Plugin_Parameters[526] = 0x73; ID_Plugin_Parameters[527] = 0x65; ID_Plugin_Parameters[528] = 0x6E; ID_Plugin_Parameters[529] = 0x64; ID_Plugin_Parameters[530] = 0x20; ID_Plugin_Parameters[531] = 0x6C; ID_Plugin_Parameters[532] = 0x65; ID_Plugin_Parameters[533] = 0x76; ID_Plugin_Parameters[534] = 0x65; ID_Plugin_Parameters[535] = 0x6C; ID_Plugin_Parameters[536] = 0x06; ID_Plugin_Parameters[537] = 0x52; ID_Plugin_Parameters[538] = 0x65; ID_Plugin_Parameters[539] = 0x76; ID_Plugin_Parameters[540] = 0x65; ID_Plugin_Parameters[541] = 0x72; ID_Plugin_Parameters[542] = 0x62; ID_Plugin_Parameters[543] = 0x05; ID_Plugin_Parameters[544] = 0x5D; ID_Plugin_Parameters[545] = 0x00; ID_Plugin_Parameters[546] = 0x00; ID_Plugin_Parameters[547] = 0x80; ID_Plugin_Parameters[548] = 0x00; ID_Plugin_Parameters[549] = 0x00; ID_Plugin_Parameters[550] = 0x00; ID_Plugin_Parameters[551] = 0x00; ID_Plugin_Parameters[552] = 0x00; ID_Plugin_Parameters[553] = 0x00; ID_Plugin_Parameters[554] = 0x00; ID_Plugin_Parameters[555] = 0x00; ID_Plugin_Parameters[556] = 0x7F; ID_Plugin_Parameters[557] = 0x00; ID_Plugin_Parameters[558] = 0x00; ID_Plugin_Parameters[559] = 0x00; ID_Plugin_Parameters[560] = 0xFF; ID_Plugin_Parameters[561] = 0xFF; ID_Plugin_Parameters[562] = 0xFF; ID_Plugin_Parameters[563] = 0xFF; ID_Plugin_Parameters[564] = 0x11; ID_Plugin_Parameters[565] = 0x43; ID_Plugin_Parameters[566] = 0x68; ID_Plugin_Parameters[567] = 0x6F; ID_Plugin_Parameters[568] = 0x72; ID_Plugin_Parameters[569] = 0x75; ID_Plugin_Parameters[570] = 0x73; ID_Plugin_Parameters[571] = 0x20; ID_Plugin_Parameters[572] = 0x73; ID_Plugin_Parameters[573] = 0x65; ID_Plugin_Parameters[574] = 0x6E; ID_Plugin_Parameters[575] = 0x64; ID_Plugin_Parameters[576] = 0x20; ID_Plugin_Parameters[577] = 0x6C; ID_Plugin_Parameters[578] = 0x65; ID_Plugin_Parameters[579] = 0x76; ID_Plugin_Parameters[580] = 0x65; ID_Plugin_Parameters[581] = 0x6C; ID_Plugin_Parameters[582] = 0x06; ID_Plugin_Parameters[583] = 0x43; ID_Plugin_Parameters[584] = 0x68; ID_Plugin_Parameters[585] = 0x6F; ID_Plugin_Parameters[586] = 0x72; ID_Plugin_Parameters[587] = 0x75; ID_Plugin_Parameters[588] = 0x73; ID_Plugin_Parameters[589] = 0x09; ID_Plugin_Parameters[590] = 0x40; ID_Plugin_Parameters[591] = 0x00; ID_Plugin_Parameters[592] = 0x00; ID_Plugin_Parameters[593] = 0x80; ID_Plugin_Parameters[594] = 0x00; ID_Plugin_Parameters[595] = 0x00; ID_Plugin_Parameters[596] = 0x00; ID_Plugin_Parameters[597] = 0x00; ID_Plugin_Parameters[598] = 0x00; ID_Plugin_Parameters[599] = 0x00; ID_Plugin_Parameters[600] = 0x00; ID_Plugin_Parameters[601] = 0x00; ID_Plugin_Parameters[602] = 0x7F; ID_Plugin_Parameters[603] = 0x00; ID_Plugin_Parameters[604] = 0x00; ID_Plugin_Parameters[605] = 0x00; ID_Plugin_Parameters[606] = 0xFF; ID_Plugin_Parameters[607] = 0xFF; ID_Plugin_Parameters[608] = 0xFF; ID_Plugin_Parameters[609] = 0xFF; ID_Plugin_Parameters[610] = 0x0C; ID_Plugin_Parameters[611] = 0x44; ID_Plugin_Parameters[612] = 0x61; ID_Plugin_Parameters[613] = 0x6D; ID_Plugin_Parameters[614] = 0x70; ID_Plugin_Parameters[615] = 0x65; ID_Plugin_Parameters[616] = 0x72; ID_Plugin_Parameters[617] = 0x20; ID_Plugin_Parameters[618] = 0x70; ID_Plugin_Parameters[619] = 0x65; ID_Plugin_Parameters[620] = 0x64; ID_Plugin_Parameters[621] = 0x61; ID_Plugin_Parameters[622] = 0x6C; ID_Plugin_Parameters[623] = 0x06; ID_Plugin_Parameters[624] = 0x44; ID_Plugin_Parameters[625] = 0x61; ID_Plugin_Parameters[626] = 0x6D; ID_Plugin_Parameters[627] = 0x70; ID_Plugin_Parameters[628] = 0x65; ID_Plugin_Parameters[629] = 0x72; ID_Plugin_Parameters[630] = 0x0A; ID_Plugin_Parameters[631] = 0x41; ID_Plugin_Parameters[632] = 0x00; ID_Plugin_Parameters[633] = 0x00; ID_Plugin_Parameters[634] = 0x80; ID_Plugin_Parameters[635] = 0x00; ID_Plugin_Parameters[636] = 0x00; ID_Plugin_Parameters[637] = 0x00; ID_Plugin_Parameters[638] = 0x00; ID_Plugin_Parameters[639] = 0x00; ID_Plugin_Parameters[640] = 0x00; ID_Plugin_Parameters[641] = 0x00; ID_Plugin_Parameters[642] = 0x00; ID_Plugin_Parameters[643] = 0x7F; ID_Plugin_Parameters[644] = 0x00; ID_Plugin_Parameters[645] = 0x00; ID_Plugin_Parameters[646] = 0x00; ID_Plugin_Parameters[647] = 0xFF; ID_Plugin_Parameters[648] = 0xFF; ID_Plugin_Parameters[649] = 0xFF; ID_Plugin_Parameters[650] = 0xFF; ID_Plugin_Parameters[651] = 0x0A; ID_Plugin_Parameters[652] = 0x50; ID_Plugin_Parameters[653] = 0x6F; ID_Plugin_Parameters[654] = 0x72; ID_Plugin_Parameters[655] = 0x74; ID_Plugin_Parameters[656] = 0x61; ID_Plugin_Parameters[657] = 0x6D; ID_Plugin_Parameters[658] = 0x65; ID_Plugin_Parameters[659] = 0x6E; ID_Plugin_Parameters[660] = 0x74; ID_Plugin_Parameters[661] = 0x6F; ID_Plugin_Parameters[662] = 0x06; ID_Plugin_Parameters[663] = 0x50; ID_Plugin_Parameters[664] = 0x6F; ID_Plugin_Parameters[665] = 0x72; ID_Plugin_Parameters[666] = 0x74; ID_Plugin_Parameters[667] = 0x61; ID_Plugin_Parameters[668] = 0x2E; ID_Plugin_Parameters[669] = 0x0B; ID_Plugin_Parameters[670] = 0x42; ID_Plugin_Parameters[671] = 0x00; ID_Plugin_Parameters[672] = 0x00; ID_Plugin_Parameters[673] = 0x80; ID_Plugin_Parameters[674] = 0x00; ID_Plugin_Parameters[675] = 0x00; ID_Plugin_Parameters[676] = 0x00; ID_Plugin_Parameters[677] = 0x00; ID_Plugin_Parameters[678] = 0x00; ID_Plugin_Parameters[679] = 0x00; ID_Plugin_Parameters[680] = 0x00; ID_Plugin_Parameters[681] = 0x00; ID_Plugin_Parameters[682] = 0x7F; ID_Plugin_Parameters[683] = 0x00; ID_Plugin_Parameters[684] = 0x00; ID_Plugin_Parameters[685] = 0x00; ID_Plugin_Parameters[686] = 0xFF; ID_Plugin_Parameters[687] = 0xFF; ID_Plugin_Parameters[688] = 0xFF; ID_Plugin_Parameters[689] = 0xFF; ID_Plugin_Parameters[690] = 0x09; ID_Plugin_Parameters[691] = 0x53; ID_Plugin_Parameters[692] = 0x75; ID_Plugin_Parameters[693] = 0x73; ID_Plugin_Parameters[694] = 0x74; ID_Plugin_Parameters[695] = 0x65; ID_Plugin_Parameters[696] = 0x6E; ID_Plugin_Parameters[697] = 0x75; ID_Plugin_Parameters[698] = 0x74; ID_Plugin_Parameters[699] = 0x6F; ID_Plugin_Parameters[700] = 0x09; ID_Plugin_Parameters[701] = 0x53; ID_Plugin_Parameters[702] = 0x75; ID_Plugin_Parameters[703] = 0x73; ID_Plugin_Parameters[704] = 0x74; ID_Plugin_Parameters[705] = 0x65; ID_Plugin_Parameters[706] = 0x6E; ID_Plugin_Parameters[707] = 0x75; ID_Plugin_Parameters[708] = 0x74; ID_Plugin_Parameters[709] = 0x6F; ID_Plugin_Parameters[710] = 0x0C; ID_Plugin_Parameters[711] = 0x43; ID_Plugin_Parameters[712] = 0x00; ID_Plugin_Parameters[713] = 0x00; ID_Plugin_Parameters[714] = 0x80; ID_Plugin_Parameters[715] = 0x00; ID_Plugin_Parameters[716] = 0x00; ID_Plugin_Parameters[717] = 0x00; ID_Plugin_Parameters[718] = 0x00; ID_Plugin_Parameters[719] = 0x00; ID_Plugin_Parameters[720] = 0x00; ID_Plugin_Parameters[721] = 0x00; ID_Plugin_Parameters[722] = 0x00; ID_Plugin_Parameters[723] = 0x7F; ID_Plugin_Parameters[724] = 0x00; ID_Plugin_Parameters[725] = 0x00; ID_Plugin_Parameters[726] = 0x00; ID_Plugin_Parameters[727] = 0xFF; ID_Plugin_Parameters[728] = 0xFF; ID_Plugin_Parameters[729] = 0xFF; ID_Plugin_Parameters[730] = 0xFF; ID_Plugin_Parameters[731] = 0x0A; ID_Plugin_Parameters[732] = 0x53; ID_Plugin_Parameters[733] = 0x6F; ID_Plugin_Parameters[734] = 0x66; ID_Plugin_Parameters[735] = 0x74; ID_Plugin_Parameters[736] = 0x20; ID_Plugin_Parameters[737] = 0x70; ID_Plugin_Parameters[738] = 0x65; ID_Plugin_Parameters[739] = 0x64; ID_Plugin_Parameters[740] = 0x61; ID_Plugin_Parameters[741] = 0x6C; ID_Plugin_Parameters[742] = 0x0A; ID_Plugin_Parameters[743] = 0x53; ID_Plugin_Parameters[744] = 0x6F; ID_Plugin_Parameters[745] = 0x66; ID_Plugin_Parameters[746] = 0x74; ID_Plugin_Parameters[747] = 0x20; ID_Plugin_Parameters[748] = 0x70; ID_Plugin_Parameters[749] = 0x65; ID_Plugin_Parameters[750] = 0x64; ID_Plugin_Parameters[751] = 0x61; ID_Plugin_Parameters[752] = 0x6C; ID_Plugin_Parameters[753] = 0x0D; ID_Plugin_Parameters[754] = 0x44; ID_Plugin_Parameters[755] = 0x00; ID_Plugin_Parameters[756] = 0x00; ID_Plugin_Parameters[757] = 0x80; ID_Plugin_Parameters[758] = 0x00; ID_Plugin_Parameters[759] = 0x00; ID_Plugin_Parameters[760] = 0x00; ID_Plugin_Parameters[761] = 0x00; ID_Plugin_Parameters[762] = 0x00; ID_Plugin_Parameters[763] = 0x00; ID_Plugin_Parameters[764] = 0x00; ID_Plugin_Parameters[765] = 0x00; ID_Plugin_Parameters[766] = 0x7F; ID_Plugin_Parameters[767] = 0x00; ID_Plugin_Parameters[768] = 0x00; ID_Plugin_Parameters[769] = 0x00; ID_Plugin_Parameters[770] = 0xFF; ID_Plugin_Parameters[771] = 0xFF; ID_Plugin_Parameters[772] = 0xFF; ID_Plugin_Parameters[773] = 0xFF; ID_Plugin_Parameters[774] = 0x11; ID_Plugin_Parameters[775] = 0x4C; ID_Plugin_Parameters[776] = 0x65; ID_Plugin_Parameters[777] = 0x67; ID_Plugin_Parameters[778] = 0x61; ID_Plugin_Parameters[779] = 0x74; ID_Plugin_Parameters[780] = 0x6F; ID_Plugin_Parameters[781] = 0x20; ID_Plugin_Parameters[782] = 0x46; ID_Plugin_Parameters[783] = 0x6F; ID_Plugin_Parameters[784] = 0x6F; ID_Plugin_Parameters[785] = 0x74; ID_Plugin_Parameters[786] = 0x73; ID_Plugin_Parameters[787] = 0x77; ID_Plugin_Parameters[788] = 0x69; ID_Plugin_Parameters[789] = 0x74; ID_Plugin_Parameters[790] = 0x63; ID_Plugin_Parameters[791] = 0x68; ID_Plugin_Parameters[792] = 0x06; ID_Plugin_Parameters[793] = 0x4C; ID_Plugin_Parameters[794] = 0x65; ID_Plugin_Parameters[795] = 0x67; ID_Plugin_Parameters[796] = 0x61; ID_Plugin_Parameters[797] = 0x74; ID_Plugin_Parameters[798] = 0x6F; ID_Plugin_Parameters[799] = 0x0E; ID_Plugin_Parameters[800] = 0x45; ID_Plugin_Parameters[801] = 0x00; ID_Plugin_Parameters[802] = 0x00; ID_Plugin_Parameters[803] = 0x80; ID_Plugin_Parameters[804] = 0x00; ID_Plugin_Parameters[805] = 0x00; ID_Plugin_Parameters[806] = 0x00; ID_Plugin_Parameters[807] = 0x00; ID_Plugin_Parameters[808] = 0x00; ID_Plugin_Parameters[809] = 0x00; ID_Plugin_Parameters[810] = 0x00; ID_Plugin_Parameters[811] = 0x00; ID_Plugin_Parameters[812] = 0x7F; ID_Plugin_Parameters[813] = 0x00; ID_Plugin_Parameters[814] = 0x00; ID_Plugin_Parameters[815] = 0x00; ID_Plugin_Parameters[816] = 0xFF; ID_Plugin_Parameters[817] = 0xFF; ID_Plugin_Parameters[818] = 0xFF; ID_Plugin_Parameters[819] = 0xFF; ID_Plugin_Parameters[820] = 0x06; ID_Plugin_Parameters[821] = 0x48; ID_Plugin_Parameters[822] = 0x6F; ID_Plugin_Parameters[823] = 0x6C; ID_Plugin_Parameters[824] = 0x64; ID_Plugin_Parameters[825] = 0x20; ID_Plugin_Parameters[826] = 0x32; ID_Plugin_Parameters[827] = 0x06; ID_Plugin_Parameters[828] = 0x48; ID_Plugin_Parameters[829] = 0x6F; ID_Plugin_Parameters[830] = 0x6C; ID_Plugin_Parameters[831] = 0x64; ID_Plugin_Parameters[832] = 0x20; ID_Plugin_Parameters[833] = 0x32; ID_Plugin_Parameters[834] = 0x12; ID_Plugin_Parameters[835] = 0x00; ID_Plugin_Parameters[836] = 0x00; ID_Plugin_Parameters[837] = 0x00; ID_Plugin_Parameters[838] = 0x80; ID_Plugin_Parameters[839] = 0x01; ID_Plugin_Parameters[840] = 0x00; ID_Plugin_Parameters[841] = 0x00; ID_Plugin_Parameters[842] = 0x00; ID_Plugin_Parameters[843] = 0x00; ID_Plugin_Parameters[844] = 0x00; ID_Plugin_Parameters[845] = 0x00; ID_Plugin_Parameters[846] = 0x00; ID_Plugin_Parameters[847] = 0x7F; ID_Plugin_Parameters[848] = 0x00; ID_Plugin_Parameters[849] = 0x00; ID_Plugin_Parameters[850] = 0x00; ID_Plugin_Parameters[851] = 0xFF; ID_Plugin_Parameters[852] = 0xFF; ID_Plugin_Parameters[853] = 0xFF; ID_Plugin_Parameters[854] = 0xFF; ID_Plugin_Parameters[855] = 0x16; ID_Plugin_Parameters[856] = 0x50; ID_Plugin_Parameters[857] = 0x69; ID_Plugin_Parameters[858] = 0x74; ID_Plugin_Parameters[859] = 0x63; ID_Plugin_Parameters[860] = 0x68; ID_Plugin_Parameters[861] = 0x20; ID_Plugin_Parameters[862] = 0x42; ID_Plugin_Parameters[863] = 0x65; ID_Plugin_Parameters[864] = 0x6E; ID_Plugin_Parameters[865] = 0x64; ID_Plugin_Parameters[866] = 0x20; ID_Plugin_Parameters[867] = 0x53; ID_Plugin_Parameters[868] = 0x65; ID_Plugin_Parameters[869] = 0x6E; ID_Plugin_Parameters[870] = 0x73; ID_Plugin_Parameters[871] = 0x69; ID_Plugin_Parameters[872] = 0x74; ID_Plugin_Parameters[873] = 0x69; ID_Plugin_Parameters[874] = 0x76; ID_Plugin_Parameters[875] = 0x69; ID_Plugin_Parameters[876] = 0x74; ID_Plugin_Parameters[877] = 0x79; ID_Plugin_Parameters[878] = 0x03; ID_Plugin_Parameters[879] = 0x50; ID_Plugin_Parameters[880] = 0x42; ID_Plugin_Parameters[881] = 0x53; ID_Plugin_Parameters[882] = 0xFF; ID_Plugin_Parameters[883] = 0x11; ID_Plugin_Parameters[884] = 0x42; ID_Plugin_Parameters[885] = 0x61; ID_Plugin_Parameters[886] = 0x73; ID_Plugin_Parameters[887] = 0x69; ID_Plugin_Parameters[888] = 0x63; ID_Plugin_Parameters[889] = 0x20; ID_Plugin_Parameters[890] = 0x63; ID_Plugin_Parameters[891] = 0x6F; ID_Plugin_Parameters[892] = 0x6E; ID_Plugin_Parameters[893] = 0x74; ID_Plugin_Parameters[894] = 0x72; ID_Plugin_Parameters[895] = 0x6F; ID_Plugin_Parameters[896] = 0x6C; ID_Plugin_Parameters[897] = 0x6C; ID_Plugin_Parameters[898] = 0x65; ID_Plugin_Parameters[899] = 0x72; ID_Plugin_Parameters[900] = 0x73; ID_Plugin_Parameters[901] = 0x13; ID_Plugin_Parameters[902] = 0x42; ID_Plugin_Parameters[903] = 0x6F; ID_Plugin_Parameters[904] = 0x6F; ID_Plugin_Parameters[905] = 0x6C; ID_Plugin_Parameters[906] = 0x65; ID_Plugin_Parameters[907] = 0x61; ID_Plugin_Parameters[908] = 0x6E; ID_Plugin_Parameters[909] = 0x20; ID_Plugin_Parameters[910] = 0x63; ID_Plugin_Parameters[911] = 0x6F; ID_Plugin_Parameters[912] = 0x6E; ID_Plugin_Parameters[913] = 0x74; ID_Plugin_Parameters[914] = 0x72; ID_Plugin_Parameters[915] = 0x6F; ID_Plugin_Parameters[916] = 0x6C; ID_Plugin_Parameters[917] = 0x6C; ID_Plugin_Parameters[918] = 0x65; ID_Plugin_Parameters[919] = 0x72; ID_Plugin_Parameters[920] = 0x73; ID_Plugin_Parameters[921] = 0x07; ID_Plugin_Parameters[922] = 0x53; ID_Plugin_Parameters[923] = 0x70; ID_Plugin_Parameters[924] = 0x65; ID_Plugin_Parameters[925] = 0x63; ID_Plugin_Parameters[926] = 0x69; ID_Plugin_Parameters[927] = 0x61; ID_Plugin_Parameters[928] = 0x6C; ID_Plugin_Parameters[929] = 0x06; ID_Plugin_Parameters[930] = 0x50; ID_Plugin_Parameters[931] = 0x61; ID_Plugin_Parameters[932] = 0x67; ID_Plugin_Parameters[933] = 0x65; ID_Plugin_Parameters[934] = 0x20; ID_Plugin_Parameters[935] = 0x34; ID_Plugin_Parameters[936] = 0x06; ID_Plugin_Parameters[937] = 0x50; ID_Plugin_Parameters[938] = 0x61; ID_Plugin_Parameters[939] = 0x67; ID_Plugin_Parameters[940] = 0x65; ID_Plugin_Parameters[941] = 0x20; ID_Plugin_Parameters[942] = 0x35; ID_Plugin_Parameters[943] = 0x06; ID_Plugin_Parameters[944] = 0x50; ID_Plugin_Parameters[945] = 0x61; ID_Plugin_Parameters[946] = 0x67; ID_Plugin_Parameters[947] = 0x65; ID_Plugin_Parameters[948] = 0x20; ID_Plugin_Parameters[949] = 0x36; ID_Plugin_Parameters[950] = 0x06; ID_Plugin_Parameters[951] = 0x50; ID_Plugin_Parameters[952] = 0x61; ID_Plugin_Parameters[953] = 0x67; ID_Plugin_Parameters[954] = 0x65; ID_Plugin_Parameters[955] = 0x20; ID_Plugin_Parameters[956] = 0x37; ID_Plugin_Parameters[957] = 0x06; ID_Plugin_Parameters[958] = 0x50; ID_Plugin_Parameters[959] = 0x61; ID_Plugin_Parameters[960] = 0x67; ID_Plugin_Parameters[961] = 0x65; ID_Plugin_Parameters[962] = 0x20; ID_Plugin_Parameters[963] = 0x38; ID_Plugin_Parameters[964] = 0x00;

    uint8_t ID_Channel_Parameters[125];
    for (uint8_t i = 0; i < 125; i++) ID_Channel_Parameters[i] = 0;
    ID_Channel_Parameters[64] = 0xFF;
    ID_Channel_Parameters[65] = 0xFF;
    ID_Channel_Parameters[73] = 0xFF;
    ID_Channel_Parameters[112] = 0x0C;
    ID_Channel_Parameters[116] = 0xFE;
    ID_Channel_Parameters[117] = 0xFF;
    ID_Channel_Parameters[118] = 0xFF;
    ID_Channel_Parameters[119] = 0xFF;
    ID_Channel_Parameters[120] = 0xFF;
    ID_Channel_Parameters[121] = 0xFF;
    ID_Channel_Parameters[122] = 0xFF;
    ID_Channel_Parameters[123] = 0xFF;

    for (uint16_t Channel = 0; Channel < 16; Channel++) {
        ID_Plugin_Parameters[4] = Channel >= 9 ? Channel + 1 : Channel; //MIDI output channel
        Make_FL_Event(FLdt_Data, 64, TO_BYTES_LITTLE_UINT8(static_cast<uint16_t>(Channel), 2));
        Make_FL_Event(FLdt_Data, 21, vector<uint8_t>{0x02});
        Make_FL_Event(FLdt_Data, 201, vector<uint8_t>{'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0, 0, 0});
        Make_FL_Event(FLdt_Data, 212, vector<uint8_t>(ID_Plugin_New, ID_Plugin_New + 24));
        wstring TrackName = L"MIDI Out -> Ch";
        TrackName += to_wstring(Channel >= 9 ? Channel + 2 : Channel + 1);
        TrackName += (wchar_t)0x0000;
        Make_FL_Event(FLdt_Data, 192, vector<uint8_t>(reinterpret_cast<const uint8_t*>(TrackName.data()), reinterpret_cast<const uint8_t*>(TrackName.data() + TrackName.size())));
        Make_FL_Event(FLdt_Data, 213, vector<uint8_t>(ID_Plugin_Parameters, ID_Plugin_Parameters + 965));
        if (Channel < 15) Make_FL_Event(FLdt_Data, 215, vector<uint8_t>(ID_Channel_Parameters, ID_Channel_Parameters + 125));
    }

    for (uint16_t i = 0; i < 384; i++) ID_Plugin_Parameters[i] = 0;
    ID_Plugin_Parameters[0] = 4;
    for (uint8_t i = 8; i < 20; ++i) ID_Plugin_Parameters[i] = 255;
    ID_Plugin_Parameters[4] = 0; //MIDI output channel
    ID_Plugin_Parameters[29] = 1; //Map note color to midi channel
    ID_Plugin_Parameters[322] = 255;

    for (uint16_t Channel = 15; Channel < 25; Channel++) {
        Make_FL_Event(FLdt_Data, 64, TO_BYTES_LITTLE_UINT8(static_cast<uint16_t>(Channel), 2));
        Make_FL_Event(FLdt_Data, 21, vector<uint8_t>{0x02});
        Make_FL_Event(FLdt_Data, 201, vector<uint8_t>{'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0, 0, 0});
        Make_FL_Event(FLdt_Data, 212, vector<uint8_t>(ID_Plugin_New, ID_Plugin_New + 24));
        wstring TrackName = L"MIDI Out -> Any";
        TrackName += (wchar_t)0x0000;
        Make_FL_Event(FLdt_Data, 192, vector<uint8_t>(reinterpret_cast<const uint8_t*>(TrackName.data()), reinterpret_cast<const uint8_t*>(TrackName.data() + TrackName.size())));
        Make_FL_Event(FLdt_Data, 213, vector<uint8_t>(ID_Plugin_Parameters, ID_Plugin_Parameters + 384));
    }

    for (uint8_t Channel = 0; Channel < 25; Channel++) {
    }

    Output_FLP.write("FLhd", 4);
    uint32_t data_FLhd_size = FLhd_Data.size();
    Output_FLP.write(reinterpret_cast<char*>(&data_FLhd_size), 4);
    Output_FLP.write(reinterpret_cast<char*>(FLhd_Data.data()), FLhd_Data.size());

    Output_FLP.write("FLdt", 4);
    uint32_t data_FLdt_size = FLdt_Data.size();
    Output_FLP.write(reinterpret_cast<char*>(&data_FLdt_size), 4);
    Output_FLP.write(reinterpret_cast<char*>(FLdt_Data.data()), FLdt_Data.size());
    Output_FLP.close();
}