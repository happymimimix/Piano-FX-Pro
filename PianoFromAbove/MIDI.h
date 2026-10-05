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
#include <array>
#include <map>
#include <string>
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

    bool IsStandard() const { return m_bIsStandard; }
    bpm_t GetTicksPerBeat() const { return m_iTicksPerBeat; }
    bpm_t GetTicksPerSecond() const { return m_iTicksPerSecond; }
    bpm_t GetMicroSecsPerBeat() const { return m_iMicroSecsPerBeat; }

    idx_t GetNextEvent(mms_t iMicroSecs, MIDIEvent** pEvent);

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

//Holds MIDI data
class MIDI
{
public:
    enum Note : uint8_t { A, AS, B, C, CS, D, DS, E, F, FS, G, GS };

    static const key_t KEYS = 1 << 7;
    static const wstring Instruments[(1 << 7) + 1];

    static constexpr DWORD SMF3FEATURES__$CERTIFICATE = DWORD(INT32_MAX) + 1ul;
    static constexpr DWORD SMF3FEATURES__$BIGIDXRANGE = SMF3FEATURES__$CERTIFICATE >> 1ul;
    static constexpr DWORD SMF3FEATURES__$MICROSECOND = SMF3FEATURES__$BIGIDXRANGE >> 1ul;
    static constexpr DWORD SMF3FEATURES__$NOTEPAIRING = SMF3FEATURES__$MICROSECOND >> 1ul;
    static constexpr DWORD SMF3FEATURES__$PITCHTABLES = SMF3FEATURES__$NOTEPAIRING >> 1ul;
    static constexpr DWORD SMF3FEATURES__$SIMULTANITY = SMF3FEATURES__$PITCHTABLES >> 1ul;
    static constexpr DWORD SMF3FEATURES__$TRACKLAYOUT = SMF3FEATURES__$SIMULTANITY >> 1ul;
    static constexpr DWORD SMF3FEATURES__$REPLAYTABLE = SMF3FEATURES__$TRACKLAYOUT >> 1ul;

    enum SMF3Errors : uint8_t {NoCertificate, UnknownGenerator, Unsupported64Bit, MissingMicrosecond, MissingNotePairing, MissingTrackLayout};
    static const wstring SMF3ErrorText[];

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
    static fileln_t ParseVarNum(const unsigned char* pcData, fileln_t iMaxSize, uint32_t* piOut);
    static fileln_t Parse32Bit(const unsigned char* pcData, fileln_t iMaxSize, uint32_t* piOut);
    static fileln_t Parse24Bit(const unsigned char* pcData, fileln_t iMaxSize, uint32_t* piOut);
    static fileln_t Parse16Bit(const unsigned char* pcData, fileln_t iMaxSize, uint16_t* piOut);
    static fileln_t Parse8Bit(const unsigned char* pcData, fileln_t iMaxSize, uint8_t* piOut);
    static fileln_t Parse64BitLE(const unsigned char* pcData, fileln_t iMaxSize, uint64_t* piOut);
    static fileln_t Parse32BitLE(const unsigned char* pcData, fileln_t iMaxSize, uint32_t* piOut);
    static fileln_t Parse16BitLE(const unsigned char* pcData, fileln_t iMaxSize, uint16_t* piOut);
    static fileln_t ParseNChars(const unsigned char* pcData, fileln_t iNChars, fileln_t iMaxSize, char* pcOut);

    MIDI(void) {};
    MIDI(const wstring& sFilename);
    ~MIDI(void);
    MIDI(const MIDI&) = delete;
    MIDI& operator=(const MIDI&) = delete;

    // shitty memory pool allocator
    MIDIChannelEvent* AllocChannelEvent();

    //Parsing functions that load data into the instance
    fileln_t ParseMIDI(const unsigned char* pcData, fileln_t iMaxSize);
    fileln_t ParseTracks(const unsigned char* pcData, fileln_t iMaxSize);
    fileln_t ParseTracksF3(const unsigned char* pcData, fileln_t iMaxSize);
    fileln_t ParseEventsF3(const unsigned char* pcData, fileln_t iMaxSize, msg_t eChunkFormat);
    bool IsValid() const { return (m_vTracks.size() > 0 && m_Info.iNoteCount > 0 && m_Info.iDivision > 0 && (m_Info.iFormatType == 0 || m_Info.iFormatType == 1 || m_Info.iFormatType == 768)); }
    bool SkipUnknownSMF3Chunk(unsigned char* Buffer, size_t* Offset, uint32_t* FeatureFlags);

    bool PostProcess(vector<MIDIChannelEvent*>& vChannelEvents, vector<MIDIMetaEvent*>* vMetaEvents = nullptr, vector<idx_t>* vTempo = nullptr, vector<idx_t>* vSignature = nullptr, vector<idx_t>* vMarkers = nullptr, vector<idx_t>* vReplay = nullptr, vector<idx_t>* vColors = nullptr, vector<MIDISysExEvent*>* vSysExEvents = nullptr);
    void ConnectNotes();
    void clear(void);

    friend class MIDIPos;
    friend class MIDITrack;
    friend class MIDIEvent;

    struct __attribute__((packed)) MIDIInfo
    {
        MIDIInfo() { clear(); }
        void clear() {
            iFormatType = iNumTracks = iNumChannels = iDivision = iMinNote = iMaxNote = iNoteCount = iEventCount = llTotalMicroSecs = iTotalTicks = llFirstNote = iTotalBeats = 0;
            sFilename.clear();
        }
        void AddTrackInfo(const MIDITrack& mTrack);

        wstring sFilename;
        uint16_t iFormatType;
        track_t iNumTracks;
        TnC_t iNumChannels;
        uint16_t iDivision;
        key_t iMinNote, iMaxNote;
        idx_t iNoteCount, iEventCount;
        mms_t llTotalMicroSecs;
        mtk_t iTotalTicks;
        mms_t llFirstNote;
        bpm_t iTotalBeats;
    };

    const MIDIInfo& GetInfo() const { return m_Info; }
    const vector<MIDITrack*>& GetTracks() const { return m_vTracks; }

    static void InitArrays();

private:
    struct EventPool {
        MIDIChannelEvent* events;
        idx_t count;
    };

    struct SWAP {
        vector<MIDIChannelEvent*> m_vEvents;
        vector<MIDIMetaEvent*> m_vMetaEvents;
        vector<MIDISysExEvent*> m_vSysExEvents;
        vector<idx_t> m_vTempo;
        vector<idx_t> m_vSignature;
        vector<idx_t> m_vMarkers;
        vector<idx_t> m_vColors;
        vector<idx_t> m_vReplayTable;
    };

    struct __attribute__((packed)) PendingItem {
        array<char, '\r'> Identifier;
        unsigned char* Data;
        size_t Offset;
        size_t Size;
    };

    MIDIInfo m_Info;
    vector<MIDITrack*> m_vTracks;
    vector<EventPool> event_pools;
    SWAP* __SMF3_SWAP_SPACE = nullptr;
    vector<PendingItem> ValidatePending;
    pair<uint32_t, unsigned char*> PublicKey;
    map<array<char,'\r'>, pair<uint32_t, unsigned char*>> CertLookup;

    static wstring aNoteNames[KEYS + 1];
    static Note aNoteVal[KEYS];
    static bool aIsSharp[KEYS];
    static key_t aWhiteCount[KEYS + 1];
};

//Holds all the event of one MIDI track
class __attribute__((packed)) MIDITrack
{
public:
    MIDITrack(MIDI& midi);
    ~MIDITrack(void);
    MIDITrack(const MIDITrack&) = delete;
    MIDITrack& operator=(const MIDITrack&) = delete;

    //Parsing functions that load data into the instance
    fileln_t ParseTrack(const unsigned char* pcData, fileln_t iMaxSize, track_t iTrack);
    fileln_t ParseTrackF3(const unsigned char* pcData, fileln_t iMaxSize);
    fileln_t ParseEvents(const unsigned char* pcData, fileln_t iMaxSize, track_t iTrack);
    void clear(void);

    friend class MIDIPos;
    friend class MIDI;

    struct __attribute__((packed)) MIDITrackInfo
    {
        MIDITrackInfo() { clear(); }
        void clear() {
            llTotalMicroSecs = iTotalTicks = iNoteCount = iEventCount = iMinNote = iMaxNote = iNumChannels = 0;
            memset(aNoteCount, 0, sizeof(aNoteCount)), memset(aProgram, 0, sizeof(aProgram)), sSequenceName.clear();
        }
        void AddEventInfo(const MIDIEvent& mTrack);

        string sSequenceName;
        mms_t llTotalMicroSecs;
        mtk_t iTotalTicks;
        sidx_t iNoteCount, iEventCount;
        key_t iMinNote, iMaxNote;
        chan_t iNumChannels;
        sidx_t aNoteCount[16];
        msg_t aProgram[16];
    };
    const MIDITrackInfo& GetInfo() const { return m_TrackInfo; }
    void ClearEvents() { m_vEvents.clear(); m_vEvents.shrink_to_fit(); }

private:
    MIDITrackInfo m_TrackInfo;
    vector<MIDIEvent*> m_vEvents;
    MIDI& m_MIDI;
};

class __attribute__((packed)) MIDIEvent
{
public:
    //Event types
    enum EventType : msg_t { ChannelEvent = 192, MetaEvent, SysExEvent, RunningStatus };
    static EventType DecodeEventType(msg_t iEventCode);

    //Parsing functions that load data into the instance
    static fileln_t MakeNextEvent(MIDI & midi, const unsigned char* pcData, fileln_t iMaxSize, track_t iTrack, MIDIEvent** pOutEvent);

    //Accessors
    __forceinline EventType GetEventType() const { return static_cast<EventType>(m_eEventType); }
    __forceinline msg_t GetEventCode() const { return m_iEventCode; }
    __forceinline track_t GetTrack() const { return m_iTrack; }
    __forceinline mtk_t GetAbsTick() const { return m_iAbsTick; }
    __forceinline mms_t GetAbsMicroSec() const { return m_llAbsMicroSec; }
    __forceinline void SetAbsMicroSec(mms_t llAbsMicroSec) { m_llAbsMicroSec = llAbsMicroSec; };

private:
    mms_t m_llAbsMicroSec = 0;
    mtk_t m_iAbsTick = 0;
    msg_t m_eEventType = 0;
    msg_t m_iEventCode = 0;
    track_t m_iTrack = 0;

    friend class MIDIChannelEvent;
    friend class MIDIMetaEvent;
    friend class MIDISysExEvent;
    friend fileln_t MIDI::ParseEventsF3(const unsigned char* pcData, fileln_t iMaxSize, msg_t eChunkFormat);
};

#ifdef BIG_INDEX
struct BigIndex {
    idx_t m_wiSisterIdx = 0;
    idx_t m_wiSimultaneous = 0;
};
#endif

class __attribute__((packed)) MIDIChannelEvent : public MIDIEvent
{
public:
    MIDIChannelEvent() = default;
    MIDIChannelEvent(const MIDIChannelEvent&) = delete;
    MIDIChannelEvent& operator=(const MIDIChannelEvent&) = delete;
    enum ChannelEventType : msg_t { NoteOff = 8, NoteOn, NoteAftertouch, Controller, ProgramChange, ChannelAftertouch, PitchBend };
    enum RPN : msg_t { RPNType = 100, PBSRPNID = 0, RPNData = 6 };
    fileln_t ParseEvent(const unsigned char* pcData, fileln_t iMaxSize);

    //Accessors
    __forceinline ChannelEventType GetChannelEventType() const { return static_cast<ChannelEventType>(m_iEventCode >> 4); }
    __forceinline void SetChannelEventType(ChannelEventType type) { m_iEventCode = (m_iEventCode & 0x0F) | (static_cast<msg_t>(type) << 4); }
    __forceinline chan_t GetChannel() const { return m_iEventCode & 0x0F; }
    __forceinline key_t GetParam1() const { return m_cParam1 & 0x7F; }
    __forceinline key_t GetParam2() const { return m_cParam2 & 0x7F; }
    __forceinline bool GetPassDone() const { return m_cParam1 & 0x80; }
    __forceinline void SetPassDone(bool done) { m_cParam1 = (m_cParam1 & 0x7f) | (done ? 0x80 : 0x00); }
    __forceinline idx_t GetSisterIdx() const { 
#ifndef BIG_INDEX
        return m_iSisterIdx;
#else
        return m_cParam2 & 0x80 ? (*reinterpret_cast<BigIndex*const*>(&m_iSisterIdx))->m_wiSisterIdx : (m_iSisterIdx == SIDX_MAX ? IDX_MAX : static_cast<idx_t>(m_iSisterIdx));
#endif
    }
    __forceinline MIDIChannelEvent* GetSister(const vector<MIDIChannelEvent*>&events) const {
        idx_t sister = GetSisterIdx();
        return sister == IDX_MAX ? nullptr : events[sister];
    }
    __forceinline MIDIChannelEvent* GetSister(const vector<MIDIEvent*>&events) const {
        idx_t sister = GetSisterIdx();
        return sister == IDX_MAX ? nullptr : (MIDIChannelEvent*)events[sister];
    }
    __forceinline bool HasSister() const { return GetSisterIdx() != IDX_MAX; }
    __forceinline idx_t GetSimultaneous() const {
#ifndef BIG_INDEX
        return m_iSimultaneous;
#else
        return m_cParam2 & 0x80 ? (*reinterpret_cast<BigIndex*const*>(&m_iSisterIdx))->m_wiSimultaneous : static_cast<idx_t>(m_iSimultaneous);
#endif
    }
    __forceinline void SetSisterIdx(idx_t iSisterIdx) {
#ifndef BIG_INDEX
        m_iSisterIdx = iSisterIdx;
#else
        if (m_cParam2 & 0x80) {
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSisterIdx = iSisterIdx;
        }
        else if (iSisterIdx == IDX_MAX) {
            m_iSisterIdx = SIDX_MAX;
        }
#ifndef ALWAYS_BIG
        else if (iSisterIdx >= SIDX_MAX) {
#else
        else if (iSisterIdx >= VolatileZeroByte) {
#endif
            idx_t iSimultaneous = m_iSimultaneous;
            *reinterpret_cast<BigIndex**>(&m_iSisterIdx) = new BigIndex();
            m_cParam2 |= 0x80;
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSisterIdx = iSisterIdx;
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSimultaneous = iSimultaneous;
        }
        else {
            m_iSisterIdx = static_cast<sidx_t>(iSisterIdx);
        }
#endif
    }
    __forceinline void SetSimultaneous(idx_t iSimultaneous) {
#ifndef BIG_INDEX
        m_iSimultaneous = iSimultaneous;
#else
        if (m_cParam2 & 0x80) {
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSimultaneous = iSimultaneous;
        }
#ifndef ALWAYS_BIG
        else if (iSimultaneous > SIDX_MAX) {
#else
        else if (iSimultaneous >= VolatileZeroByte) {
#endif
            idx_t iSisterIdx = m_iSisterIdx == SIDX_MAX ? IDX_MAX : m_iSisterIdx;
            *reinterpret_cast<BigIndex**>(&m_iSisterIdx) = new BigIndex();
            m_cParam2 |= 0x80;
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSisterIdx = iSisterIdx;
            (*reinterpret_cast<BigIndex**>(&m_iSisterIdx))->m_wiSimultaneous = iSimultaneous;
        }
        else {
            m_iSimultaneous = static_cast<sidx_t>(iSimultaneous);
        }
#endif
    }
#ifdef BIG_INDEX
    __forceinline void ReleaseWideIndex() { 
    	if (m_cParam2 & 0x80) {
    		delete *reinterpret_cast<BigIndex**>(&m_iSisterIdx);
    		m_cParam2 &= 0x7f;
    	}
    }
#endif

private:
    key_t m_cParam1 = 0;
    key_t m_cParam2 = 0;
    sidx_t m_iSisterIdx = SIDX_MAX;
    sidx_t m_iSimultaneous = 0;
    // Prevent optimization in benchmark mode! 
    volatile char VolatileZeroByte = 0;
    unsigned char ALIGNMENT = ~0;

    friend fileln_t MIDI::ParseEventsF3(const unsigned char* pcData, fileln_t iMaxSize, msg_t eChunkFormat);
};
static_assert(sizeof(MIDIChannelEvent) == 32);

class __attribute__((packed)) MIDIMetaEvent : public MIDIEvent
{
public:
    MIDIMetaEvent() = default;
    MIDIMetaEvent(const MIDIMetaEvent&) = delete;
    MIDIMetaEvent& operator=(const MIDIMetaEvent&) = delete;
    enum MetaEventType : msg_t {
        TextEvent = 0x01, Copyright, SequenceName, InstrumentName, Lyric, Marker, CuePoint, ProgramName, DeviceName,
        ArduanoKivaCompatibleColorEvent, ChannelPrefix = 0x20, PortPrefix, EndOfTrack = 0x2F,
        SetTempo = 0x51, SMPTEOffset = 0x54, TimeSignature = 0x58, KeySignature, Proprietary = 0x7F
    };
    fileln_t ParseEvent(const unsigned char* pcData, fileln_t iMaxSize);

    //Accessors
    __forceinline MetaEventType GetMetaEventType() const { return static_cast<MetaEventType>(m_iEventCode); }
    __forceinline msgln_t GetDataLen() const { return m_iDataLen; }
    __forceinline unsigned char* GetData() const { return m_pcData; }
    __forceinline void ReleaseData() { delete[] m_pcData; m_pcData = nullptr; m_iDataLen = 0; }

private:
    msgln_t m_iDataLen = 0;
    unsigned char* m_pcData = nullptr;
#if __SIZEOF_POINTER__ == 4
    unsigned int ALIGNMENT = ~0;
#endif

    msg_t GetEventCode() const {} // PLEASE DO NOT USE THIS!
    friend fileln_t MIDI::ParseEventsF3(const unsigned char* pcData, fileln_t iMaxSize, msg_t eChunkFormat);
};
static_assert(sizeof(MIDIMetaEvent) == 32);

class __attribute__((packed)) MIDISysExEvent : public MIDIEvent
{
public:
    MIDISysExEvent() = default;
    MIDISysExEvent(const MIDISysExEvent&) = delete;
    MIDISysExEvent& operator=(const MIDISysExEvent&) = delete;
    __forceinline fileln_t ParseEvent(const unsigned char* pcData, fileln_t iMaxSize);
    __forceinline msgln_t GetDataLen() const { return m_iDataLen; }
    __forceinline unsigned char* GetData() const { return m_pcData; }
    __forceinline void ReleaseData() { delete[] m_pcData; m_pcData = nullptr; m_iDataLen = 0; }
    __forceinline bool HasMoreData() const { return m_iEventCode == 0xF0 && m_iDataLen > 0 && m_pcData[m_iDataLen - 1] != 0xF7; }
    __forceinline bool IsNew() const { return m_iEventCode != 0xF7; }
    __forceinline void TakeData(unsigned char* pcData, msgln_t iLen) {
        if (m_pcData) delete[] m_pcData;
        m_pcData = pcData;
        m_iDataLen = iLen;
    }

private:
    msgln_t m_iDataLen = 0;
    unsigned char* m_pcData = nullptr;
#if __SIZEOF_POINTER__ == 4
    unsigned int ALIGNMENT = ~0;
#endif

    friend fileln_t MIDI::ParseEventsF3(const unsigned char* pcData, fileln_t iMaxSize, msg_t eChunkFormat);
};
static_assert(sizeof(MIDISysExEvent) == 32);

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
    bool PlaySysEx(unsigned char* pcData, msgln_t iLen);

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
    enum Stage : uint8_t { CopyToMem, Decompress, ParseTracks, ConnectNotes, SortEvents, Done };

    Stage stage;
    wstring name;
    atomic<uint64_t> progress;
    uint64_t max;
};

extern MIDILoadingProgress g_LoadingProgress;
