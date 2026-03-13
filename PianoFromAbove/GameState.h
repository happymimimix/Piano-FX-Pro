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
using namespace std;

#include "Renderer.h"
#include "MIDI.h"
#include "Misc.h"

inline mms_t m_llStartTime;
inline mms_t m_llRndStartTime;
inline mtk_t m_iStartTick;
inline idx_t* m_vNCTable = nullptr;
inline string llStartTimeFormatted;
inline idx_t polyphony;
inline string polyFormatted;
inline idx_t nps;
inline string npsFormatted;
inline idx_t passed;
inline string passedFormatted;
inline idx_t TotalNC;
inline win32_t width = -1;
inline win32_t height = -1;
inline uint16_t resolution = -1;
inline char CheatEngineCaption[(1 << 7) * (1 << 10)] = {};
inline mms_t TotalTime;
inline string TotalTimeFormatted;
inline char Difficulty[1 << 10] = {};
inline bool UpdateNotePos = true;
inline static const mms_t MS = 1e+3;
inline static const mms_t S = 1e+6;
inline volatile bool CE_Connected = false;
inline volatile bool CE_DoNextTick = false;
inline volatile bool CE_Responded = false;

inline void GetGDI(HWND hGDI, win32_t W, win32_t H, char* Output) {
    HDC GDIdc = GetDC(hGDI);
    HDC MEMdc = CreateCompatibleDC(GDIdc);
    HBITMAP BMP = CreateCompatibleBitmap(GDIdc, W, H);
    SelectObject(MEMdc, BMP);
    BitBlt(MEMdc, 0, 0,W,H, GDIdc, 0, 0, SRCCOPY);
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = W;
    bmpInfo.bmiHeader.biHeight = -H;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    GetDIBits(MEMdc, BMP, 0, H, Output, &bmpInfo, DIB_RGB_COLORS);
    DeleteDC(MEMdc);
    DeleteObject(BMP);
    ReleaseDC(hGDI, GDIdc);
}


//Abstract base class
class GameState
{
public:
    enum GameError : uint8_t { Success = 0, BadPointer, OutOfMemory, DirectXError};
    enum State : uint8_t { Intro = 0, Splash, Practice };

    //Static methods
    static const wstring Errors[];
    static GameError ChangeState(GameState* pNextState, GameState** pDestObj);

    //Constructors
    GameState(HWND hWnd, Renderer11* pRenderer) : m_hWnd(hWnd), m_pRenderer(pRenderer), m_pNextState(NULL) {};
    virtual ~GameState(void) {};

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

struct ChannelSettings
{
    ChannelSettings() { bHidden = bMuted = false; SetColor(0x00000000); }
    void SetColor();
    void SetColor(color_t iColor, double dDark = 0.5, double dVeryDark = 0.2);

    bool bHidden, bMuted;
    color_t iPrimaryRGB, iDarkRGB, iVeryDarkRGB, iOrigBGR;
};
struct TrackSettings { ChannelSettings aChannels[16]; };

class SplashScreen : public GameState
{
public:
    SplashScreen(HWND hWnd, Renderer11* pRenderer, bool enableSplash = true);

    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic();
    GameError Render();

private:
    void InitNotes(const vector< MIDIEvent* >& vEvents);
    void InitState();
    void ColorChannel(track_t iTrack, chan_t iChannel, color_t iColor, bool bRandom = false);
    void SetChannelSettings(const vector<bool>& vMuted, const vector<bool>& vHidden, const vector<color_t>& vColor);

    void UpdateState(sidx_t iPos);

    void RenderGlobals();
    void RenderNotes();
    void RenderNote(MIDIChannelEvent* pNote);
    float GetNoteX(key_t iNote);
    void GenNoteXTable();

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector< MIDIChannelEvent* > m_vEvents; // The channel events of the song
    sidx_t m_iStartPos, m_iEndPos;
    vector<sidx_t> m_vState[128];  // The notes that are on at time m_llStartTime.
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

class IntroScreen : public GameState
{
public:
    IntroScreen(HWND hWnd, Renderer11* pRenderer) : GameState(hWnd, pRenderer) {}

    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic();
    GameError Render();
};

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

typedef struct {
    sidx_t idx;
    sidx_t sister_idx;
} thread_work_t;

class MainScreen : public GameState
{
public:
    static const float KBPercent;

    MainScreen(wstring sMIDIFile, State eGameMode, HWND hWnd, Renderer11* pRenderer);

    // GameState functions
    GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    GameError Init();
    GameError Logic(void);
    GameError Render(void);

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
    void UpdateState(key_t key, const thread_work_t& work);
    void UpdateStateBackwards(key_t key, const thread_work_t& work);
    void JumpTo(mms_t llStartTime, boolean loadingMode = false);
    void ApplyMarker(unsigned char* data, msgln_t size);
    void ApplyColor(MIDIMetaEvent* event);
    void AdvanceIterators(mms_t llTime, bool bIsJump);
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
    void RenderMessage(LPRECT prcMsg, const wstring& sMsg);

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector<MIDIChannelEvent*> m_vEvents; // The channel events of the song
    vector<MIDIMetaEvent*> m_vMetaEvents; // The meta events of the song
    eventvec_t m_vTempo; // Tracked for drawing measure lines
    eventvec_t m_vSignature; // Measure lines again
    eventvec_t m_vMarkers; // Tracked for section names in some longer MIDIs
    eventvec_t m_vColors; // Tracked for section names in some longer MIDIs
    eventvec_t::const_iterator m_itNextTempo;
    eventvec_t::const_iterator m_itNextSignature;
    eventvec_t::const_iterator m_itNextMarker;
    eventvec_t::const_iterator m_itNextColor;
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
    bool Next_is_PBS[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    short m_pBendsRange[16] = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

    // Playback
    State m_eGameMode;
    sidx_t m_iStartPos, m_iEndPos, m_iPrevStartPos; // Postions of the start and end events that occur in the current window
    mms_t m_llTimeSpan;  // Times of the start and end events of the current window
    mms_t m_llPrevTime;
    vector<sidx_t> m_vState[128];  // The notes that are on at time m_llStartTime.
    vector<thread_work_t> m_vThreadWork[128];
    sidx_t m_pNoteState[128]; // The last note that was turned on
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