/*************************************************************************************************
*
* File: Config.cpp
*
* Description: Implements the configuration objects
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <Windows.h>
#include <Shlobj.h>
#include <TChar.h>

#include <fstream>
using namespace std;

#include "Config.h"
#include "Misc.h"
//-----------------------------------------------------------------------------
// Main Config class
//-----------------------------------------------------------------------------

Config& Config::GetConfig()
{
    static Config instance;
    return instance;
}

Config::Config()
{
    LoadDefaultValues();
    LoadConfigValues();
}

string Config::GetFolder()
{
    char sAppData[MAX_PATH];
    if (FAILED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, sAppData)))
        return string();

    strcat_s(sAppData, "\\");
    strcat_s(sAppData, APPNAME);
    if (GetFileAttributesA(sAppData) == INVALID_FILE_ATTRIBUTES)
        if (!CreateDirectoryA(sAppData, NULL))
            return string();

    return sAppData;
}

void Config::LoadDefaultValues()
{
    m_VisualSettings.LoadDefaultValues();
    m_AudioSettings.LoadDefaultValues();
    m_VideoSettings.LoadDefaultValues();
    m_ControlsSettings.LoadDefaultValues();
    m_PlaybackSettings.LoadDefaultValues();
    m_ViewSettings.LoadDefaultValues();
}

void Config::LoadConfigValues()
{
    // Where to load?
    string sPath = GetFolder();
    if (sPath.length() == 0) return;

    // Load it
    TiXmlDocument doc(sPath + "\\Config.xml");
    if (!doc.LoadFile()) return;

    // Get the root element
    TiXmlElement* txRoot = doc.FirstChildElement();
    if (!txRoot) return;

    LoadConfigValues(txRoot);
}

void Config::LoadConfigValues(TiXmlElement* txRoot)
{
    m_VisualSettings.LoadConfigValues(txRoot);
    m_AudioSettings.LoadConfigValues(txRoot);
    m_VideoSettings.LoadConfigValues(txRoot);
    m_ControlsSettings.LoadConfigValues(txRoot);
    m_PlaybackSettings.LoadConfigValues(txRoot);
    m_ViewSettings.LoadConfigValues(txRoot);
}

bool Config::SaveConfigValues()
{
    // Where to save?
    string sPath = GetFolder();
    if (sPath.length() == 0) return false;

    // Create the XML document
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);
    TiXmlElement* txRoot = new TiXmlElement(APPNAMENOSPACES);
    doc.LinkEndChild(txRoot);

    // Save each of the config
    SaveConfigValues(txRoot);

    // Write it!
    return doc.SaveFile(sPath + "\\Config.xml");
}

bool Config::SaveConfigValues(TiXmlElement* txRoot)
{
    bool bSaved = true;
    bSaved &= m_VisualSettings.SaveConfigValues(txRoot);
    bSaved &= m_AudioSettings.SaveConfigValues(txRoot);
    bSaved &= m_VideoSettings.SaveConfigValues(txRoot);
    bSaved &= m_ControlsSettings.SaveConfigValues(txRoot);
    bSaved &= m_PlaybackSettings.SaveConfigValues(txRoot);
    bSaved &= m_ViewSettings.SaveConfigValues(txRoot);
    return bSaved;
}

//-----------------------------------------------------------------------------
// LoadDefaultValues
//-----------------------------------------------------------------------------

void VisualSettings::LoadDefaultValues()
{
    eKeysShown = All;
    iFirstKey = 0;
    iLastKey = 127;
    bRandomizeColor = false;
    sBackground = L"";

    VisualSettings::LoadDefaultColors();
}

void VisualSettings::LoadDefaultColors()
{
    iBkgColor = 0x007F7F00;
    iBarColor = 0x000000FF;
    int R, G, B = 0, S = 80, V = 100;
    int iColors = sizeof(colors) / sizeof(colors[0]);
    for (int i = 10, count = 0; count < iColors; i = (i + 7) % iColors, count++)
    {
        Util::HSVtoRGB(360 * i / iColors, S, V, R, G, B);
        colors[count] = RGB(R, G, B);
    }
    swap(colors[2], colors[4]);
}

void AudioSettings::LoadDefaultValues()
{
    iOutDevice = -1;
    LoadMIDIDevices();
    bKDMAPI = false;
}

void VideoSettings::LoadDefaultValues()
{
    bTickBased = false;
    bVisualizePitchBends = true;
    bSameWidth = false;
    bMapVel = false;
    bShowMarkers = true;
    eMarkerEncoding = MarkerEncoding::CP437;
    bLimitFPS = true;
    bDebug = false;
    bDisableUI = false;
    bOR = false;
}

void ControlsSettings::LoadDefaultValues()
{
    dFwdBackSecs = 5;
    dSpeedUpPct = 10;
    bAlwaysShowControls = false;
    bPhigros = false;
    sSplashMIDI = L"";
    iVelocityThreshold = 0;
    bDumpFrames = false;
}

void PlaybackSettings::LoadDefaultValues()
{
    m_ePlayMode = GameState::Splash;
    m_bMute = false;
    m_bPlayable = false;
    m_bPaused = true;
    m_dSpeed = 1.0;
    m_dNSpeed = 0.25;
    m_dVolume = 1.0;
}

void ViewSettings::LoadDefaultValues()
{
    m_bControls = true;
    m_bKeyboard = true;
    m_bOnTop = false;
    m_bFullScreen = false;
    m_fOffsetX = 0.0f;
    m_fOffsetY = 0.0f;
    m_fZoomX = 1.0f;
    m_iMainLeft = CW_USEDEFAULT;
    m_iMainTop = CW_USEDEFAULT;
    m_iMainWidth = 640;
    m_iMainHeight = 480;
}

void AudioSettings::LoadMIDIDevices()
{
    wstring oldOutDev(iOutDevice >= 0 ? vMIDIOutDevices[iOutDevice] : L"");
    iOutDevice = -1;
    vMIDIOutDevices.clear();
    int iNumOutDevs = midiOutGetNumDevs();
    for (int i = 0; i < iNumOutDevs; i++)
    {
        MIDIOUTCAPS moc;
        midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS));
        vMIDIOutDevices.push_back(moc.szPname);

        if (sDesiredOut == vMIDIOutDevices[i])
            iOutDevice = i;
        if (oldOutDev == vMIDIOutDevices[i] && iOutDevice < 0)
            iOutDevice = i;
    }
    if (iOutDevice < 0)
        iOutDevice = iNumOutDevs - 1;
}

//-----------------------------------------------------------------------------
// LoadConfigValues
//-----------------------------------------------------------------------------

void VisualSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVisual = txRoot->FirstChildElement("Visual");
    if (!txVisual) return;

    // Attributes
    int iAttrVal;
    if (txVisual->QueryIntAttribute("KeysShown", &iAttrVal) == TIXML_SUCCESS)
        eKeysShown = static_cast<KeysShown>(max(KeysShown::All, min(iAttrVal, KeysShown::Custom)));
    txVisual->QueryIntAttribute("FirstKey", &iFirstKey);
    txVisual->QueryIntAttribute("LastKey", &iLastKey);

    //Colors
    int r, g, b, a = 0;
    size_t i = 0;
    TiXmlElement* txColors = txVisual->FirstChildElement("Colors");
    if (txColors)
        for (TiXmlElement* txColor = txColors->FirstChildElement("Color");
            txColor && i < sizeof(colors) / sizeof(colors[0]);
            txColor = txColor->NextSiblingElement("Color"), i++)
            if (txColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
                colors[i] = ((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24);
    TiXmlElement* txBkgColor = txVisual->FirstChildElement("BkgColor");
    if (txBkgColor)
        if (txBkgColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
            iBkgColor = ((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24);
    TiXmlElement* txBarColor = txVisual->FirstChildElement("BarColor");
    if (txBarColor)
        if (txBarColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
            iBarColor = ((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24);
    if (txVisual->QueryIntAttribute("RandomizeColor", &iAttrVal) == TIXML_SUCCESS)
        bRandomizeColor = (iAttrVal != 0);
    string sTempStr;
    txVisual->QueryStringAttribute("Background", &sTempStr);
    sBackground = Util::StringToWstring(sTempStr);
}

void AudioSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txAudio = txRoot->FirstChildElement("Audio");
    if (!txAudio) return;

    string sMIDIOutDevice;
    if (txAudio->QueryStringAttribute("MIDIOutDevice", &sMIDIOutDevice) == TIXML_SUCCESS)
    {
        sDesiredOut = Util::StringToWstring(sMIDIOutDevice);
        for (size_t i = 0; i < vMIDIOutDevices.size(); i++)
            if (vMIDIOutDevices[i] == sDesiredOut)
                iOutDevice = (int)i;
    }

    int iAttrVal;
    if (txAudio->QueryIntAttribute("KDMAPI", &iAttrVal) == TIXML_SUCCESS)
        bKDMAPI = (iAttrVal != 0);
}

void VideoSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVideo = txRoot->FirstChildElement("Video");
    if (!txVideo) return;

    int iAttrVal;
    if (txVideo->QueryIntAttribute("TickBased", &iAttrVal) == TIXML_SUCCESS)
        bTickBased = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("VisualizePitchBends", &iAttrVal) == TIXML_SUCCESS)
        bVisualizePitchBends = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("SameWidthNotes", &iAttrVal) == TIXML_SUCCESS)
        bSameWidth = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("MapVelocity", &iAttrVal) == TIXML_SUCCESS)
        bMapVel = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("ShowMarkers", &iAttrVal) == TIXML_SUCCESS)
        bShowMarkers = (iAttrVal != 0);
    int tmpMarkerEncoding;
    txVideo->QueryIntAttribute("MarkerEncoding", &tmpMarkerEncoding);
    eMarkerEncoding = static_cast<MarkerEncoding>(max(MarkerEncoding::CP1252, min(tmpMarkerEncoding, MarkerEncoding::UTF8)));
    if (txVideo->QueryIntAttribute("LimitFPS", &iAttrVal) == TIXML_SUCCESS)
        bLimitFPS = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("Debug", &iAttrVal) == TIXML_SUCCESS)
        bDebug = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("DisableUI", &iAttrVal) == TIXML_SUCCESS)
        bDisableUI = (iAttrVal != 0);
    if (txVideo->QueryIntAttribute("RemoveOverlaps", &iAttrVal) == TIXML_SUCCESS)
        bOR = (iAttrVal != 0);
}

void ControlsSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txControls = txRoot->FirstChildElement("Controls");
    if (!txControls) return;

    txControls->QueryDoubleAttribute("FwdBackSecs", &dFwdBackSecs);
    txControls->QueryDoubleAttribute("SpeedUpPct", &dSpeedUpPct);
    int iAttrVal;
    if (txControls->QueryIntAttribute("PhigrosMode", &iAttrVal) == TIXML_SUCCESS)
        bPhigros = (iAttrVal != 0);
    if (txControls->QueryIntAttribute("AlwaysShowControls", &iAttrVal) == TIXML_SUCCESS)
        bAlwaysShowControls = (iAttrVal != 0);
    string sTempStr;
    txControls->QueryStringAttribute("SplashMIDI", &sTempStr);
    sSplashMIDI = Util::StringToWstring(sTempStr);
    if (txControls->QueryIntAttribute("VelocityThreshold", &iAttrVal) == TIXML_SUCCESS)
        iVelocityThreshold = static_cast<uint8_t>(max(0, min(iAttrVal, 127)));
    if (txControls->QueryIntAttribute("DumpFrames", &iAttrVal) == TIXML_SUCCESS)
        bDumpFrames = (iAttrVal != 0);
}

void PlaybackSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txPlayback = txRoot->FirstChildElement("Playback");
    if (!txPlayback) return;

    int iAttrVal;
    if (txPlayback->QueryIntAttribute("Mute", &iAttrVal) == TIXML_SUCCESS)
        m_bMute = (iAttrVal != 0);
    txPlayback->QueryDoubleAttribute("PlaybackSpeed", &m_dSpeed);
    txPlayback->QueryDoubleAttribute("NoteSpeed", &m_dNSpeed);
    txPlayback->QueryDoubleAttribute("Volume", &m_dVolume);
}

void ViewSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txView = txRoot->FirstChildElement("View");
    if (!txView) return;

    int iAttrVal;
    if (txView->QueryIntAttribute("Controls", &iAttrVal) == TIXML_SUCCESS)
        m_bControls = (iAttrVal != 0);
    if (txView->QueryIntAttribute("Keyboard", &iAttrVal) == TIXML_SUCCESS)
        m_bKeyboard = (iAttrVal != 0);
    if (txView->QueryIntAttribute("OnTop", &iAttrVal) == TIXML_SUCCESS)
        m_bOnTop = (iAttrVal != 0);
    txView->QueryFloatAttribute("OffsetX", &m_fOffsetX);
    txView->QueryFloatAttribute("OffsetY", &m_fOffsetY);
    txView->QueryFloatAttribute("ZoomX", &m_fZoomX);
    txView->QueryIntAttribute("MainLeft", &m_iMainLeft);
    txView->QueryIntAttribute("MainTop", &m_iMainTop);
    txView->QueryIntAttribute("MainWidth", &m_iMainWidth);
    txView->QueryIntAttribute("MainHeight", &m_iMainHeight);
}

//-----------------------------------------------------------------------------
// SaveConfigValues
//-----------------------------------------------------------------------------

bool VisualSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVisual = new TiXmlElement("Visual");
    txRoot->LinkEndChild(txVisual);
    txVisual->SetAttribute("KeysShown", eKeysShown);
    txVisual->SetAttribute("FirstKey", iFirstKey);
    txVisual->SetAttribute("LastKey", iLastKey);

    TiXmlElement* txColors = new TiXmlElement("Colors");
    txVisual->LinkEndChild(txColors);
    for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++)
    {
        TiXmlElement* txColor = new TiXmlElement("Color");
        txColors->LinkEndChild(txColor);
        txColor->SetAttribute("R", (colors[i] >> 0) & 0xFF);
        txColor->SetAttribute("G", (colors[i] >> 8) & 0xFF);
        txColor->SetAttribute("B", (colors[i] >> 16) & 0xFF);
        txColor->SetAttribute("A", (colors[i] >> 24) & 0xFF);
    }

    TiXmlElement* txBkgColor = new TiXmlElement("BkgColor");
    txVisual->LinkEndChild(txBkgColor);
    txBkgColor->SetAttribute("R", (iBkgColor >> 0) & 0xFF);
    txBkgColor->SetAttribute("G", (iBkgColor >> 8) & 0xFF);
    txBkgColor->SetAttribute("B", (iBkgColor >> 16) & 0xFF);
    txBkgColor->SetAttribute("A", (iBkgColor >> 24) & 0xFF);

    TiXmlElement* txBarColor = new TiXmlElement("BarColor");
    txVisual->LinkEndChild(txBarColor);
    txBarColor->SetAttribute("R", (iBarColor >> 0) & 0xFF);
    txBarColor->SetAttribute("G", (iBarColor >> 8) & 0xFF);
    txBarColor->SetAttribute("B", (iBarColor >> 16) & 0xFF);
    txBarColor->SetAttribute("A", (iBarColor >> 24) & 0xFF);

    txVisual->SetAttribute("RandomizeColor", bRandomizeColor);
    txVisual->SetAttribute("Background", Util::WstringToString(sBackground));

    return true;
}

bool AudioSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txAudio = new TiXmlElement("Audio");
    txRoot->LinkEndChild(txAudio);

    if (this->sDesiredOut.length() > 0)
        txAudio->SetAttribute("MIDIOutDevice", Util::WstringToString(this->sDesiredOut));

    txAudio->SetAttribute("KDMAPI", bKDMAPI);
    return true;
}

bool VideoSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVideo = new TiXmlElement("Video");
    txRoot->LinkEndChild(txVideo);
    txVideo->SetAttribute("TickBased", bTickBased);
    txVideo->SetAttribute("VisualizePitchBends", bVisualizePitchBends);
    txVideo->SetAttribute("SameWidthNotes", bSameWidth);
    txVideo->SetAttribute("MapVelocity", bMapVel);
    txVideo->SetAttribute("ShowMarkers", bShowMarkers);
    txVideo->SetAttribute("MarkerEncoding", eMarkerEncoding);
    txVideo->SetAttribute("LimitFPS", bLimitFPS);
    txVideo->SetAttribute("Debug", bDebug);
    txVideo->SetAttribute("DisableUI", bDisableUI);
    txVideo->SetAttribute("RemoveOverlaps", bOR);
    return true;
}

bool ControlsSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txControls = new TiXmlElement("Controls");
    txRoot->LinkEndChild(txControls);
    txControls->SetDoubleAttribute("FwdBackSecs", dFwdBackSecs);
    txControls->SetDoubleAttribute("SpeedUpPct", dSpeedUpPct);
    txControls->SetAttribute("AlwaysShowControls", bAlwaysShowControls);
    txControls->SetAttribute("PhigrosMode", bPhigros);
    txControls->SetAttribute("SplashMIDI", Util::WstringToString(sSplashMIDI));
    txControls->SetAttribute("VelocityThreshold", iVelocityThreshold);
    txControls->SetAttribute("DumpFrames", bDumpFrames);
    return true;
}

bool PlaybackSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txPlayback = new TiXmlElement("Playback");
    txRoot->LinkEndChild(txPlayback);
    txPlayback->SetAttribute("Mute", m_bMute);
    txPlayback->SetDoubleAttribute("PlaybackSpeed", m_dSpeed);
    txPlayback->SetDoubleAttribute("NoteSpeed", m_dNSpeed);
    txPlayback->SetDoubleAttribute("Volume", m_dVolume);
    return true;
}

bool ViewSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txView = new TiXmlElement("View");
    txRoot->LinkEndChild(txView);
    txView->SetAttribute("Controls", m_bControls);
    txView->SetAttribute("Keyboard", m_bKeyboard);
    txView->SetAttribute("OnTop", m_bOnTop);
    txView->SetDoubleAttribute("OffsetX", m_fOffsetX);
    txView->SetDoubleAttribute("OffsetY", m_fOffsetY);
    txView->SetDoubleAttribute("ZoomX", m_fZoomX);
    txView->SetAttribute("MainLeft", m_iMainLeft);
    txView->SetAttribute("MainTop", m_iMainTop);
    txView->SetAttribute("MainWidth", m_iMainWidth);
    txView->SetAttribute("MainHeight", m_iMainHeight);
    return true;
}
