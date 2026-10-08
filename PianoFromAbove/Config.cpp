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
#include <Config.h>
#include <Misc.h>
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
    char sAppData[LONG_MAX_PATH];
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
    color_t R, G, B = 0, S = 80, V = 100;
    chan_t iColors = sizeof(colors) / sizeof(colors[0]);
    for (chan_t i = 10, count = 0; count < iColors; i = (i + 7) % iColors, count++)
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
    eMarkerEncoding = 437;
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
    win32_t iNumOutDevs = midiOutGetNumDevs();
    for (win32_t i = 0; i < iNumOutDevs; i++)
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
        eKeysShown = static_cast<KeysShown>(max(KeysShown::All, min(*reinterpret_cast<key_t*>(&iAttrVal), KeysShown::Custom)));
    if (txVisual->QueryIntAttribute("FirstKey", &iAttrVal) == TIXML_SUCCESS)
        iFirstKey = *reinterpret_cast<key_t*>(&iAttrVal);
    if (txVisual->QueryIntAttribute("LastKey", &iAttrVal) == TIXML_SUCCESS)
        iLastKey = *reinterpret_cast<key_t*>(&iAttrVal);

    //Colors
    int r, g, b, a = 0;
    chan_t i = 0;
    TiXmlElement* txColors = txVisual->FirstChildElement("Colors");
    if (txColors)
        for (TiXmlElement* txColor = txColors->FirstChildElement("Color");
            txColor && i < sizeof(colors) / sizeof(colors[0]);
            txColor = txColor->NextSiblingElement("Color"), i++)
            if (txColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
                txColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
                colors[i] = static_cast<color_t>(((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24));
    TiXmlElement* txBkgColor = txVisual->FirstChildElement("BkgColor");
    if (txBkgColor)
        if (txBkgColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
            txBkgColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
            iBkgColor = static_cast<color_t>(((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24));
    TiXmlElement* txBarColor = txVisual->FirstChildElement("BarColor");
    if (txBarColor)
        if (txBarColor->QueryIntAttribute("R", &r) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("G", &g) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("B", &b) == TIXML_SUCCESS &&
            txBarColor->QueryIntAttribute("A", &a) == TIXML_SUCCESS)
            iBarColor = static_cast<color_t>(((r & 0xFF) << 0) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a & 0xFF) << 24));
    if (txVisual->QueryBoolAttribute("RandomizeColor", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bRandomizeColor = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    string sTempStr;
    if (txVisual->QueryStringAttribute("Background", &sTempStr)) == TIXML_SUCCESS)
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
        for (win32_t i = 0; i < vMIDIOutDevices.size(); i++)
            if (vMIDIOutDevices[i] == sDesiredOut)
                iOutDevice = i;
    }

    int iAttrVal;
    if (txAudio->QueryBoolAttribute("KDMAPI", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bKDMAPI = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
}

void VideoSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVideo = txRoot->FirstChildElement("Video");
    if (!txVideo) return;

    int iAttrVal;
    if (txVideo->QueryBoolAttribute("TickBased", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bTickBased = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("VisualizePitchBends", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bVisualizePitchBends = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("SameWidthNotes", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bSameWidth = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("MapVelocity", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bMapVel = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("ShowMarkers", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bShowMarkers = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryIntAttribute("CodePage", &iAttrVal) == TIXML_SUCCESS)
        eMarkerEncoding = *reinterpret_cast<WORD*>(&iAttrVal);
    if (txVideo->QueryBoolAttribute("LimitFPS", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bLimitFPS = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("Debug", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bDebug = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("DisableUI", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bDisableUI = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txVideo->QueryBoolAttribute("RemoveOverlaps", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bOR = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
}

void ControlsSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txControls = txRoot->FirstChildElement("Controls");
    if (!txControls) return;

    int iAttrVal;
    if (txControls->QueryIntAttribute("FwdBackSecs", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<int*>(&dFwdBackSecs) = iAttrVal;
    if (txControls->QueryIntAttribute("FwdBackSecsL", &iAttrVal) == TIXML_SUCCESS)
        *(reinterpret_cast<int*>(&dFwdBackSecs) + 1) = iAttrVal;
    if (txControls->QueryIntAttribute("SpeedUpPct", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<int*>(&dSpeedUpPct) = iAttrVal;
    if (txControls->QueryIntAttribute("SpeedUpPctL", &iAttrVal) == TIXML_SUCCESS)
        *(reinterpret_cast<int*>(&dSpeedUpPct) + 1) = iAttrVal;
    if (txControls->QueryBoolAttribute("PhigrosMode", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bPhigros = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txControls->QueryBoolAttribute("AlwaysShowControls", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bAlwaysShowControls = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    string sTempStr;
    if (txControls->QueryStringAttribute("SplashMIDI", &sTempStr) == TIXML_SUCCESS)
        sSplashMIDI = Util::StringToWstring(sTempStr);
    if (txControls->QueryIntAttribute("VelocityThreshold", &iAttrVal) == TIXML_SUCCESS)
        iVelocityThreshold = *reinterpret_cast<key_t*>(&iAttrVal);
    if (txControls->QueryBoolAttribute("DumpFrames", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        bDumpFrames = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
}

void PlaybackSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txPlayback = txRoot->FirstChildElement("Playback");
    if (!txPlayback) return;

    int iAttrVal;
    if (txPlayback->QueryBoolAttribute("Mute", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        m_bMute = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txPlayback->QueryIntAttribute("PlaybackSpeed", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_dSpeed) = iAttrVal;
    if (txPlayback->QueryIntAttribute("PlaybackSpeedL", &iAttrVal) == TIXML_SUCCESS)
        *(reinterpret_cast<uint32_t*>(&m_dSpeed) + 1) = iAttrVal;
    if (txPlayback->QueryIntAttribute("NoteSpeed", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_dNSpeed) = iAttrVal;
    if (txPlayback->QueryIntAttribute("NoteSpeedL", &iAttrVal) == TIXML_SUCCESS)
        *(reinterpret_cast<uint32_t*>(&m_dNSpeed) + 1) = iAttrVal;
    if (txPlayback->QueryIntAttribute("Volume", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_dVolume) = iAttrVal;
    if (txPlayback->QueryIntAttribute("VolumeL", &iAttrVal) == TIXML_SUCCESS)
        *(reinterpret_cast<uint32_t*>(&m_dVolume) + 1) = iAttrVal;
}

void ViewSettings::LoadConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txView = txRoot->FirstChildElement("View");
    if (!txView) return;

    int iAttrVal;
    if (txView->QueryBoolAttribute("Controls", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        m_bControls = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txView->QueryBoolAttribute("Keyboard", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        m_bKeyboard = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txView->QueryBoolAttribute("OnTop", reinterpret_cast<bool*>(&iAttrVal)) == TIXML_SUCCESS)
        m_bOnTop = (*reinterpret_cast<bool*>(&iAttrVal) != 0);
    if (txView->QueryIntAttribute("OffsetX", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_fOffsetX) = iAttrVal;
    if (txView->QueryIntAttribute("OffsetY", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_fOffsetY) = iAttrVal;
    if (txView->QueryIntAttribute("ZoomX", &iAttrVal) == TIXML_SUCCESS)
        *reinterpret_cast<uint32_t*>(&m_fZoomX) = iAttrVal;
    if (txView->QueryIntAttribute("MainLeft", &iAttrVal) == TIXML_SUCCESS)
        m_iMainLeft = iAttrVal;
    if (txView->QueryIntAttribute("MainTop", &iAttrVal) == TIXML_SUCCESS)
        m_iMainTop = iAttrVal;
    if (txView->QueryIntAttribute("MainWidth", &iAttrVal) == TIXML_SUCCESS)
        m_iMainWidth = iAttrVal;
    if (txView->QueryIntAttribute("MainHeight", &iAttrVal) == TIXML_SUCCESS)
        m_iMainHeight = iAttrVal;
}

//-----------------------------------------------------------------------------
// SaveConfigValues
//-----------------------------------------------------------------------------

bool VisualSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVisual = new TiXmlElement("Visual");
    txRoot->LinkEndChild(txVisual);
    txVisual->SetAttribute("KeysShown", static_cast<int>(key_t(eKeysShown)));
    txVisual->SetAttribute("FirstKey", static_cast<int>(key_t(iFirstKey)));
    txVisual->SetAttribute("LastKey", static_cast<int>(key_t(iLastKey)));

    TiXmlElement* txColors = new TiXmlElement("Colors");
    txVisual->LinkEndChild(txColors);
    for (chan_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++)
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

    txVisual->SetAttribute("RandomizeColor", bRandomizeColor ? "yes" : "no");
    txVisual->SetAttribute("Background", Util::WstringToString(sBackground));

    return true;
}

bool AudioSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txAudio = new TiXmlElement("Audio");
    txRoot->LinkEndChild(txAudio);

    if (this->sDesiredOut.length() > 0)
        txAudio->SetAttribute("MIDIOutDevice", Util::WstringToString(this->sDesiredOut));

    txAudio->SetAttribute("KDMAPI", bKDMAPI ? "yes" : "no");
    return true;
}

bool VideoSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txVideo = new TiXmlElement("Video");
    txRoot->LinkEndChild(txVideo);
    txVideo->SetAttribute("TickBased", bTickBased ? "yes" : "no");
    txVideo->SetAttribute("VisualizePitchBends", bVisualizePitchBends ? "yes" : "no");
    txVideo->SetAttribute("SameWidthNotes", bSameWidth ? "yes" : "no");
    txVideo->SetAttribute("MapVelocity", bMapVel ? "yes" : "no");
    txVideo->SetAttribute("ShowMarkers", bShowMarkers ? "yes" : "no");
    txVideo->SetAttribute("MarkerEncoding", static_cast<int>(WORD(eMarkerEncoding)));
    txVideo->SetAttribute("LimitFPS", bLimitFPS ? "yes" : "no");
    txVideo->SetAttribute("Debug", bDebug ? "yes" : "no");
    txVideo->SetAttribute("DisableUI", bDisableUI ? "yes" : "no");
    txVideo->SetAttribute("RemoveOverlaps", bOR ? "yes" : "no");
    return true;
}

bool ControlsSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txControls = new TiXmlElement("Controls");
    txRoot->LinkEndChild(txControls);
    txControls->SetAttribute("FwdBackSecs", *reinterpret_cast<int*>(&dFwdBackSecs));
    txControls->SetAttribute("FwdBackSecsL", *(reinterpret_cast<int*>(&dFwdBackSecs) + 1));
    txControls->SetAttribute("SpeedUpPct", *reinterpret_cast<int*>(&dSpeedUpPct));
    txControls->SetAttribute("SpeedUpPctL", *(reinterpret_cast<int*>(&dSpeedUpPct) + 1));
    txControls->SetAttribute("AlwaysShowControls", bAlwaysShowControls ? "yes" : "no");
    txControls->SetAttribute("PhigrosMode", bPhigros ? "yes" : "no");
    txControls->SetAttribute("SplashMIDI", Util::WstringToString(sSplashMIDI));
    txControls->SetAttribute("VelocityThreshold", iVelocityThreshold ? "yes" : "no");
    txControls->SetAttribute("DumpFrames", bDumpFrames ? "yes" : "no");
    return true;
}

bool PlaybackSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txPlayback = new TiXmlElement("Playback");
    txRoot->LinkEndChild(txPlayback);
    txPlayback->SetAttribute("Mute", m_bMute ? "yes" : "no");
    txPlayback->SetAttribute("PlaybackSpeed", *reinterpret_cast<int*>(&m_dSpeed));
    txPlayback->SetAttribute("PlaybackSpeedL", *(reinterpret_cast<int*>(&m_dSpeed) + 1));
    txPlayback->SetAttribute("NoteSpeed", *reinterpret_cast<int*>(&m_dNSpeed));
    txPlayback->SetAttribute("NoteSpeedL", *(reinterpret_cast<int*>(&m_dNSpeed) + 1));
    txPlayback->SetAttribute("Volume", *reinterpret_cast<int*>(&m_dVolume));
    txPlayback->SetAttribute("VolumeL", *(reinterpret_cast<int*>(&m_dVolume) + 1));
    return true;
}

bool ViewSettings::SaveConfigValues(TiXmlElement* txRoot)
{
    TiXmlElement* txView = new TiXmlElement("View");
    txRoot->LinkEndChild(txView);
    txView->SetAttribute("Controls", m_bControls ? "yes" : "no");
    txView->SetAttribute("Keyboard", m_bKeyboard ? "yes" : "no");
    txView->SetAttribute("OnTop", m_bOnTop ? "yes" : "no");
    txView->SetAttribute("OffsetX", *reinterpret_cast<int*>(&m_fOffsetX));
    txView->SetAttribute("OffsetY", *reinterpret_cast<int*>(&m_fOffsetY));
    txView->SetAttribute("ZoomX", *reinterpret_cast<int*>(&m_fZoomX));
    txView->SetAttribute("MainLeft", m_iMainLeft);
    txView->SetAttribute("MainTop", m_iMainTop);
    txView->SetAttribute("MainWidth", m_iMainWidth);
    txView->SetAttribute("MainHeight", m_iMainHeight);
    return true;
}
