#pragma once
#include <PFXSTUDIO_Global_Imports.h>

void Make_FL_Event(vector<uint8_t>& FLdt_Data, uint8_t Value, vector<uint8_t> Data) {
    FLdt_Data.push_back(Value);
    if (Value < (1<<6)) { // int8
        Data.resize(1, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 1);
    }
    else if (Value < (1<<7)) { // int16
        Data.resize(2, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 2);
    }
    else if (Value < ((1<<8)-(1<<6))) { // int32
        Data.resize(4, 0); // Fill the rest of the space with 0 if the data length is shorter than what we expect. 
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.begin() + 4);
    }
    else { // text
        size_t DataSize = Data.size();
        while (DataSize >= 0x80) {
            FLdt_Data.push_back(static_cast<uint8_t>((DataSize & 0x7F) | 0x80));
            DataSize >>= 7;
        }
        FLdt_Data.push_back(static_cast<uint8_t>(DataSize & 0x7F));
        FLdt_Data.insert(FLdt_Data.end(), Data.begin(), Data.end());
    }
}

template <typename Unknown>
vector<uint8_t> ToBytes(Unknown value, uint8_t length) {
    vector<uint8_t> bytes(length, 0);
    for (uint8_t i = 0; i < length; i++) {
        bytes[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }
    return bytes;
}

void SetKnob(uint8_t* ParameterList, uint32_t Value, size_t Offset) {
    ParameterList[Offset + 3] = static_cast<uint8_t>((0xFF000000 & Value) >> 24);
    ParameterList[Offset + 2] = static_cast<uint8_t>((0x00FF0000 & Value) >> 16);
    ParameterList[Offset + 1] = static_cast<uint8_t>((0x0000FF00 & Value) >> 8);
    ParameterList[Offset] = static_cast<uint8_t>(0x000000FF & Value);
}

void SetKnobAuto(uint8_t* ParameterList, uint32_t Value, size_t Page, size_t KnobID) {
    SetKnob(ParameterList, Value, Page * 9 * 4 + KnobID * 4 + 34);
}

void CreateKnob(uint8_t* ParameterList, uint8_t KnobID, uint32_t ControllerID, uint32_t ControllerType, uint32_t MinValue, uint32_t MaxValue, uint32_t Channel, string LongName, string ShortName, size_t& Offset) {
    ParameterList[Offset] = KnobID;
    ParameterList[Offset + 4] = static_cast<uint8_t>((0xFF000000 & ControllerID) >> 24);
    ParameterList[Offset + 3] = static_cast<uint8_t>((0x00FF0000 & ControllerID) >> 16);
    ParameterList[Offset + 2] = static_cast<uint8_t>((0x0000FF00 & ControllerID) >> 8);
    ParameterList[Offset + 1] = static_cast<uint8_t>(0x000000FF & ControllerID);
    ParameterList[Offset + 8] = static_cast<uint8_t>((0xFF000000 & ControllerType) >> 24);
    ParameterList[Offset + 7] = static_cast<uint8_t>((0x00FF0000 & ControllerType) >> 16);
    ParameterList[Offset + 6] = static_cast<uint8_t>((0x0000FF00 & ControllerType) >> 8);
    ParameterList[Offset + 5] = static_cast<uint8_t>(0x000000FF & ControllerType);
    ParameterList[Offset + 12] = static_cast<uint8_t>((0xFF000000 & MinValue) >> 24);
    ParameterList[Offset + 11] = static_cast<uint8_t>((0x00FF0000 & MinValue) >> 16);
    ParameterList[Offset + 10] = static_cast<uint8_t>((0x0000FF00 & MinValue) >> 8);
    ParameterList[Offset + 9] = static_cast<uint8_t>(0x000000FF & MinValue);
    ParameterList[Offset + 16] = static_cast<uint8_t>((0xFF000000 & MaxValue) >> 24);
    ParameterList[Offset + 15] = static_cast<uint8_t>((0x00FF0000 & MaxValue) >> 16);
    ParameterList[Offset + 14] = static_cast<uint8_t>((0x0000FF00 & MaxValue) >> 8);
    ParameterList[Offset + 13] = static_cast<uint8_t>(0x000000FF & MaxValue);
    ParameterList[Offset + 20] = static_cast<uint8_t>((0xFF000000 & Channel) >> 24);
    ParameterList[Offset + 19] = static_cast<uint8_t>((0x00FF0000 & Channel) >> 16);
    ParameterList[Offset + 18] = static_cast<uint8_t>((0x0000FF00 & Channel) >> 8);
    ParameterList[Offset + 17] = static_cast<uint8_t>(0x000000FF & Channel);
    Offset += 21;
    size_t DataSize = LongName.size();
    while (DataSize >= 0x80) {
        ParameterList[Offset] = static_cast<uint8_t>((DataSize & 0x7F) | 0x80);
        DataSize >>= 7;
        Offset++;
    }
    for (unsigned char Byte : LongName) {
        ParameterList[Offset] = Byte;
        Offset++;
    }
    size_t DataSize = ShortName.size();
    while (DataSize >= 0x80) {
        ParameterList[Offset] = static_cast<uint8_t>((DataSize & 0x7F) | 0x80);
        DataSize >>= 7;
        Offset++;
    }
    for (unsigned char Byte : ShortName) {
        ParameterList[Offset] = Byte;
        Offset++;
    }
}

void CreateKnobAuto(uint8_t* ParameterList, size_t Page, size_t KnobID, uint32_t ControllerID, uint32_t ControllerType, uint32_t MinValue, uint32_t MaxValue, uint32_t Channel, string LongName, string ShortName, size_t& Offset) {
    CreateKnob(ParameterList, static_cast<uint8_t>(Page * 9 + KnobID), ControllerID, ControllerType, MinValue, MaxValue, Channel, LongName, ShortName, Offset);
}

void DefinePage(uint8_t* ParameterList, string PageName, size_t& Offset) {
    size_t DataSize = PageName.size();
    while (DataSize >= 0x80) {
        ParameterList[Offset] = static_cast<uint8_t>((DataSize & 0x7F) | 0x80);
        DataSize >>= 7;
        Offset++;
    }
    for (unsigned char Byte : PageName) {
        ParameterList[Offset] = Byte;
        Offset++;
    }
}

void CreateFLP(wstring Path, uint16_t PPQ) {
    ofstream Output_FLP(Path, ios::binary);
    vector<uint8_t> FLhd_Data;
    FLhd_Data.push_back(0x00);
    FLhd_Data.push_back(0x00);
    for (uint8_t NUM : ToBytes(0, 2))
    {
        FLhd_Data.push_back(NUM);
    }
    for (uint8_t NUM : ToBytes(PPQ, 2))
    {
        FLhd_Data.push_back(NUM);
    }
    vector<uint8_t> FLdt_Data;
    Make_FL_Event(FLdt_Data, 199, vector<uint8_t>{'1', '2', '.', '3', '.', '0', '.', '7', '2', 0});
    Make_FL_Event(FLdt_Data, 9, vector<uint8_t>{0x01}); //Loop Active
    Make_FL_Event(FLdt_Data, 11, vector<uint8_t>{0x00}); //No Shuffle
    Make_FL_Event(FLdt_Data, 12, vector<uint8_t>{0x00}); //Volume
    Make_FL_Event(FLdt_Data, 80, vector<uint8_t>{0x00}); //Pitch
    Make_FL_Event(FLdt_Data, 17, vector<uint8_t>{0x04}); //Time Division Numerator: 4
    Make_FL_Event(FLdt_Data, 18, vector<uint8_t>{0x04}); //Time Division Denominator: 4
    Make_FL_Event(FLdt_Data, 156, vector<uint8_t>{0x10, 0xF7, 0x07, 0x00}); //Tempo: 522.000
    Make_FL_Event(FLdt_Data, 67, vector<uint8_t>{0x01}); //Selected Pattern: 01
    Make_FL_Event(FLdt_Data, 28, vector<uint8_t>{0x00}); //Is Registered

    uint8_t ID_Plugin_New[24];
    for (uint8_t i = 0; i < 24; i++) ID_Plugin_New[i] = 0;
    ID_Plugin_New[8] = 2;
    ID_Plugin_New[16] = 16;
    uint8_t ID_Plugin_Parameters[4096];
    for (uint16_t i = 0; i < 4096; i++) ID_Plugin_Parameters[i] = 0;
    ID_Plugin_Parameters[0] = 4;
    for (uint8_t i = 8; i < 20; ++i) ID_Plugin_Parameters[i] = 255;
    ID_Plugin_Parameters[28] = 1; //Send MIDI pan/vol/pitch
    ID_Plugin_Parameters[29] = 0; //Map note color to midi channel
    ID_Plugin_Parameters[30] = 1; //Instrument
    
    // Set current value
    // Page 1
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 0);
    SetKnobAuto(ID_Plugin_Parameters, 127, 0, 1);
    SetKnobAuto(ID_Plugin_Parameters, 64, 0, 2);
    SetKnobAuto(ID_Plugin_Parameters, 127, 0, 3);
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 0, 8);
    // Page 2
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 0);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 2);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 3);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 1, 8);
    // Page 3
    SetKnobAuto(ID_Plugin_Parameters, 63, 2, 0);
    SetKnobAuto(ID_Plugin_Parameters, 63, 2, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 2, 2);
    SetKnobAuto(ID_Plugin_Parameters, 63, 2, 3);
    SetKnobAuto(ID_Plugin_Parameters, 63, 2, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 2, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 2, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 2, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 2, 8);
    // Page 4
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 0);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 2);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 3);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 3, 8);
    // Page 5
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 0);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 2);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 3);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 4, 8);
    // Page 6
    SetKnobAuto(ID_Plugin_Parameters, 12, 5, 0);
    SetKnobAuto(ID_Plugin_Parameters, 2, 5, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 5, 2);
    SetKnobAuto(ID_Plugin_Parameters, 8192, 5, 3);
    SetKnobAuto(ID_Plugin_Parameters, 8192, 5, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 5, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 5, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 5, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 5, 8);
    // Page 7
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 0);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 1);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 2);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 3);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 6, 5);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 6);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 7);
    SetKnobAuto(ID_Plugin_Parameters, 64, 6, 8);
    // Page 8
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 0);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 1);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 2);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 3);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 4);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 5);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 6);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 7);
    SetKnobAuto(ID_Plugin_Parameters, 0, 7, 8);
    
    // Define knob
    size_t Cursor = 7 * 9 * 4 + 9 * 4 + 34;
    // Page 1
    CreateKnobAuto(ID_Plugin_Parameters, 0, 0, 1, 0, 0, 127, -1, "Modulation", "Mod", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 0, 1, 7, 0, 0, 127, -1, "Volume", "Vol", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 0, 2, 10, 0, 0, 127, -1, "Panpot", "Pan", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 0, 3, 11, 0, 0, 127, -1, "Expression", "Exp", Cursor);
    // Page 2
    CreateKnobAuto(ID_Plugin_Parameters, 1, 0, 0, 0, 0, 127, -1, "Bank Select - Most Significant Byte", "MSB", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 1, 1, 32, 0, 0, 127, -1, "Bank Select - Least Significant Byte", "LSB", Cursor);
    // Page 3
    CreateKnobAuto(ID_Plugin_Parameters, 2, 0, 64, 0, 63, 64, -1, "Hold Peadal", "Hold", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 2, 1, 67, 0, 63, 64, -1, "Soft Pedal", "Soft", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 2, 4, 66, 0, 63, 64, -1, "Sostenuto", "Soste", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 2, 5, 65, 0, 63, 64, -1, "Sostenuto", "Porta", Cursor);
    // Page 4
    CreateKnobAuto(ID_Plugin_Parameters, 3, 0, 91, 0, 0, 127, -1, "Reverb", "Reverb", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 3, 1, 93, 0, 0, 127, -1, "Chorus", "Chorus", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 3, 2, 94, 0, 0, 127, -1, "Delay", "Delay", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 3, 3, -1, 3, 0, 255, -1, "Channel Aftertouch", "ChanAft", Cursor);
    // Page 5
    CreateKnobAuto(ID_Plugin_Parameters, 4, 0, 120, 0, 0, 1, -1, "All Sound Off", "SndOff", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 4, 1, 121, 0, 0, 1, -1, "Reset All Controller", "ResetCC", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 4, 2, 123, 0, 0, 1, -1, "All Note Off", "NoteOff", Cursor);
    // Page 6
    CreateKnobAuto(ID_Plugin_Parameters, 5, 0, 0, 1, 0, 127, -1, "Pitch Bend Sensitivity", "PitchRange", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 5, 1, 5, 1, 0, 127, -1, "Modulation Sensitivity", "ModRange", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 5, 3, 1, 1, 0, 16383, -1, "Fine Tune", "Fine", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 5, 4, 2, 1, 0, 16383, -1, "Coarse Tune", "Coarse", Cursor);
    // Page 7
    CreateKnobAuto(ID_Plugin_Parameters, 6, 0, 76, 0, 0, 127, -1, "Vibrato Rate", "VbrRate", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 1, 77, 0, 0, 127, -1, "Vibrato Depth", "VbrDepth", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 2, 78, 0, 0, 127, -1, "Vibrato Delay", "VbrDelay", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 3, 74, 0, 0, 127, -1, "Brightness (LPF Cutoff)", "Cutoff", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 4, 71, 0, 0, 127, -1, "Harmonic Content (Resonance)", "Resonance", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 6, 73, 0, 0, 127, -1, "Attack Time", "Attack", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 7, 75, 0, 0, 127, -1, "Decay Time", "Decay", Cursor);
    CreateKnobAuto(ID_Plugin_Parameters, 6, 8, 72, 0, 0, 127, -1, "Release Time", "Release", Cursor);
    
    // Page names
    ID_Plugin_Parameters[Cursor] = 0xFF;
    DefinePage(ID_Plugin_Parameters, "Basic", Cursor);
    DefinePage(ID_Plugin_Parameters, "Program", Cursor);
    DefinePage(ID_Plugin_Parameters, "Pedal", Cursor);
    DefinePage(ID_Plugin_Parameters, "Effect", Cursor);
    DefinePage(ID_Plugin_Parameters, "Reset", Cursor);
    DefinePage(ID_Plugin_Parameters, "RPN", Cursor);
    DefinePage(ID_Plugin_Parameters, "NRPN", Cursor);
    DefinePage(ID_Plugin_Parameters, "Reserved", Cursor);

    MessageBoxA(0, to_string(Cursor).c_str(), "Cursor:", 0);
    
    uint8_t ID_Channel_Parameters[125];
    for (uint8_t i = 0; i < 125; i++) ID_Channel_Parameters[i] = 0;
    ID_Channel_Parameters[64] = 0xFF;
    ID_Channel_Parameters[65] = 0xFF;
    ID_Channel_Parameters[68] = 0x00; //INT32_MIN
    ID_Channel_Parameters[69] = 0x00;
    ID_Channel_Parameters[70] = 0x00;
    ID_Channel_Parameters[71] = 0x80;
    ID_Channel_Parameters[72] = 0xFF; //INT32_MAX
    ID_Channel_Parameters[73] = 0xFF;
    ID_Channel_Parameters[74] = 0xFF;
    ID_Channel_Parameters[75] = 0x7F;
    ID_Channel_Parameters[112] = 0x0C; //Pitch Range: 12
    ID_Channel_Parameters[116] = 0xFE;
    ID_Channel_Parameters[117] = 0xFF;
    ID_Channel_Parameters[118] = 0xFF;
    ID_Channel_Parameters[119] = 0xFF;
    ID_Channel_Parameters[120] = 0xFF;
    ID_Channel_Parameters[121] = 0xFF;
    ID_Channel_Parameters[122] = 0xFF;
    ID_Channel_Parameters[123] = 0xFF;
    ID_Channel_Parameters[125] = 0x84;

    for (uint16_t Channel = 0; Channel < 16; Channel++) {
        ID_Plugin_Parameters[4] = Channel >= 9 ? Channel + 1 : Channel; //MIDI output channel
        Make_FL_Event(FLdt_Data, 64, ToBytes(static_cast<uint16_t>(Channel), 2));
        Make_FL_Event(FLdt_Data, 21, vector<uint8_t>{0x02});
        Make_FL_Event(FLdt_Data, 201, vector<uint8_t>{'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0, 0, 0});
        Make_FL_Event(FLdt_Data, 212, vector<uint8_t>(ID_Plugin_New, ID_Plugin_New + 24));
        wstring TrackName = L"MIDI Out -> Ch";
        TrackName += to_wstring(Channel >= 9 ? Channel + 2 : Channel + 1);
        TrackName += (wchar_t)0x0000;
        Make_FL_Event(FLdt_Data, 192, vector<uint8_t>(reinterpret_cast<const uint8_t*>(TrackName.data()), reinterpret_cast<const uint8_t*>(TrackName.data() + TrackName.size())));
        Make_FL_Event(FLdt_Data, 213, vector<uint8_t>(ID_Plugin_Parameters, ID_Plugin_Parameters + 965));
        Make_FL_Event(FLdt_Data, 215, vector<uint8_t>(ID_Channel_Parameters, ID_Channel_Parameters + 125));
    }

    for (uint16_t i = 0; i < 330; i++) ID_Plugin_Parameters[i] = 0;
    ID_Plugin_Parameters[0] = 4;
    for (uint8_t i = 8; i < 20; ++i) ID_Plugin_Parameters[i] = 255;
    ID_Plugin_Parameters[4] = 0; //MIDI output channel
    ID_Plugin_Parameters[28] = 0; //Send MIDI pan/vol/pitch
    ID_Plugin_Parameters[29] = 1; //Map note color to midi channel
    ID_Plugin_Parameters[30] = 0; //Instrument
    ID_Plugin_Parameters[322] = 0xFF; //IDK What's this but it prevent's FL Studio from crashing LOL.

    for (uint16_t Channel = 15; Channel < 25; Channel++) {
        Make_FL_Event(FLdt_Data, 64, ToBytes(static_cast<uint16_t>(Channel), 2));
        Make_FL_Event(FLdt_Data, 21, vector<uint8_t>{0x02});
        Make_FL_Event(FLdt_Data, 201, vector<uint8_t>{'M', 0, 'I', 0, 'D', 0, 'I', 0, ' ', 0, 'O', 0, 'u', 0, 't', 0, 0, 0});
        Make_FL_Event(FLdt_Data, 212, vector<uint8_t>(ID_Plugin_New, ID_Plugin_New + 24));
        wstring TrackName = L"MIDI Out -> Any";
        TrackName += (wchar_t)0x0000;
        Make_FL_Event(FLdt_Data, 192, vector<uint8_t>(reinterpret_cast<const uint8_t*>(TrackName.data()), reinterpret_cast<const uint8_t*>(TrackName.data() + TrackName.size())));
        Make_FL_Event(FLdt_Data, 213, vector<uint8_t>(ID_Plugin_Parameters, ID_Plugin_Parameters + 330));
        Make_FL_Event(FLdt_Data, 215, vector<uint8_t>(ID_Channel_Parameters, ID_Channel_Parameters + 125));
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