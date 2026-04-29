/*************************************************************************************************
*
* File: GameState.h
*
* Description: Defines the game states and objects rendered into the graphics window
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>
#include <map>
#include <string>
#include <functional>
#include <deque>
#include <Renderer.h>
#include <MIDI.h>
#include <Misc.h>

inline mms_t m_llStartTime;
inline mms_t m_llRndStartTime;
inline mtk_t m_iStartTick;
inline idx_t* m_vNCTable = nullptr;
inline idx_t polyphony;
inline wstring polyFormatted;
inline idx_t nps;
inline wstring npsFormatted;
inline idx_t passed;
inline wstring passedFormatted;
inline idx_t TotalNC;
inline win32_t width = -1;
inline win32_t height = -1;
inline uint16_t resolution = -1;
inline char CheatEngineCaption[1 << 19] = { 'C' };
inline char* CaptionContent = CheatEngineCaption + 1;
inline mms_t TotalTime;
inline string TotalTimeFormatted;
inline string llStartTimeFormatted;
inline char Difficulty[1 << 10] = {};
inline bool UpdateNotePos = true;
inline static const mms_t MS = 1e+3;
inline static const mms_t S = 1e+6;
inline bool CE_Connected = false;
inline bool CE_DoNextTick = false;
inline bool CE_Responded = false;

inline HRESULT GetGDI(HWND hGDI, win32_t W, win32_t H, char* Output) {
    if (!hGDI) return E_FAIL;
    HDC GDIdc = GetDC(hGDI);
    BITMAPINFO BMPinfo = {};
    BMPinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BMPinfo.bmiHeader.biWidth = W;
    BMPinfo.bmiHeader.biHeight = -H;
    BMPinfo.bmiHeader.biPlanes = 1;
    BMPinfo.bmiHeader.biBitCount = 32;
    BMPinfo.bmiHeader.biCompression = BI_RGB;
    BMPinfo.bmiHeader.biSizeImage = 0;
    void* Bits = nullptr;
    HDC MEMdc = CreateCompatibleDC(GDIdc);
    HBITMAP BMP = CreateDIBSection(GDIdc, &BMPinfo, DIB_RGB_COLORS, &Bits, nullptr, 0);
    SelectObject(MEMdc, BMP);
    BitBlt(MEMdc, 0, 0, W, H, GDIdc, 0, 0, SRCCOPY);
    GdiFlush();
    memcpy(Output, Bits, W * H * 4);
    DeleteDC(MEMdc);
    DeleteObject(BMP);
    ReleaseDC(hGDI, GDIdc);
    return S_OK;
}

// This is a large dynamic array of 1 and 0. 
// It's purpose is to track what keys are pressed. 
struct dynamic_bitset {
    static constexpr unsigned char WORD_SIZE = sizeof(size_t) * CHAR_BIT;
    static constexpr unsigned char WORD_DIV_SHR = __builtin_ctz(WORD_SIZE);
    static constexpr size_t WORD_MASK = WORD_SIZE - 1;
    idx_t minWord = 0;
    idx_t maxWord = 0;
    // Disable copying
    dynamic_bitset(const dynamic_bitset&) = delete;
    dynamic_bitset& operator=(const dynamic_bitset&) = delete;

    dynamic_bitset(idx_t size) {
        // This is the ONLY place where we need to modify wordCount. It should never be changed elsewhere.
        wordCount = (size + WORD_MASK) >> WORD_DIV_SHR;
        bits = new size_t[wordCount]();
        this->Clear();
    }
    ~dynamic_bitset() { delete[] bits; }

    idx_t Size() {
        return wordCount;
    }

    void Clear() {
        if (maxWord >= minWord && maxWord < wordCount) {
            memset(&bits[minWord], 0, (maxWord - minWord + 1) * sizeof(size_t));
        }
        minWord = wordCount - 1;
        maxWord = 0;
    }

    __forceinline void Activate(idx_t idx) {
        idx_t word = idx >> WORD_DIV_SHR;
        bits[word] |= (size_t(1) << (idx & WORD_MASK));
        minWord = min(minWord, word);
        maxWord = max(maxWord, word);
    }

    __forceinline void Deactivate(idx_t idx) {
        bits[idx >> WORD_DIV_SHR] &= ~(size_t(1) << (idx & WORD_MASK));
    }

    __forceinline bool IsActive(idx_t idx) const {
        return bits[idx >> WORD_DIV_SHR] & (size_t(1) << (idx & WORD_MASK));
    }

    static __forceinline idx_t CTZ(size_t word) {
        if constexpr (sizeof(size_t) == sizeof(unsigned long long)) return __builtin_ctzll(word);
        else return __builtin_ctz(word);
    }

    static __forceinline idx_t CLZ(size_t word) {
        if constexpr (sizeof(size_t) == sizeof(unsigned long long)) return __builtin_clzll(word);
        else return __builtin_clz(word);
    }

    template<typename Func>
    __forceinline void ForEach(Func&& FuncPtr) {
		if (minWord > maxWord) return;
		minWord = min(minWord, wordCount - 1);
        maxWord = min(maxWord, wordCount - 1);
        idx_t newMin = wordCount - 1;
        idx_t newMax = 0;
        bool first = true;
        for (idx_t word_id = minWord; word_id <= maxWord; word_id++) {
            size_t word = bits[word_id];
            if (word) {
                if (first) {
                    newMin = word_id;
                    first = false;
                }
                newMax = word_id;
            }
            while (word) {
                idx_t bit = CTZ(word);
                FuncPtr((word_id << WORD_DIV_SHR) | bit);
                word &= word - 1;
            }
        }
        minWord = newMin;
        maxWord = newMax;
    }

    template<typename Func>
    __forceinline void ForEachReversed(Func&& FuncPtr) {
        if (minWord > maxWord) return;
        minWord = min(minWord, wordCount - 1);
        maxWord = min(maxWord, wordCount - 1);
        idx_t newMin = wordCount - 1;
        idx_t newMax = 0;
        bool first = true;
        for (idx_t word_id = maxWord; word_id >= minWord && word_id != IDX_MAX; word_id--) {
            size_t word = bits[word_id];
            if (word) {
                newMin = word_id;
                if (first) {
                    newMax = word_id;
                    first = false;
                }
            }
            while (word) {
                idx_t bit = WORD_MASK - CLZ(word);
                FuncPtr((word_id << WORD_DIV_SHR) | bit);
                word ^= size_t(1) << bit;
            }
        }
        minWord = newMin;
        maxWord = newMax;
    }
private:
    size_t* bits = nullptr;
    idx_t wordCount = 0;
};

struct ChannelSettings
{
    ChannelSettings() { bHidden = bMuted = false; SetColor(0x00000000); }
    void SetColor();
    void SetColor(color_t iColor, double dDark = 0.5, double dVeryDark = 0.2);

    bool bHidden, bMuted;
    color_t iPrimaryRGB, iDarkRGB, iVeryDarkRGB, iOrigBGR;
};
struct TrackSettings { ChannelSettings aChannels[16]; };

class CustomHashFunc {
public:
    unsigned operator() (MIDIChannelEvent* key) const {
        return (uint64_t)key & 0xFFFFFFFF;
    }
};

class CustomKeyEqualFunc {
public:
    bool operator() (MIDIChannelEvent* a, MIDIChannelEvent* b) const {
        return a == b;
    }
};

//Abstract base class
class GameState
{
public:
    enum GameError : uint8_t { Success = 0, BadPointer, JumpToFailure, OutOfMemory, DirectXError };
    enum State : uint8_t { Intro = 0, Splash, Practice };

    //Static methods
    static const wstring Errors[];
    static GameError ChangeState(GameState* pNextState, GameState** pDestObj);

    //Constructors
    GameState(HWND hWnd, Renderer11* pRenderer) : m_hWnd(hWnd), m_pRenderer(pRenderer), m_pNextState(NULL) {};
    virtual ~GameState() {};

    // Initialize after all other game states have been deleted
    virtual GameError Init() = 0;

    //Handle events
    virtual GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    //Run logic
    virtual GameError Logic() = 0;

    //Render scene
    virtual GameError Render() = 0;

    //Null for same state, 
    GameState* NextState() { return m_pNextState; };

    void SetHWnd(HWND hWnd) { m_hWnd = hWnd; }
    void SetRenderer(Renderer11* pRenderer) { m_pRenderer = pRenderer; }

protected:
    //Windows info
    HWND m_hWnd;

    //Rendering device
    Renderer11* m_pRenderer;

    GameState* m_pNextState;
};

class IntroScreen : public GameState
{
public:
    IntroScreen(HWND hWnd, Renderer11* pRenderer) : GameState(hWnd, pRenderer) {}

    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic();
    GameError Render();
};

class SplashScreen : public GameState
{
public:
    SplashScreen(HWND hWnd, Renderer11* pRenderer, bool enableSplash = true);
    ~SplashScreen() { delete m_pState; }

    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic();
    GameError Render();

private:
    void InitNotes(const vector<MIDIEvent*>& vEvents);
    void InitState();
    void ColorChannel(track_t iTrack, chan_t iChannel, color_t iColor, bool bRandom = false);
    void SetChannelSettings(const vector<bool>& vMuted, const vector<bool>& vHidden, const vector<color_t>& vColor);

    void UpdateState(idx_t idx, idx_t sister_idx);

    void RenderGlobals();
    void RenderNotes();
    void RenderNote(MIDIChannelEvent* pNote);
    float GetNoteX(key_t iNote);
    void GenNoteXTable();

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector<MIDIChannelEvent*> m_vEvents; // The channel events of the song
    sidx_t m_iStartPos, m_iEndPos;
    dynamic_bitset* m_pState = nullptr;
    Timer m_Timer; // Frame timers
    double m_dVolume;
    bool m_bMute;

    MIDIOutDevice m_OutDevice;

    static const float SharpRatio;
    static const mms_t TimeSpan = 500000;
    vector<TrackSettings> m_vTrackSettings;

    // Computed in RenderGlobal
    key_t m_iStartNote, m_iEndNote; // Start and end notes of the songs
    float m_fNotesX, m_fNotesY, m_fNotesCX, m_fNotesCY; // Notes position
    key_t m_iAllWhiteKeys; // Number of white keys are on the screen
    float m_fWhiteCX; // Width of the white keys

    float notex_table[128];
};

class MainScreen : public GameState
{
public:
    static const float KBPercent;

    MainScreen(wstring sMIDIFile, HWND hWnd, Renderer11* pRenderer);
    ~MainScreen() { delete m_pState; }

    // GameState functions
    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic();
    GameError Render();

    // Info
    bool IsValid() const { return m_MIDI.IsValid(); }
    const MIDI& GetMIDI() const { return m_MIDI; }

    // Settings
    void MuteChannel(track_t iTrack, chan_t iChannel, bool bMuted) { m_vTrackSettings[iTrack].aChannels[iChannel].bMuted = bMuted; }
    void HideChannel(track_t iTrack, chan_t iChannel, bool bHidden) { m_vTrackSettings[iTrack].aChannels[iChannel].bHidden = bHidden; }
    void ColorChannel(track_t iTrack, chan_t iChannel, color_t iColor, bool bRandom = false);
    void SetChannelSettings(const vector<bool>& vMuted, const vector<bool>& vHidden, const vector<color_t>& vColor);

private:
    // Initialization
    void InitColors();
    void InitState();

    // Logic
    void UpdateState(idx_t idx, idx_t sister_idx);
    void UpdateStateBackwards(idx_t start, idx_t end);
    void JumpTo(mms_t llStartTime, boolean loadingMode = false);
    void ApplyMarker(unsigned char* data, msgln_t size);
    void ApplyColor(MIDIMetaEvent* event);
    void AdvanceIterators(mms_t llTime, bool bIsJump);
    void SendSysEx(MIDISysExEvent* pSysEx);
    MIDIMetaEvent* GetPrevious(eventvec_t::const_iterator& itCurrent, const eventvec_t& vEventMap, msgln_t iDataLen);

    // MIDI helpers
    mtk_t GetCurrentTick(mms_t llStartTime);
    mtk_t GetCurrentTick(mms_t llStartTime, mtk_t iLastTempoTick, mms_t llLastTempoTime, bpm_t iMicroSecsPerBeat);
    mms_t GetTickTime(mtk_t iTick);
    mms_t GetTickTime(mtk_t iTick, mtk_t iLastTempoTick, mms_t llLastTempoTime, bpm_t iMicroSecsPerBeat);
    bpm_t GetBeat(mtk_t iTick, bpm_t iBeatType, mtk_t iLastTempoTick);
    mtk_t GetBeatTick(mtk_t iTick, bpm_t iBeatType, mtk_t iLastTempoTick);
    mms_t GetMinTime() const { return m_MIDI.GetInfo().llFirstNote - 3000000; }
    mms_t GetMaxTime() const { return m_MIDI.GetInfo().llTotalMicroSecs + 500000; }

    // Rendering
    void RenderGlobals();
    void RenderLines();
    void RenderNotes();
    void RenderNote(const MIDIChannelEvent* pNote);
    void GenNoteXTable();
    float GetNoteX(key_t iNote);
    void RenderKeys();
    void RenderText();
    void RenderStatusLine(unsigned char line, const wchar_t* left, const wchar_t* format, ...);
    void RenderStatus(LPRECT prcPos);
    void RenderMarker(const wstring& str);
    void RenderMessage(LPRECT prcMsg, LPRECT prcScr, const wstring& sMsg, char lnAlign);

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector<MIDIChannelEvent*> m_vEvents; // The channel events of the song
    vector<MIDIMetaEvent*> m_vMetaEvents; // The meta events of the song
    vector<MIDISysExEvent*> m_vSysExEvents; // The SysEx events of the song
    eventvec_t m_vTempo; // Tracked for drawing measure lines
    eventvec_t m_vSignature; // Measure lines again
    eventvec_t m_vMarkers; // Tracked for section names in some longer MIDIs
    eventvec_t m_vColors; // Tracked for section names in some longer MIDIs
    eventvec_t::const_iterator m_itNextTempo;
    eventvec_t::const_iterator m_itNextSignature;
    eventvec_t::const_iterator m_itNextMarker;
    eventvec_t::const_iterator m_itNextColor;
    vector<MIDISysExEvent*>::const_iterator m_itNextSysEx;
    bpm_t m_iMicroSecsPerBeat; // Tempo
    mtk_t m_iLastTempoTick; // Tempo
    mms_t m_llLastTempoTime; // Tempo
    bpm_t m_CurBeat, m_iBeatsPerMeasure, m_iBeatType, m_iClocksPerMet; // Time signature
    mtk_t m_iLastSignatureTick;
    wstring m_sMarker; // Current marker to display on the screen
    unsigned char* m_pMarkerData = nullptr; // Used for refreshing marker data when changing encoding on the fly
    msgln_t m_iMarkerSize = 0;
    uint8_t m_iCurEncoding;

    // color events and bend range and such
    bool Next_is_PBS[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
    short m_pBendsRange[16] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };

    // Playback
    sidx_t m_iStartPos, m_iEndPos, m_iPrevStartPos; // Postions of the start and end events that occur in the current window
    mms_t m_llTimeSpan;  // Times of the start and end events of the current window
    mms_t m_llPrevTime;
    dynamic_bitset* m_pState = nullptr;
    NoteColor m_pKeyColors[128]; // Per-key blended color for keyboard rendering.
    __uint128_t m_bKeyPressed; // Is key pressed?
    __forceinline bool IsPressed(key_t Key) { return m_bKeyPressed & (static_cast<__uint128_t>(1) << Key); }
    __forceinline void PushKey(key_t Key) { m_bKeyPressed |= (static_cast<__uint128_t>(1) << Key); }
    __forceinline void InitKeyColor() {
        m_bKeyPressed = 0;
        for (key_t i = 0; i < 12; i++) {
            const ChannelSettings& cs = MIDI::IsSharp(i) ? m_csKBSharp : m_csKBWhite;
            m_pKeyColors[i] = { cs.iPrimaryRGB, cs.iDarkRGB, cs.iVeryDarkRGB };
        }
        memcpy(m_pKeyColors + 12, m_pKeyColors, 12 * sizeof(NoteColor));
        memcpy(m_pKeyColors + 24, m_pKeyColors, 24 * sizeof(NoteColor));
        memcpy(m_pKeyColors + 48, m_pKeyColors, 48 * sizeof(NoteColor));
        memcpy(m_pKeyColors + 96, m_pKeyColors, 32 * sizeof(NoteColor));
    }
    double m_dSpeed; // Speed multiplier
    bool IsLastFrameReversed = false;
    bool IsReversedStateInitialized = false;
    double m_dNSpeed; // Note Speed multiplier
    bool m_bPaused; // Paused state
    Timer m_Timer; // Frame timers
    bool m_bMute;
    double m_dVolume;
    bool m_bTickMode = false;
    UINT nxtdelay = 1 << 6;
    mms_t JumpTarget = ~0;

    // FPS variables
    short m_iFPSCount;
    mms_t m_llFPSTime;
    double m_dFPS;
    uint8_t FrameCount = 0;

    // Devices
    MIDIOutDevice m_OutDevice;

    // Visual
    static const float SharpRatio;
    static const float KeyRatio;
    bool m_bShowKB;
    uint8_t m_eKeysShown;
    ChannelSettings m_csBackground;
    ChannelSettings m_csKBRed, m_csKBWhite, m_csKBSharp, m_csKBBackground;
    vector<TrackSettings> m_vTrackSettings;
    float m_pBends[16] = {};
    float m_pBendsValue[16] = {};
    wstring m_sCurBackground;
    bool m_bBackgroundLoaded;
    bool m_bRemoveOverlaps;

    float m_fZoomX, m_fOffsetX, m_fOffsetY;
    float m_fTempZoomX, m_fTempOffsetX, m_fTempOffsetY;
    bool m_bZoomMove, m_bTrackPos, m_bTrackZoom;
    POINT m_ptStartZoom, m_ptLastPos;

    float notex_table[128];

    // Computed in RenderGlobal
    key_t m_iStartNote, m_iEndNote; // Start and end notes of the songs
    key_t m_iAllWhiteKeys; // Number of white keys are on the screen
    bool m_bFlipKeyboard;
    float m_fNotesX, m_fNotesY, m_fNotesCX, m_fNotesCY; // Notes position
    float m_fWhiteCX; // Width of the white keys
    bool m_bSameWidth;
    bool m_bMapVel;

    // Frame dumping stuff
    bool m_bDumpFrames = false;
    vector<unsigned char> m_vImageData;
    HANDLE m_hVideoPipe;

    // Debug assertion fail workaround
    bool m_bNextMarkerInited = false;
};