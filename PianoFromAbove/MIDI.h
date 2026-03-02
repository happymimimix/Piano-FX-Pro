/*************************************************************************************************
*
* File: MIDI.h
*
* Description: Defines the MIDI objects
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <atomic>
#include <stdint.h>
using namespace std;

#include "Misc.h"

//Classes defined in this file
class MIDI;
class MIDITrack;
class MIDIEvent;
class MIDIChannelEvent;
class MIDIMetaEvent;
class MIDISysExEvent;
class MIDIPos;
class MIDIOutDevice;

//
// MIDI File Classes
//

class MIDIPos
{
public:
    MIDIPos(MIDI& midi);
    ~MIDIPos();

    idx_t GetNextEvent(mms_t iMicroSecs, MIDIEvent** pEvent);
    idx_t GetNextEvents(mms_t iMicroSecs, vector<MIDIEvent*>& vEvents);

    bool IsStandard() const { return m_bIsStandard; }
    bpm_t GetTicksPerBeat() const { return m_iTicksPerBeat; }
    bpm_t GetTicksPerSecond() const { return m_iTicksPerSecond; }
    bpm_t GetMicroSecsPerBeat() const { return m_iMicroSecsPerBeat; }

    int* m_pTrackTime;

private:
    // Where are we in the file?
    MIDI& m_MIDI;
    vector< size_t > m_vTrackPos;

    // Tempo variables
    bool m_bIsStandard;
    bpm_t m_iTicksPerBeat, m_iMicroSecsPerBeat, m_iTicksPerSecond; // For SMPTE division

    // Position variables
    tick_t m_iCurrTick;
    mms_t m_iCurrMicroSec;
};

typedef vector<pair<mms_t, idx_t>> eventvec_t;

//Holds MIDI data
class MIDI
{
public:
    enum Note { A, AS, B, C, CS, D, DS, E, F, FS, G, GS };

    static const key_t KEYS = 129; // One extra because 128th is a sharp
    static const wstring Instruments[129];
    static const wstring& NoteName(key_t iNote);
    static Note NoteVal(key_t iNote);
    static bool IsSharp(key_t iNote);
    static key_t WhiteCount(key_t iMinNote, key_t iMaxNote);

    //Generally usefull static parsing functions
    static uint32_t ParseVarNum(const unsigned char* pcData, msgln_t iMaxSize, uint32_t* piOut);
    static uint32_t Parse32Bit(const unsigned char* pcData, msgln_t iMaxSize, uint32_t* piOut);
    static uint32_t Parse24Bit(const unsigned char* pcData, msgln_t iMaxSize, uint32_t* piOut);
    static uint16_t Parse16Bit(const unsigned char* pcData, msgln_t iMaxSize, uint16_t* piOut);
    static uint32_t ParseNChars(const unsigned char* pcData, msgln_t iNChars, msgln_t iMaxSize, char* pcOut);

    MIDI(void) {};
    MIDI(const wstring& sFilename);
    ~MIDI(void);

    // shitty memory pool allocator
    MIDIChannelEvent* AllocChannelEvent();

    //Parsing functions that load data into the instance
    idx_t ParseMIDI(const unsigned char* pcData, idx_t iMaxSize);
    idx_t ParseTracks(const unsigned char* pcData, idx_t iMaxSize);
    idx_t ParseEvents(const unsigned char* pcData, idx_t iMaxSize);
    bool IsValid() const { return (m_vTracks.size() > 0 && m_Info.iNoteCount > 0 && m_Info.iDivision > 0); }

    void PostProcess(vector<MIDIChannelEvent*>& vChannelEvents, vector<MIDIMetaEvent*>* vMetaEvents = nullptr, eventvec_t* vTempo = nullptr, eventvec_t* vSignature = nullptr, eventvec_t* vMarkers = nullptr, eventvec_t* vColors = nullptr);
    void ConnectNotes();
    void clear(void);

    friend class MIDIPos;
    friend class MIDITrack;
    friend class MIDIEvent;

    struct MIDIInfo
    {
        MIDIInfo() { clear(); }
        void clear() {
            llTotalMicroSecs = llFirstNote = iFormatType = iNumTracks = iNumChannels = iDivision = iMinNote =
                iMaxNote = iNoteCount = iEventCount = iTotalTicks = iTotalBeats = 0;
            sFilename.clear();
        }
        void AddTrackInfo(const MIDITrack& mTrack);

        wstring sFilename;
        uint16_t iFormatType;
        uint16_t iNumTracks, iNumChannels;
        uint16_t iDivision;
        unsigned char iMinNote, iMaxNote;
        size_t iNoteCount, iEventCount;
        long iTotalTicks, iTotalBeats;
        long long llTotalMicroSecs, llFirstNote;
    };

    const MIDIInfo& GetInfo() const { return m_Info; }
    const vector< MIDITrack* >& GetTracks() const { return m_vTracks; }

private:
    struct EventPool {
        MIDIChannelEvent* events;
        size_t count;
    };

    static void InitArrays();
    static wstring aNoteNames[KEYS + 1];
    static Note aNoteVal[KEYS];
    static bool aIsSharp[KEYS];
    static unsigned char aWhiteCount[KEYS + 1];

    MIDIInfo m_Info;
    vector< MIDITrack* > m_vTracks;

    vector<EventPool> event_pools;
};

//Holds all the event of one MIDI track
class MIDITrack
{
public:
    MIDITrack(MIDI& midi);
    ~MIDITrack(void);

    //Parsing functions that load data into the instance
    idx_t ParseTrack(const unsigned char* pcData, idx_t iMaxSize, track_t iTrack);
    idx_t ParseEvents(const unsigned char* pcData, idx_t iMaxSize, track_t iTrack);
    void clear(void);

    friend class MIDIPos;
    friend class MIDI;

    struct MIDITrackInfo
    {
        MIDITrackInfo() { clear(); }
        void clear() {
            llTotalMicroSecs = iSequenceNumber = iMinNote = iMaxNote = iNoteCount =
                iEventCount = iTotalTicks = iNumChannels = 0;
            memset(aNoteCount, 0, sizeof(aNoteCount)),
                memset(aProgram, 0, sizeof(aProgram)),
                sSequenceName.clear();
        }
        void AddEventInfo(const MIDIEvent& mTrack);

        uint16_t iSequenceNumber;
        string sSequenceName;
        unsigned char iMinNote, iMaxNote;
        size_t iNoteCount, iEventCount;
        long iTotalTicks;
        long long llTotalMicroSecs;
        size_t aNoteCount[16];
        unsigned char aProgram[16], iNumChannels;
    };
    const MIDITrackInfo& GetInfo() const { return m_TrackInfo; }
    void ClearEvents() { m_vEvents.clear(); m_vEvents.shrink_to_fit(); }

private:
    MIDITrackInfo m_TrackInfo;
    vector< MIDIEvent* > m_vEvents;
    MIDI& m_MIDI;
};

//Base Event class
//Should really be a single class with unions for the different events. much faster that way.
//Might be forced to convert if batch processing is too slow
class MIDIEvent
{
public:
    //Event types
    enum EventType { ChannelEvent, MetaEvent, SysExEvent, RunningStatus };
    static EventType DecodeEventType(int iEventCode);

    //Parsing functions that load data into the instance
    static uint32_t MakeNextEvent(MIDI& midi, const unsigned char* pcData, msgln_t iMaxSize, track_t iTrack, MIDIEvent** pOutEvent);

    //Accessors
    EventType GetEventType() const { return (EventType)m_eEventType; }
    msg_t GetEventCode() const { return m_iEventCode; }
    track_t GetTrack() const { return m_iTrack; }
    tick_t GetAbsT() const { return m_iAbsT; }
    mms_t GetAbsMicroSec() const { return m_llAbsMicroSec; }
    void SetAbsMicroSec(mms_t llAbsMicroSec) { m_llAbsMicroSec = llAbsMicroSec; };

    mms_t m_llAbsMicroSec;
    tick_t m_iAbsT;
    track_t m_iTrack;
    msg_t m_eEventType;
    msg_t m_iEventCode;
};

//Channel Event: notes and whatnot
class MIDIChannelEvent : public MIDIEvent
{
public:
    MIDIChannelEvent() : m_iSisterIdx(-1ll), m_iSimultaneous(0ll), m_bPassDone(false) { }

    enum ChannelEventType { NoteOff = 0x8, NoteOn, NoteAftertouch, Controller, ProgramChange, ChannelAftertouch, PitchBend };
    enum RPN { RPNType = 100, PBSRPNID = 0, RPNData = 6 };
    uint32_t ParseEvent(const unsigned char* pcData, msgln_t iMaxSize);

    //Accessors
    ChannelEventType GetChannelEventType() const { return static_cast<ChannelEventType>(m_iEventCode >> 4); }
    void SetChannelEventType(ChannelEventType type) { m_iEventCode = (m_iEventCode & 0x0F) | (static_cast<msg_t>(type) << 4); }
    msg_t GetChannel() const { return m_cChannel & 0x0F; }
    msg_t GetParam1() const { return m_cParam1 & 0x7F; }
    msg_t GetParam2() const { return m_cParam2 & 0x7F; }
    MIDIChannelEvent* GetSister(const vector<MIDIChannelEvent*>& events) const {
        return m_iSisterIdx == IDX_MAX ? nullptr : events[m_iSisterIdx];
    }
    MIDIChannelEvent* GetSister(const vector<MIDIEvent*>& events) const {
        return m_iSisterIdx == IDX_MAX ? nullptr : (MIDIChannelEvent*)events[m_iSisterIdx];
    }
    idx_t GetSisterIdx() const { return m_iSisterIdx; }
    idx_t GetSimultaneous() const { return m_iSimultaneous; }
    idx_t GetLength() const { return m_uLength; }
    bool GetPassDone() const { return m_bPassDone; }

    void SetChannel(unsigned char channel) { m_cChannel = channel; }
    void SetParam1(unsigned char param1) { m_cParam1 = param1; }
    void SetParam2(unsigned char param2) { m_cParam2 = param2; }
    void SetSisterIdx(size_t iSisterIdx) { m_iSisterIdx = iSisterIdx; }
    void SetSimultaneous(size_t iSimultaneous) { m_iSimultaneous = iSimultaneous; }
    void SetLength(size_t length) { m_uLength = length; }
    void SetPassDone(bool done) { m_bPassDone = done; }

    bool HasSister() const { return m_iSisterIdx != IDX_MAX; }

private:
    idx_t m_iSisterIdx;
    idx_t m_iSimultaneous;
    idx_t m_uLength;
    bool m_bPassDone;
    chan_t m_cChannel;
    key_t m_cParam1;
    key_t m_cParam2;
};

//Meta Event: info about the notes and whatnot
class MIDIMetaEvent : public MIDIEvent
{
public:
    MIDIMetaEvent() : m_pcData(0) { }
    ~MIDIMetaEvent() { if (m_pcData) delete[] m_pcData; }

    enum MetaEventType {
        SequenceNumber, TextEvent, Copyright, SequenceName, InstrumentName, Lyric, Marker,
        CuePoint, GenericTextA = 0x0a, ChannelPrefix = 0x20, PortPrefix = 0x21, EndOfTrack = 0x2F, SetTempo = 0x51,
        SMPTEOffset = 0x54, TimeSignature = 0x58, KeySignature = 0x59, Proprietary = 0x7F
    };
    uint32_t ParseEvent(const unsigned char* pcData, msgln_t iMaxSize);

    //Accessors
    MetaEventType GetMetaEventType() const { return m_eMetaEventType; }
    msgln_t GetDataLen() const { return m_iDataLen; }
    unsigned char* GetData() const { return m_pcData; }

private:
    MetaEventType m_eMetaEventType;
    msgln_t m_iDataLen;
    unsigned char* m_pcData;
};

//SysEx Event: probably to be ignored
class MIDISysExEvent : public MIDIEvent
{
public:
    MIDISysExEvent() : m_pcData(0) { }
    ~MIDISysExEvent() { if (m_pcData) delete[] m_pcData; }

    uint32_t ParseEvent(const unsigned char* pcData, msgln_t iMaxSize);

private:
    uint32_t m_iDataLen;
    unsigned char* m_pcData;
    bool m_bHasMoreData;
};

//
// MIDI Device Classes
//

class MIDIOutDevice
{
public:
    MIDIOutDevice() : m_bIsOpen(false), m_hMIDIOut(NULL) { }
    virtual ~MIDIOutDevice() { Close(); }

    win32_t GetNumDevs() const;
    wstring GetDevName(win32_t iDev) const;
    bool Open(win32_t iDev);
    bool OpenKDMAPI();
    void Close();
    void Reset();

    bool IsOpen() const { return m_bIsOpen; }
    const wstring& GetDevice() const { return m_sDevice; };

    void AllNotesOff();
    void AllNotesOff(const vector<chan_t>& vChannels);
    void SetVolume(double dVolume);

    bool PlayEventAcrossChannels(msg_t cStatus, msg_t cParam1, msg_t cParam2);
    bool PlayEventAcrossChannels(msg_t cStatus, msg_t cParam1, msg_t cParam2, const vector<chan_t>& vChannels);
    bool PlayEvent(msg_t bStatus, msg_t bParam1, msg_t bParam2 = 0);

private:
    static FARPROC GetOmniMIDIProc(const char* func);

    bool m_bIsOpen;
    bool m_bIsKDMAPI;
    void(WINAPI* SendDirectData)(DWORD);
    wstring m_sDevice;
    HMIDIOUT m_hMIDIOut;
};

class MIDILoadingProgress {
public:
    enum Stage { CopyToMem, Decompress, ParseTracks, ConnectNotes, SortEvents, NCTable, Done };

    Stage stage;
    wstring name;
    atomic<uint64_t> progress;
    uint64_t max;
};

extern MIDILoadingProgress g_LoadingProgress;