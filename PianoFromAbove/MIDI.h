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
#include <Misc.h>

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

    mtk_t* m_pTrackTime;

private:
    // Tournament tree
    track_t* m_pLoserTree;
    track_t m_iTreeSize;
    track_t m_iTrackCount;
    bool m_bTreeBuilt;

    __forceinline idx_t TreeSize() const {
        // Size 0 is impossible, so whenever we see 0 we know the actual size is 65536.
        // It overflowed to 0 because the size field is an unsigned 16 bit.
        return m_iTreeSize == 0 ? static_cast<idx_t>(UINT16_MAX) + static_cast<idx_t>(1) : static_cast<idx_t>(m_iTreeSize);
    }

    __forceinline mtk_t TreeKey(track_t trackId) const {
        return (trackId < m_iTrackCount) ? m_pTrackTime[trackId] : INT64_MAX;
    }

    __forceinline bool TrackBeats(track_t a, track_t b) const {
        const mtk_t ka = TreeKey(a), kb = TreeKey(b);
        if (ka != kb) return ka < kb;
        return a < b;
    }

    __forceinline track_t PlayMatch(track_t a, track_t b, track_t* loserOut) const {
        if (TrackBeats(a, b)) { *loserOut = b; return a; }
        else { *loserOut = a; return b; }
    }

    void BuildTree();
    void RestoreTree(track_t leafId);

    MIDI& m_MIDI;
    idx_t* m_pTrackPos;

    // Tempo variables
    bool m_bIsStandard;
    bpm_t m_iTicksPerBeat, m_iMicroSecsPerBeat, m_iTicksPerSecond; // For SMPTE division

    // Position variables
    mtk_t m_iCurrTick;
    mms_t m_iCurrMicroSec;
};

typedef vector<pair<mms_t, idx_t>> eventvec_t;

//Holds MIDI data
class MIDI
{
public:
    enum Note { A, AS, B, C, CS, D, DS, E, F, FS, G, GS };

    static const key_t KEYS = 1 << 7;
    static const wstring Instruments[(1 << 7) + 1];

    __forceinline static const wstring& NoteName(key_t iNote)
    {
        if (iNote & 0x80) return aNoteNames[MIDI::KEYS];
        return aNoteNames[iNote];
    }

    __forceinline static Note NoteVal(key_t iNote)
    {
        if (iNote & 0x80) return C;
        return aNoteVal[iNote];
    }

    __forceinline static bool IsSharp(key_t iNote)
    {
        if (iNote < 192) {
            return (1 << (iNote % 12)) & 0b010101001010;
        }
        else {
            skey_t siNote = (skey_t)iNote;
            return (1 << (siNote % 12)) & 0b010101001010;
        }
    }

    __forceinline static key_t WhiteCount(key_t iMinNote, key_t iMaxNote)
    {
        if (iMinNote >= MIDI::KEYS || iMaxNote > MIDI::KEYS) return false;
        return aWhiteCount[iMaxNote] - aWhiteCount[iMinNote];
    }

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

    bool PostProcess(vector<MIDIChannelEvent*>& vChannelEvents, vector<MIDIMetaEvent*>* vMetaEvents = nullptr, eventvec_t* vTempo = nullptr, eventvec_t* vSignature = nullptr, eventvec_t* vMarkers = nullptr, eventvec_t* vColors = nullptr, vector<MIDISysExEvent*>* vSysExEvents = nullptr);
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
        track_t iNumTracks;
        chan_t iNumChannels;
        uint16_t iDivision;
        key_t iMinNote, iMaxNote;
        idx_t iNoteCount, iEventCount;
        mtk_t iTotalTicks;
        bpm_t iTotalBeats;
        mms_t llTotalMicroSecs, llFirstNote;
    };

    const MIDIInfo& GetInfo() const { return m_Info; }
    const vector<MIDITrack*>& GetTracks() const { return m_vTracks; }

    static void InitArrays();

private:
    struct EventPool {
        MIDIChannelEvent* events;
        idx_t count;
    };

    static wstring aNoteNames[KEYS + 1];
    static Note aNoteVal[KEYS];
    static bool aIsSharp[KEYS];
    static key_t aWhiteCount[KEYS + 1];

    MIDIInfo m_Info;
    vector<MIDITrack*> m_vTracks;

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

        track_t iSequenceNumber;
        string sSequenceName;
        key_t iMinNote, iMaxNote;
        idx_t iNoteCount, iEventCount;
        mtk_t iTotalTicks;
        mms_t llTotalMicroSecs;
        idx_t aNoteCount[16];
        msg_t aProgram[16];
        chan_t iNumChannels;
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
class __attribute__((packed)) MIDIEvent
{
public:
    //Event types
    enum EventType { ChannelEvent, MetaEvent, SysExEvent, RunningStatus };
    static EventType DecodeEventType(msg_t iEventCode);

    //Parsing functions that load data into the instance
    static uint32_t MakeNextEvent(MIDI & midi, const unsigned char* pcData, msgln_t iMaxSize, track_t iTrack, MIDIEvent * *pOutEvent);

    //Accessors
    __forceinline EventType GetEventType() const { return (EventType)m_eEventType; }
    __forceinline msg_t GetEventCode() const { return m_iEventCode; }
    __forceinline track_t GetTrack() const { return m_iTrack; }
    __forceinline mtk_t GetAbsT() const { return m_iAbsT; }
    __forceinline mms_t GetAbsMicroSec() const { return m_llAbsMicroSec; }
    __forceinline void SetAbsMicroSec(mms_t llAbsMicroSec) { m_llAbsMicroSec = llAbsMicroSec; };

    track_t m_iTrack;
    mms_t m_llAbsMicroSec;
    mtk_t m_iAbsT;
    msg_t m_eEventType;
    msg_t m_iEventCode;
};

//Channel Event: notes and whatnot
class __attribute__((packed)) MIDIChannelEvent : public MIDIEvent
{
public:
    MIDIChannelEvent() : m_iSisterIdx(IDX_MAX), m_iSimultaneous(0), m_bPassDone(false) { }

    enum ChannelEventType { NoteOff = 0x8, NoteOn, NoteAftertouch, Controller, ProgramChange, ChannelAftertouch, PitchBend };
    enum RPN { RPNType = 100, PBSRPNID = 0, RPNData = 6 };
    uint32_t ParseEvent(const unsigned char* pcData, msgln_t iMaxSize);

    //Accessors
    __forceinline ChannelEventType GetChannelEventType() const { return static_cast<ChannelEventType>(m_iEventCode >> 4); }
    __forceinline void SetChannelEventType(ChannelEventType type) { m_iEventCode = (m_iEventCode & 0x0F) | (static_cast<msg_t>(type) << 4); }
    __forceinline chan_t GetChannel() const { return m_cChannel & 0x0F; }
    __forceinline key_t GetParam1() const { return m_cParam1 & 0x7F; }
    __forceinline key_t GetParam2() const { return m_cParam2 & 0x7F; }
    __forceinline MIDIChannelEvent* GetSister(const vector<MIDIChannelEvent*>&events) const {
        return m_iSisterIdx == IDX_MAX ? nullptr : events[m_iSisterIdx];
    }
    __forceinline MIDIChannelEvent* GetSister(const vector<MIDIEvent*>&events) const {
        return m_iSisterIdx == IDX_MAX ? nullptr : (MIDIChannelEvent*)events[m_iSisterIdx];
    }
    __forceinline idx_t GetSisterIdx() const { return m_iSisterIdx; }
    __forceinline idx_t GetSimultaneous() const { return m_iSimultaneous; }
    __forceinline bool GetPassDone() const { return m_bPassDone; }

    __forceinline void SetChannel(chan_t channel) { m_cChannel = channel; }
    __forceinline void SetParam1(key_t param1) { m_cParam1 = param1; }
    __forceinline void SetParam2(key_t param2) { m_cParam2 = param2; }
    __forceinline void SetSisterIdx(idx_t iSisterIdx) { m_iSisterIdx = iSisterIdx; }
    __forceinline void SetSimultaneous(idx_t iSimultaneous) { m_iSimultaneous = iSimultaneous; }
    __forceinline void SetPassDone(bool done) { m_bPassDone = done; }

    __forceinline bool HasSister() const { return m_iSisterIdx != IDX_MAX; }

private:
    chan_t m_cChannel;
    key_t m_cParam1;
    key_t m_cParam2;
    idx_t m_iSisterIdx;
    idx_t m_iSimultaneous;
    bool m_bPassDone;
};

//Meta Event: info about the notes and whatnot
class __attribute__((packed)) MIDIMetaEvent : public MIDIEvent
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
    __forceinline MetaEventType GetMetaEventType() const { return m_eMetaEventType; }
    __forceinline msgln_t GetDataLen() const { return m_iDataLen; }
    __forceinline unsigned char* GetData() const { return m_pcData; }

private:
    MetaEventType m_eMetaEventType;
    msgln_t m_iDataLen;
    unsigned char* m_pcData;
};

//SysEx Event: forwarded to MIDI output device
class __attribute__((packed)) MIDISysExEvent : public MIDIEvent
{
public:
    MIDISysExEvent() : m_pcData(0) { }
    ~MIDISysExEvent() { if (m_pcData) delete[] m_pcData; }

    __forceinline uint32_t ParseEvent(const unsigned char* pcData, msgln_t iMaxSize);
    __forceinline uint32_t GetDataLen() const { return m_iDataLen; }
    __forceinline unsigned char* GetData() const { return m_pcData; }
    __forceinline bool HasMoreData() const { return m_iEventCode == 0xF0 && m_iDataLen > 0 && m_pcData[m_iDataLen - 1] != 0xF7; }
    __forceinline bool IsNew() const { return m_iEventCode != 0xF7; }
    __forceinline void TakeData(unsigned char* pcData, uint32_t iLen) {
        if (m_pcData) delete[] m_pcData;
        m_pcData = pcData;
        m_iDataLen = iLen;
    }

private:
    uint32_t m_iDataLen;
    unsigned char* m_pcData;
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
    bool PlaySysEx(unsigned char* pcData, msg_t iLen);

private:
    static FARPROC GetOmniMIDIProc(const char* func);

    bool m_bIsOpen;
    bool m_bIsKDMAPI;
    void (WINAPI *SendDirectData)(DWORD);
    MMRESULT (WINAPI *PrepareLongData)(MIDIHDR*, UINT);
    MMRESULT (WINAPI *UnprepareLongData)(MIDIHDR*, UINT);
    MMRESULT (WINAPI *SendDirectLongData)(MIDIHDR*, UINT);
    wstring m_sDevice;
    HMIDIOUT m_hMIDIOut;
};

class MIDILoadingProgress {
public:
    enum Stage : uint8_t { CopyToMem, Decompress, ParseTracks, ConnectNotes, SortEvents, NCTable, Done };

    Stage stage;
    wstring name;
    atomic<uint64_t> progress;
    uint64_t max;
};

extern MIDILoadingProgress g_LoadingProgress;