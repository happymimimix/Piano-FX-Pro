/*************************************************************************************************
*
* File: Config.h
*
* Description: Defines the configuration objects
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <vector>
#include <map>
#include <string>
#include <TinyXML/tinyxml.h>
#include <MIDI.h>
#include <GameState.h>
#include <MainProcs.h>

#define APPNAME "Piano From Above"
#define APPNAMENOSPACES "PianoFromAbove"
#define CLASSNAME TEXT("PFX")
#define GFXCLASSNAME TEXT("PFXGFX")
#define POSNCLASSNAME TEXT("PFXPOSN")

class ISettings;
class Config;

class ISettings
{
public:
    virtual void LoadDefaultValues() = 0;
    virtual void LoadConfigValues(TiXmlElement* txRoot) = 0;
    virtual bool SaveConfigValues(TiXmlElement* txRoot) = 0;
};

struct VisualSettings : public ISettings
{
    void LoadDefaultValues();
    void LoadDefaultColors();
    void LoadDefaultShifts();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    enum KeysShown : uint8_t { All, Song, Custom } eKeysShown;
    key_t iFirstKey, iLastKey;
    color_t colors[16], iBkgColor;
    color_t iBarColor;
    bool bRandomizeColor;
    wstring sBackground;
    color_t iBackgroundHSL;
};

struct AudioSettings : public ISettings
{
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    void LoadMIDIDevices();
    vector<wstring> vMIDIOutDevices;
    win32_t iOutDevice;
    wstring sDesiredOut;
    bool bKDMAPI;
};

struct VideoSettings : public ISettings
{
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    bool bTickBased;
    bool bVisualizePitchBends;
    bool bPortDependentPitchBends;
    bool bSameWidth;
    bool bMapVel;
    bool bHighlight;
    bool bOR;
    bool bShowMarkers;
    bool bShowLyrics;
    bool bTrackDependentLyrics;
    WORD eTextEncoding;
    bool bLimitFPS;
    bool bDebug;
    bool bTopmostStats;
    bool bDisableStats;
};

struct ControlSettings : public ISettings
{
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    double dFwdBackSecs, dSpeedUpPct, dWheelMul;
    bool bAlwaysShowControls;
    bool bPhigros;
    bool bManualTimer;
    key_t iVelocityThreshold;
    key_t eInstrumentOverride;
    bool bAllowUnknown;
    bool bLiveMode;
    wstring sSplashMIDI;
};

struct ExportSettings : public ISettings
{
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    bool bDumpFrames;
    bool bFullScreenCapture;
    unsigned short iFPS;
    bool bDumpEvents;
    bool bPreciseEventDump;
    bool bUseSMPTE;
};

class PlaybackSettings : public ISettings
{
public:
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    void ToggleMute(bool bUpdateGUI = false) { SetMute(!m_bMute, bUpdateGUI); }
    void TogglePaused(bool bUpdateGUI = false) { SetPaused(!m_bPaused, bUpdateGUI); }
    void SetPosition(winword_t iPosition) { ::SetPosition(iPosition); }

    // Set accessors. A bit more advanced because they optionally update the GUI
    void SetPlayMode(GameState::State ePlayMode, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetPlayMode(ePlayMode); m_ePlayMode = ePlayMode; }
    void SetPlayable(bool bPlayable, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetPlayable(bPlayable); m_bPlayable = bPlayable; }
    void SetPaused(bool bPaused, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetPlayPauseStop(!bPaused, bPaused, false); m_bPaused = bPaused; }
    void SetStopped(bool bUpdateGUI = false) { if (bUpdateGUI) ::SetPlayPauseStop(false, false, true); m_bPaused = true; }
    void SetSpeed(double dSpeed, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetSpeed(dSpeed); m_dSpeed = dSpeed; }
    void SetNSpeed(double dNSpeed, bool bUpdateGUI = false) { dNSpeed = max(min(dNSpeed, 10.0), 0.005); if (bUpdateGUI) ::SetNSpeed(dNSpeed); m_dNSpeed = dNSpeed; }
    void SetVolume(double dVolume, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetVolume(dVolume); m_dVolume = max(min(dVolume, 2.0), 0.0); }
    void SetMute(bool bMute, bool bUpdateGUI = false) { if (bUpdateGUI) ::SetMute(bMute); m_bMute = bMute; }

    // Get accessors. Simple.
    GameState::State GetPlayMode() const { return m_ePlayMode; }
    bool GetPlayable() const { return m_bPlayable; }
    bool GetPaused() const { return m_bPaused; }
    bool GetMute() const { return m_bMute; }
    double GetSpeed() const { return m_dSpeed; }
    double GetNSpeed() const { return m_dNSpeed; }
    double GetVolume() const { return max(min(m_dVolume, 2.0), 0.0); }
    wstring GetPausedAddress() const { return GetAddress(m_bPaused); }
    wstring GetMuteAddress() const { return GetAddress(m_bMute); }
    wstring GetSpeedAddress() const { return GetAddress(m_dSpeed); }
    wstring GetNSpeedAddress() const { return GetAddress(m_dNSpeed); }
    wstring GetVolumeAddress() const { return GetAddress(m_dVolume); }
    uint8_t GetPausedSize() const { return sizeof(m_bPaused); }
    uint8_t GetMuteSize() const { return sizeof(m_bMute); }
    uint8_t GetSpeedSize() const { return sizeof(m_dSpeed); }
    uint8_t GetNSpeedSize() const { return sizeof(m_dNSpeed); }
    uint8_t GetVolumeSize() const { return sizeof(m_dVolume); }

private:
    GameState::State m_ePlayMode;
    bool m_bPlayable, m_bPaused;
    bool m_bMute;
    double m_dSpeed, m_dNSpeed, m_dVolume;
};

class ViewSettings : public ISettings
{
public:
    void LoadDefaultValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues(TiXmlElement* txRoot);

    void ToggleControls(bool bUpdateGUI = false) { SetControls(!m_bControls, bUpdateGUI); }
    void ToggleKeyboard(bool bUpdateGUI = false) { SetKeyboard(!m_bKeyboard, bUpdateGUI); }
    void ToggleOnTop(bool bUpdateGUI = false) { SetOnTop(!m_bOnTop, bUpdateGUI); }
    void ToggleFullScreen(bool bUpdateGUI = false) { SetFullScreen(!m_bFullScreen, bUpdateGUI); }

    void SetMainPos(win32_t iMainLeft, win32_t iMainTop) { m_iMainLeft = iMainLeft; m_iMainTop = iMainTop; }
    void SetMainSize(win32_t iMainWidth, win32_t iMainHeight) { m_iMainWidth = iMainWidth; m_iMainHeight = iMainHeight; }
    void SetOffsetX(float fOffsetX) { m_fOffsetX = fOffsetX; }
    void SetOffsetY(float fOffsetY) { m_fOffsetY = fOffsetY; }
    void SetZoomX(float fZoomX) { m_fZoomX = fZoomX; }
    void SetControls(bool bControls, bool bUpdateGUI = false) { m_bControls = bControls; if (bUpdateGUI) ::ShowControls(bControls); }
    void SetKeyboard(bool bKeyboard, bool bUpdateGUI = false) { m_bKeyboard = bKeyboard; if (bUpdateGUI) ::ShowKeyboard(bKeyboard); }
    void SetOnTop(bool bOnTop, bool bUpdateGUI = false) { m_bOnTop = bOnTop; if (bUpdateGUI) ::SetOnTop(bOnTop); }
    void SetFullScreen(bool bFullScreen, bool bUpdateGUI = false) { m_bFullScreen = bFullScreen; if (bUpdateGUI) ::SetFullScreen(bFullScreen); }
    void SetZoomMove(bool bZoomMove) { m_bZoomMove = bZoomMove; ::SetZoomMove(bZoomMove); }

    win32_t GetMainLeft() const { return m_iMainLeft == -32000 ? CW_USEDEFAULT : m_iMainLeft; }
    win32_t GetMainTop() const { return m_iMainTop == -32000 ? CW_USEDEFAULT : m_iMainTop; }
    win32_t GetMainWidth() const { return m_iMainWidth; }
    win32_t GetMainHeight() const { return m_iMainHeight; }
    float GetOffsetX() const { return m_fOffsetX; }
    float GetOffsetY() const { return m_fOffsetY; }
    float GetZoomX() const { return m_fZoomX; }
    wstring GetOffsetXAddress() const { return GetAddress(m_fOffsetX); }
    wstring GetOffsetYAddress() const { return GetAddress(m_fOffsetY); }
    wstring GetZoomXAddress() const { return GetAddress(m_fZoomX); }
    uint8_t GetOffsetXSize() const { return sizeof(m_fOffsetX); }
    uint8_t GetOffsetYSize() const { return sizeof(m_fOffsetY); }
    uint8_t GetZoomXSize() const { return sizeof(m_fZoomX); }
    bool GetControls() const { return m_bControls; }
    bool GetKeyboard() const { return m_bKeyboard; }
    wstring GetKeyboardAddress() const { return GetAddress(m_bKeyboard); }
    uint8_t GetKeyboardVarSize() const { return sizeof(m_bKeyboard); }
    bool GetOnTop() const { return m_bOnTop; }
    bool GetFullScreen() const { return m_bFullScreen; }
    bool GetZoomMove() const { return m_bZoomMove; }

private:
    bool m_bControls, m_bKeyboard, m_bOnTop, m_bFullScreen, m_bZoomMove;
    float m_fOffsetX, m_fOffsetY, m_fZoomX;
    win32_t m_iMainLeft, m_iMainTop, m_iMainWidth, m_iMainHeight;
};

class Config : public ISettings
{
public:
    // Singleton
    static Config& GetConfig();
    static wstring GetFolder();

    // Interface
    void LoadDefaultValues();
    void LoadConfigValues();
    void LoadConfigValues(TiXmlElement* txRoot);
    bool SaveConfigValues();
    bool SaveConfigValues(TiXmlElement* txRoot);

    void LoadMIDIDevices() { m_AudioSettings.LoadMIDIDevices(); }

    const VisualSettings& GetVisualSettings() const { return m_VisualSettings; }
    const AudioSettings& GetAudioSettings() const { return m_AudioSettings; }
    const VideoSettings& GetVideoSettings() const { return m_VideoSettings; }
    const ControlSettings& GetControlSettings() const { return m_ControlSettings; }
    const ExportSettings& GetExportSettings() const { return m_ExportSettings; }
    PlaybackSettings& GetPlaybackSettings() { return m_PlaybackSettings; }
    ViewSettings& GetViewSettings() { return m_ViewSettings; }

    void SetVisualSettings(const VisualSettings& VisualSettings) { m_VisualSettings = VisualSettings; }
    void SetAudioSettings(const AudioSettings& audioSettings) { m_AudioSettings = audioSettings; }
    void SetVideoSettings(const VideoSettings& videoSettings) { m_VideoSettings = videoSettings; }
    void SetControlSettings(const ControlSettings& ControlSettings) { m_ControlSettings = ControlSettings; }
    void SetexportSettings(const ExportSettings& ExportSettings) { m_ExportSettings = ExportSettings; }

private:
    // Singleton
    Config();
    ~Config() {}
    Config(const Config&);
    Config& operator=(const Config&);

    VisualSettings m_VisualSettings;
    AudioSettings m_AudioSettings;
    VideoSettings m_VideoSettings;
    ControlSettings m_ControlSettings;
    ExportSettings m_ExportSettings;
    PlaybackSettings m_PlaybackSettings;
    ViewSettings m_ViewSettings;
};