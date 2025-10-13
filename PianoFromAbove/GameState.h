﻿/*************************************************************************************************
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

inline long long m_llStartTime;
inline int m_iStartTick;
inline long* m_vNCTable = nullptr;
inline string llStartTimeFormatted;
inline long polyphony;
inline string polyFormatted;
inline long nps;
inline string npsFormatted;
inline long passed;
inline string passedFormatted;
inline uint8_t FrameCount = 0;
inline int width = -1;
inline int height = -1;
inline uint16_t resolution = -1;
inline char CheatEngineCaption[(1<<7)*(1<<10)] = {};
inline size_t TotalNC;
inline long long MinimalTime;
inline long long TotalTime;
inline string TotalTimeFormatted;
inline char Difficulty[1 << 10] = {};
inline bool UpdateNotePos = true;
inline long long JumpTarget = ~0;
inline UINT nxtdelay = 1<<6;
inline static const signed long long MS = 1e+3;
inline static const signed long long S = 1e+6;

//Abstract base class
class GameState
{
public:
    enum GameError : uint8_t { Success = 0, BadPointer, DirectXError };
    enum State : uint8_t { Intro = 0, Splash, Practice };

    //Static methods
    static const wstring Errors[];
    static GameError ChangeState( GameState *pNextState, GameState **pDestObj );

    //Constructors
    GameState( HWND hWnd, D3D12Renderer *pRenderer ) : m_hWnd( hWnd ), m_pRenderer( pRenderer ), m_pNextState( NULL ) {};
    virtual ~GameState( void ) {};

    // Initialize after all other game states have been deleted
    virtual GameError Init() = 0;

    //Handle events
    virtual GameError MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    //Run logic
    virtual GameError Logic() = 0;

    //Render scene
    virtual GameError Render() = 0;

    //Null for same state, 
    GameState *NextState() { return m_pNextState; };

    void SetHWnd( HWND hWnd ) { m_hWnd = hWnd; }
    void SetRenderer( D3D12Renderer *pRenderer ) { m_pRenderer = pRenderer; }

protected:
    //Windows info
    HWND m_hWnd;

    //Rendering device
    D3D12Renderer *m_pRenderer;

    GameState *m_pNextState;

    static const int QueueSize = 50;
};

struct ChannelSettings
{
    ChannelSettings() { bHidden = bMuted = false; SetColor( 0x00000000 ); }
    void SetColor();
    void SetColor( unsigned int iColor, double dDark = 0.5, double dVeryDark = 0.2 );

    bool bHidden, bMuted;
    unsigned int iPrimaryRGB, iDarkRGB, iVeryDarkRGB, iOrigBGR;
};
struct TrackSettings { ChannelSettings aChannels[16]; };

class SplashScreen : public GameState
{
public:
    SplashScreen( HWND hWnd, D3D12Renderer *pRenderer, bool enableSplash = true);

    GameError MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    GameError Init();
    GameError Logic();
    GameError Render();

private:
    void InitNotes( const vector< MIDIEvent* > &vEvents );
    void InitState();
    void ColorChannel( int iTrack, int iChannel, unsigned int iColor, bool bRandom = false );
    void SetChannelSettings( const vector< bool > &vMuted, const vector< bool > &vHidden, const vector< unsigned > &vColor );

    void UpdateState( int iPos );

    void RenderGlobals();
    void RenderNotes();
    void RenderNote(MIDIChannelEvent* pNote);
    float GetNoteX( int iNote );
    void GenNoteXTable();

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector< MIDIChannelEvent* > m_vEvents; // The channel events of the song
    int m_iStartPos;
    int m_iEndPos;
    vector<int> m_vState[128];  // The notes that are on at time m_llStartTime.
    Timer m_Timer; // Frame timers
    double m_dVolume;
    bool m_bMute;

    MIDIOutDevice m_OutDevice;

    static const float SharpRatio;
    static const long long TimeSpan = 500000;
    vector< TrackSettings > m_vTrackSettings;

    // Computed in RenderGlobal
    uint8_t m_iStartNote, m_iEndNote; // Start and end notes of the songs
    float m_fNotesX, m_fNotesY, m_fNotesCX, m_fNotesCY; // Notes position
    uint8_t m_iAllWhiteKeys; // Number of white keys are on the screen
    float m_fWhiteCX; // Width of the white keys
    long long m_llRndStartTime; // Rounded start time to make stuff drop at the same time

    float notex_table[128];
};

class IntroScreen : public GameState
{
public:
    IntroScreen( HWND hWnd, D3D12Renderer *pRenderer ) : GameState( hWnd, pRenderer ) {}

    GameError MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
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
    unsigned idx;
    unsigned sister_idx;
} thread_work_t;

class MainScreen : public GameState
{
public:
    static const float KBPercent;

    MainScreen( wstring sMIDIFile, State eGameMode, HWND hWnd, D3D12Renderer *pRenderer );

    // GameState functions
    GameError MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    GameError Init();
    GameError Logic( void );
    GameError Render( void );

    // Info
    bool IsValid() const { return m_MIDI.IsValid(); }
    const MIDI& GetMIDI() const { return m_MIDI; }

    // Settings
    void ToggleMuted( int iTrack, int iChannel ) { m_vTrackSettings[iTrack].aChannels[iChannel].bMuted =
                                                  !m_vTrackSettings[iTrack].aChannels[iChannel].bMuted; }
    void ToggleHidden( int iTrack, int iChannel ) { m_vTrackSettings[iTrack].aChannels[iChannel].bHidden =
                                                   !m_vTrackSettings[iTrack].aChannels[iChannel].bHidden; }
    void MuteChannel( int iTrack, int iChannel, bool bMuted ) { m_vTrackSettings[iTrack].aChannels[iChannel].bMuted = bMuted; }
    void HideChannel( int iTrack, int iChannel, bool bHidden ) { m_vTrackSettings[iTrack].aChannels[iChannel].bHidden = bHidden; }
    void ColorChannel( int iTrack, int iChannel, unsigned int iColor, bool bRandom = false );
    ChannelSettings* GetChannelSettings( int iChannel );
    void SetChannelSettings( const vector< bool > &vMuted, const vector< bool > &vHidden, const vector< unsigned > &vColor );

private:
    // Initialization
    void InitColors();
    void InitState();

    // Logic
    void UpdateState(int key, const thread_work_t& work);
    void UpdateStateBackwards(int key, const thread_work_t& work);
    void JumpTo(long long llStartTime, boolean loadingMode = false);
    void ApplyMarker(unsigned char* data, size_t size);
    void ApplyColor(MIDIMetaEvent* event);
    void AdvanceIterators( long long llTime, bool bIsJump );
    MIDIMetaEvent* GetPrevious( eventvec_t::const_iterator &itCurrent,
                                const eventvec_t &vEventMap, int iDataLen );

    // MIDI helpers
    int GetCurrentTick( long long llStartTime );
    int GetCurrentTick( long long llStartTime, int iLastTempoTick, long long llLastTempoTime, int iMicroSecsPerBeat );
    long long GetTickTime( int iTick );
    long long GetTickTime( int iTick, int iLastTempoTick, long long llLastTempoTime, int iMicroSecsPerBeat );
    int GetBeat( int iTick, int iBeatType, int iLastTempoTick );
    int GetBeatTick( int iTick, int iBeatType, int iLastTempoTick );
    long long GetMinTime() const { return m_MIDI.GetInfo().llFirstNote - 3000000; }
    long long GetMaxTime() const { return m_MIDI.GetInfo().llTotalMicroSecs + 500000; }

    // Rendering
    void RenderGlobals();
    void RenderLines();
    void RenderNotes();
    void RenderNote(const MIDIChannelEvent* pNote);
    void GenNoteXTable();
    float GetNoteX( int iNote );
    void RenderKeys();
    void RenderText();
    void RenderStatusLine(int line, const char* left, const char* format, ...);
    void RenderStatus( LPRECT prcPos );
    void RenderMarker(const char* str);
    void RenderMessage(LPRECT prcMsg, const char* sMsg);

    // MIDI info
    MIDI m_MIDI; // The song to display
    vector< MIDIChannelEvent* > m_vEvents; // The channel events of the song
    vector< MIDIMetaEvent* > m_vMetaEvents; // The meta events of the song
    eventvec_t m_vTempo; // Tracked for drawing measure lines
    eventvec_t m_vSignature; // Measure lines again
    eventvec_t m_vMarkers; // Tracked for section names in some longer MIDIs
    eventvec_t m_vColors; // Tracked for section names in some longer MIDIs
    eventvec_t::const_iterator m_itNextTempo;
    eventvec_t::const_iterator m_itNextSignature;
    eventvec_t::const_iterator m_itNextMarker;
    eventvec_t::const_iterator m_itNextColor;
    uint32_t m_iMicroSecsPerBeat, m_iLastTempoTick; // Tempo
    long long m_llLastTempoTime; // Tempo
    int m_CurBeat, m_iBeatsPerMeasure, m_iBeatType, m_iClocksPerMet, m_iLastSignatureTick; // Time signature
    std::string m_sMarker; // Current marker to display on the screen
    unsigned char* m_pMarkerData = nullptr; // Used for refreshing marker data when changing encoding on the fly
    size_t m_iMarkerSize = 0;
    int m_iCurEncoding;

    // color events and bend range and such
    bool Next_is_PBS[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
    short m_pBendsRange[16] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };

    // Playback
    State m_eGameMode;
    long long m_iStartPos, m_iEndPos, m_iPrevStartPos; // Postions of the start and end events that occur in the current window
    long long m_llTimeSpan;  // Times of the start and end events of the current window
    long long m_llPrevTime;
    vector<int> m_vState[128];  // The notes that are on at time m_llStartTime.
    vector<thread_work_t> m_vThreadWork[128];
    int m_pNoteState[128]; // The last note that was turned on
    double m_dSpeed; // Speed multiplier
    bool IsLastFrameReversed = false;
    bool IsReversedStateInitialized = false;
    double m_dNSpeed; // Note Speed multiplier
    bool m_bPaused; // Paused state
    Timer m_Timer; // Frame timers
    bool m_bMute;
    double m_dVolume;
    bool m_bTickMode = false;

    // FPS variables
    int m_iFPSCount;
    long long m_llFPSTime;
    double m_dFPS;

    // Devices
    MIDIOutDevice m_OutDevice;

    // Visual
    static const float SharpRatio;
    static const float KeyRatio;
    bool m_bShowKB;
    int m_eKeysShown;
    ChannelSettings m_csBackground;
    ChannelSettings m_csKBRed, m_csKBWhite, m_csKBSharp, m_csKBBackground;
    vector< TrackSettings > m_vTrackSettings;
    float m_pBends[16] = {};
    float m_pBendsValue[16] = {};
    std::wstring m_sCurBackground;
    bool m_bBackgroundLoaded;

    float m_fZoomX, m_fOffsetX, m_fOffsetY;
    float m_fTempZoomX, m_fTempOffsetX, m_fTempOffsetY;
    bool m_bZoomMove, m_bTrackPos, m_bTrackZoom;
    POINT m_ptStartZoom, m_ptLastPos;

    float notex_table[128];

    // Computed in RenderGlobal
    int m_iStartNote, m_iEndNote; // Start and end notes of the songs
    bool m_bFlipKeyboard;
    float m_fNotesX, m_fNotesY, m_fNotesCX, m_fNotesCY; // Notes position
    int m_iAllWhiteKeys; // Number of white keys are on the screen
    float m_fWhiteCX; // Width of the white keys
    long long m_llRndStartTime; // Rounded start time to make stuff drop at the same time
    
    // Frame dumping stuff
    bool m_bDumpFrames = false;
    std::vector<unsigned char> m_vImageData;
    HANDLE m_hVideoPipe;

    // Debug assertion fail workaround
    bool m_bNextMarkerInited = false;
};