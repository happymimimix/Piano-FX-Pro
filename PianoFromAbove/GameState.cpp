/*************************************************************************************************
*
* File: GameState.cpp
*
* Description: Implements the game states and objects rendered into the graphics window
*              Contains the core game logic (IntroScreen, SplashScreen, MainScreen objects)
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <algorithm>
#include <tchar.h>
#include <dwmapi.h>
#include <d3d9types.h>
#include <numeric>
#include <lzma.h>
#include <resource.h>
#include <PackWrapper.hpp>
#include <Globals.h>
#include <GameState.h>
#include <Config.h>
#include <ConfigProcs.h>

const wstring GameState::Errors[] =
{
    L"Success.",
    L"Invalid pointer passed. It would be nice if you could submit feedback with a description of how this happened.",
    L"Out of memory.",
    L"Error calling DirectX. It would be nice if you could submit feedback with a description of how this happened."
};

GameState::GameError GameState::ChangeState(GameState* pNextState, GameState** pDestObj) {
    // Null NextState is valid. Signifies no change in state.
    if (!pNextState)
        return Success;
    if (!pDestObj)
        return BadPointer;

    // Get rid of the old one. Carry over new window/renderer if needed
    if (*pDestObj)
    {
        if (!pNextState->m_hWnd) pNextState->m_hWnd = (*pDestObj)->m_hWnd;
        if (!pNextState->m_pRenderer) pNextState->m_pRenderer = (*pDestObj)->m_pRenderer;
        delete* pDestObj;
    }
    *pDestObj = pNextState;
    GameError iResult = pNextState->Init();
    if (iResult)
    {
        *pDestObj = new IntroScreen(pNextState->m_hWnd, pNextState->m_pRenderer);
        delete pNextState;
        iResult = (*pDestObj)->Init();
        return iResult;
    }


    return Success;
}

//-----------------------------------------------------------------------------
// IntroScreen GameState object
//-----------------------------------------------------------------------------

GameState::GameError IntroScreen::MsgProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_COMMAND:
    {
        winword_t iId = LOWORD(wParam);
        switch (iId)
        {
        case ID_CHANGESTATE:
            m_pNextState = reinterpret_cast<GameState*>(lParam);
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            if (FAILED(m_pRenderer->ResetDevice())) return DirectXError;
            return Success;
        }
        break;
    }
    }

    return Success;
}

GameState::GameError IntroScreen::Init() {
    return Success;
}

GameState::GameError IntroScreen::Logic() {
    return Success;
}

GameState::GameError IntroScreen::Render() {
    if (FAILED(m_pRenderer->ResetDeviceIfNeeded())) return DirectXError;

    // Clear the backbuffer to a blue color
    color_t iColor = Config::GetConfig().GetVisualSettings().iBkgColor;
    color_t R = (iColor >> 0) & 0xFF;
    color_t G = (iColor >> 8) & 0xFF;
    color_t B = (iColor >> 16) & 0xFF;
    if (FAILED(m_pRenderer->ClearAndBeginScene(D3DCOLOR_XRGB(R, G, B)))) return DirectXError;
    m_pRenderer->AddText(StatisticsText1 L" v" VersionString, 1 << 5, 16, 6, 0xFF404040);
    m_pRenderer->AddText(StatisticsText1 L" v" VersionString, 1 << 5, 14, 4, 0xFFFFFFFF);

    // Present the backbuffer contents to the display
    if (FAILED(m_pRenderer->EndScene())) return DirectXError;
    if (FAILED(m_pRenderer->Present())) return DirectXError;

    return Success;
}

//-----------------------------------------------------------------------------
// SplashScreen GameState object
//-----------------------------------------------------------------------------

void InsertSorted(vector<idx_t>& vec, idx_t value) {
    auto it = lower_bound(vec.begin(), vec.end(), value);
    vec.insert(it, value);
}

idx_t LocateElement(const vector<idx_t>& vec, idx_t value) {
    auto it = lower_bound(vec.begin(), vec.end(), value);
    if (it != vec.end() && *it == value) return it - vec.begin();
    return IDX_MAX;
}

SplashScreen::SplashScreen(HWND hWnd, Renderer11* pRenderer, bool enableSplash) : GameState(hWnd, pRenderer) {
    if (enableSplash) {
        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_SPLASHMIDI), TEXT("MIDI"));
        HGLOBAL hRes = LoadResource(NULL, hResInfo);
        win32_t iSize = SizeofResource(NULL, hResInfo);
        unsigned char* pData = (unsigned char*)LockResource(hRes);

        Config& config = Config::GetConfig();
        const ControlsSettings& cControls = config.GetControlsSettings();

        // Parse MIDI
        if (!cControls.sSplashMIDI.empty()) {
            // this is REALLY BAD, but i can't figure out how to make it move ownership of the memory pool vector instead of copying
            m_MIDI.~MIDI();
            new (&m_MIDI) MIDI(cControls.sSplashMIDI);
            if (!m_MIDI.IsValid()) {
                MessageBox(hWnd, L"The custom splash MIDI failed to load. Please choose a different MIDI.", L"", MB_ICONWARNING);
                m_MIDI = MIDI();
                goto SplashFailed;
            }
        }
        else {
        SplashFailed:
            constexpr uint8_t lzma_magic[] = { 0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00 };
            while (iSize >= LZMA_STREAM_HEADER_SIZE * 2 && !memcmp(pData, lzma_magic, sizeof(lzma_magic))) {
                unsigned char* compressed = pData;
                uint64_t decompressed_size = 0;
                lzma_stream strm = LZMA_STREAM_INIT;
                lzma_stream_flags stream_flags;
                lzma_index* index = nullptr;
                auto pos = (int64_t)iSize;
                lzma_ret ret;
                do {
                    pos -= LZMA_STREAM_HEADER_SIZE;
                    uint64_t footer_pos;
                    while (true) {
                        footer_pos = pos;

                        int i = 2;
                        if (*(uint32_t*)&compressed[footer_pos + 8] != 0)
                            break;

                        do {
                            pos -= 4;
                            --i;
                        } while (i >= 0 && *(uint32_t*)&compressed[footer_pos + i * 4] == 0);
                    }
                    ret = lzma_stream_footer_decode(&stream_flags, &compressed[footer_pos]);
                    pos -= stream_flags.backward_size;
                    lzma_index_decoder(&strm, &index, UINT64_MAX);
                    strm.avail_in = stream_flags.backward_size;
                    strm.next_in = &compressed[pos];
                    pos += stream_flags.backward_size;
                    ret = lzma_code(&strm, LZMA_RUN);
                    pos -= stream_flags.backward_size + LZMA_STREAM_HEADER_SIZE;
                    pos -= lzma_index_total_size(index);
                    decompressed_size += lzma_index_uncompressed_size(index);
                } while (pos > 0);
                pData = new unsigned char[decompressed_size];
                uint8_t* write_ptr = pData;
                lzma_end(&strm);
                strm = LZMA_STREAM_INIT;
                lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED);
                strm.next_in = compressed;
                strm.avail_in = iSize;
                bool done = false;
                lzma_action action = LZMA_RUN;
                while (!done) {
                    if (strm.avail_in == 0)
                        action = LZMA_FINISH;
                    lzma_ret ret = lzma_code(&strm, action);
                    if (strm.avail_out == 0) {
                        auto remaining = min(decompressed_size - (write_ptr - pData), 1 << 20);
                        strm.next_out = write_ptr;
                        strm.avail_out = remaining;
                        write_ptr += remaining;
                    }
                    switch (ret) {
                    case LZMA_STREAM_END:
                        done = true;
                        break;
                    case LZMA_OK:
                        break;
                    }
                }
                iSize = decompressed_size;
            }
            m_MIDI.ParseMIDI(pData, iSize);
            delete[] pData;
        }
        vector< MIDIEvent* > vEvents;
        vEvents.reserve(m_MIDI.GetInfo().iEventCount);
        m_MIDI.ConnectNotes(); // Order's important here
        bool IsPostProcessOK = m_MIDI.PostProcess(m_vEvents);
        if (!IsPostProcessOK) {
            MessageBoxW(hWnd, Errors[GameError::OutOfMemory].c_str(), L"Error", MB_OK);
            return;
        }

        // Allocate
        m_vTrackSettings.clear();
        m_vTrackSettings.resize(min(m_MIDI.GetInfo().iNumTracks, MaxTrackColors));
        m_vState.clear();
        m_vState.reserve(1 << 10);
    }
    InitState();
    UpdateNotePos = true;
}

void SplashScreen::InitNotes(const vector<MIDIEvent*>& vEvents) {
    //Get only the channel events
    m_vEvents.reserve(vEvents.size());
    for (vector<MIDIEvent*>::const_iterator it = vEvents.begin(); it != vEvents.end(); ++it)
        if ((*it)->GetEventType() == MIDIEvent::ChannelEvent)
            m_vEvents.push_back(reinterpret_cast<MIDIChannelEvent*>(*it));
}

void SplashScreen::InitState() {
    static Config& config = Config::GetConfig();
    static const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    static const VisualSettings& cVisual = config.GetVisualSettings();
    static const AudioSettings& cAudio = config.GetAudioSettings();

    m_iStartPos = 0;
    m_iEndPos = -1;
    m_llStartTime = m_MIDI.GetInfo().llFirstNote - 1000000;
    m_bMute = cPlayback.GetMute();

    SetChannelSettings(vector<bool>(), vector<bool>(), vector<color_t>(cVisual.colors, cVisual.colors + sizeof(cVisual.colors) / sizeof(cVisual.colors[0])));

    if (cAudio.bKDMAPI) {
        m_OutDevice.OpenKDMAPI();
    }
    else {
        if (cAudio.iOutDevice >= 0)
            m_OutDevice.Open(cAudio.iOutDevice);
    }
    m_OutDevice.SetVolume(1.0);

    m_Timer.Init(false);
}

GameState::GameError SplashScreen::Init() {
    UpdateNotePos = true;
    return Success;
}

void SplashScreen::ColorChannel(track_t iTrack, chan_t iChannel, color_t iColor, bool bRandom) {
    if (bRandom)
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor();
    else
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor(iColor);
}

void SplashScreen::SetChannelSettings(const vector<bool>&, const vector<bool>&, const vector<color_t>& vColor) {
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    const vector<MIDITrack*>& vTracks = m_MIDI.GetTracks();

    static Config& config = Config::GetConfig();
    static const VisualSettings& cVisual = config.GetVisualSettings();

#ifdef LIMIT_COLORS
    static bool IsEmpty[MaxTrackColors][16];
    memset(IsEmpty, true, sizeof(IsEmpty));
    for (track_t i = 0; i < mInfo.iNumTracks; i++) {
        const MIDITrack::MIDITrackInfo& info = vTracks[i]->GetInfo();
        for (chan_t j = 0; j < MaxChannelColors; j++) {
            if (info.aNoteCount[j] > 0) {
                IsEmpty[i % MaxTrackColors][j] = false;
            }
        }
    }
#endif

    idx_t iPos = 0;
    for (track_t i = 0; i < min(mInfo.iNumTracks, MaxTrackColors); i++)
    {
        const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
        for (chan_t j = 0; j < MaxChannelColors; j++) {
#ifdef LIMIT_COLORS
            if (!IsEmpty[i][j])
#else
            if (mTrackInfo.aNoteCount[j] > 0)
#endif
            {
                if (cVisual.bRandomizeColor) {
                    ColorChannel(i, j, 0, true);
                }
                else {
                    ColorChannel(i, j, vColor[iPos % vColor.size()]);
                }
                iPos++;
            }
        }
    }
}

GameState::GameError SplashScreen::MsgProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam) {
    static Config& config = Config::GetConfig();
    static const AudioSettings& cAudio = config.GetAudioSettings();

    switch (msg)
    {
    case WM_COMMAND:
    {
        winword_t iId = LOWORD(wParam);
        switch (iId)
        {
        case ID_CHANGESTATE:
            m_pNextState = reinterpret_cast<GameState*>(lParam);
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            if (FAILED(m_pRenderer->ResetDevice())) return DirectXError;
            return Success;
        }
        break;
    }
    case WM_DEVICECHANGE:
        if (cAudio.bKDMAPI) {
            m_OutDevice.OpenKDMAPI();
        }
        else {
            m_OutDevice.Open(cAudio.iOutDevice);
        }
        break;
    }

    return Success;
}

GameState::GameError SplashScreen::Logic() {
    static Config& config = Config::GetConfig();
    static PlaybackSettings& cPlayback = config.GetPlaybackSettings();

    // Set the state
    m_bMute = cPlayback.GetMute();
    m_dVolume = cPlayback.GetVolume();

    // Time stuff
    mms_t llElapsed = m_Timer.GetMicroSecs();
    m_Timer.Start();

    // Figure out start and end times for display
    m_llStartTime = m_llStartTime + llElapsed;
    if (m_llStartTime > m_MIDI.GetInfo().llTotalMicroSecs + 500000) {
        m_llStartTime = m_MIDI.GetInfo().llFirstNote - 1000000;
        m_iStartPos = 0;
        m_iEndPos = -1;
    }
    mms_t llEndTime = m_llStartTime + TimeSpan;

    // Needs start time to be set. For creating textparticles.
    RenderGlobals();

    // Advance end position
    idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
    while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() < llEndTime) m_iEndPos++;

    // Advance start position
    while (m_iStartPos < iEventCount && m_vEvents[m_iStartPos]->GetAbsMicroSec() <= m_llStartTime)
    {
        const MIDIChannelEvent* pEvent = m_vEvents[m_iStartPos];
        if (IsNotNote(pEvent->GetChannelEventType())) {
            m_OutDevice.PlayEvent(pEvent->GetEventCode(), pEvent->GetParam1(), pEvent->GetParam2());
        }
        else if (!m_bMute && pEvent->HasSister()) {
            if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2())) {
                m_OutDevice.PlayEvent(pEvent->GetEventCode(), pEvent->GetParam1(), NoteVelFormula(pEvent->GetParam2()));
            }
            else {
                m_OutDevice.PlayEvent(off2on(pEvent->GetEventCode()), pEvent->GetParam1(), 0x00);
            }
        }
        if (IsNote(pEvent->GetChannelEventType()) && pEvent->HasSister()) {
            UpdateState(static_cast<idx_t>(m_iStartPos), IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2()) ? IDX_MAX : pEvent->GetSisterIdx());
        }
        m_iStartPos++;
    }

    return Success;
}

void SplashScreen::UpdateState(idx_t idx, idx_t sister_idx) {
    if (sister_idx == IDX_MAX) {
        // Note-on
        m_vState.push_back(idx);
    }
    else {
        // Note-off
        idx_t pos = LocateElement(m_vState, sister_idx);
        if (pos != IDX_MAX) {
            m_vState.erase(m_vState.begin() + pos);
        }
        else {
            MessageBoxW(NULL, Errors[GameError::BadPointer].c_str(), L"Error", MB_OK);
        }
    }
}

const float SplashScreen::SharpRatio = 0.65f;

GameState::GameError SplashScreen::Render() {
    if (FAILED(m_pRenderer->ResetDeviceIfNeeded())) return DirectXError;

    // Clear the backbuffer to a blue color
    color_t iColor = Config::GetConfig().GetVisualSettings().iBkgColor;
    int R = (iColor >> 0) & 0xFF;
    int G = (iColor >> 8) & 0xFF;
    int B = (iColor >> 16) & 0xFF;
    if (FAILED(m_pRenderer->ClearAndBeginScene(D3DCOLOR_XRGB(R, G, B)))) return DirectXError;
    RenderNotes();
    m_pRenderer->AddText(StatisticsText1 L" v" VersionString, 1 << 5, 16, 6, 0xFF404040);
    m_pRenderer->AddText(StatisticsText1 L" v" VersionString, 1 << 5, 14, 4, 0xFFFFFFFF);

    // Present the backbuffer contents to the display
    if (FAILED(m_pRenderer->EndScene())) return DirectXError;
    if (FAILED(m_pRenderer->Present())) return DirectXError;

    return Success;
}

void SplashScreen::RenderGlobals() {
    // Midi info
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    m_iStartNote = mInfo.iMinNote;
    m_iEndNote = mInfo.iMaxNote;

    // Screen info
    m_fNotesX = 0.0f;
    m_fNotesCX = static_cast<float>(m_pRenderer->GetBufferWidth());
    m_fNotesY = 0.0f;
    m_fNotesCY = static_cast<float>(m_pRenderer->GetBufferHeight());

    // Keys info
    m_iAllWhiteKeys = MIDI::WhiteCount(m_iStartNote, m_iEndNote + 1);
    float fBuffer = (MIDI::IsSharp(m_iStartNote) ? SharpRatio / 2.0f : 0.0f) +
        (MIDI::IsSharp(m_iEndNote) ? SharpRatio / 2.0f : 0.0f);
    m_fWhiteCX = m_fNotesCX / (m_iAllWhiteKeys + fBuffer);

    // Round down start time. This is only used for rendering purposes
    mms_t llMicroSecsPP = static_cast<mms_t>(TimeSpan / m_fNotesCY + 0.5f);
    m_llRndStartTime = m_llStartTime - (m_llStartTime < 0 ? llMicroSecsPP : 0);
    m_llRndStartTime = (m_llRndStartTime / llMicroSecsPP) * llMicroSecsPP;

    if (UpdateNotePos) GenNoteXTable();
}

void SplashScreen::RenderNotes() {
    // Do we have any notes to render?
    if (m_iEndPos < 0 || m_iStartPos >= m_vEvents.size())
        return;

    // White held notes
    for (idx_t idx : m_vState) {
        if (!MIDI::IsSharp(m_vEvents[idx]->GetParam1())) {
            RenderNote(m_vEvents[idx]);
        }
    }
    // White falling notes
    for (sidx_t i = m_iStartPos; i <= m_iEndPos; i++) {
        MIDIChannelEvent* pEvent = m_vEvents[i];
        if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2()) && pEvent->HasSister() &&
            !MIDI::IsSharp(pEvent->GetParam1()) &&
            m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
            RenderNote(pEvent);
        }
    }
    // Sharp held notes
    for (idx_t idx : m_vState) {
        if (MIDI::IsSharp(m_vEvents[idx]->GetParam1())) {
            RenderNote(m_vEvents[idx]);
        }
    }
    // Sharp falling notes
    for (sidx_t i = m_iStartPos; i <= m_iEndPos; i++) {
        MIDIChannelEvent* pEvent = m_vEvents[i];
        if (IsOn(pEvent->GetChannelEventType(), pEvent->GetParam2()) && pEvent->HasSister() &&
            MIDI::IsSharp(pEvent->GetParam1()) &&
            m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
            RenderNote(pEvent);
        }
    }
}

void SplashScreen::RenderNote(MIDIChannelEvent* pNote) {
    key_t iNote = pNote->GetParam1();
    track_t iTrack = pNote->GetTrack() % MaxTrackColors;
    chan_t iChannel = pNote->GetChannel();
    mms_t llNoteStart = pNote->GetAbsMicroSec();
    mms_t llNoteEnd = llNoteStart + pNote->GetLength();

    ChannelSettings& csTrack = m_vTrackSettings[iTrack].aChannels[iChannel];
    if (m_vTrackSettings[iTrack].aChannels[iChannel].bHidden) return;

    // Compute true positions
    float x = GetNoteX(iNote);
    float y = m_fNotesY + m_fNotesCY * (1.0f - static_cast<float>(llNoteStart - m_llRndStartTime) / TimeSpan);
    float cx = MIDI::IsSharp(iNote) ? m_fWhiteCX * SharpRatio : m_fWhiteCX;
    float cy = m_fNotesCY * (static_cast<float>(llNoteEnd - llNoteStart) / TimeSpan);
    float fDeflate = m_fWhiteCX * 0.15f / 2.0f;

    // Rounding to make everything consistent
    cy = floor(cy + 0.5f); // constant cy across rendering
    y = floor(y + 0.5f);
    fDeflate = floor(fDeflate + 0.5f);
    fDeflate = max(min(fDeflate, 3.0f), 1.0f);

    // Clipping :/
    float fMinY = m_fNotesY - 5.0f;
    float fMaxY = m_fNotesY + m_fNotesCY + 5.0f;
    if (y > fMaxY)
    {
        cy -= (y - fMaxY);
        y = fMaxY;
    }
    if (y - cy < fMinY)
    {
        cy -= (fMinY - (y - cy));
        y = fMinY + cy;
    }

    // Visualize!
    color_t iAlpha = min(max(static_cast<mtk_t>(0xFF * (m_fNotesCY - y) / m_fNotesCY), 0x00), 0xFF);
    iAlpha <<= 24;
    m_pRenderer->DrawRect(x, y - cy, cx, cy, csTrack.iVeryDarkRGB & 0x00FFFFFF | iAlpha);
    m_pRenderer->DrawRect(x + fDeflate, y - cy + fDeflate,
        cx - fDeflate * 2.0f, cy - fDeflate * 2.0f,
        csTrack.iPrimaryRGB & 0x00FFFFFF | iAlpha, csTrack.iDarkRGB & 0x00FFFFFF | iAlpha, csTrack.iDarkRGB & 0x00FFFFFF | iAlpha, csTrack.iPrimaryRGB & 0x00FFFFFF | iAlpha);
}

void SplashScreen::GenNoteXTable() {
    UpdateNotePos = false;
    for (key_t i = m_iStartNote; i <= m_iEndNote; i++) {
        key_t iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
        float fStartX = (MIDI::IsSharp(m_iStartNote) - MIDI::IsSharp(i)) * SharpRatio / 2.0f;
        if (MIDI::IsSharp(i))
        {
            MIDI::Note eNote = MIDI::NoteVal(i);
            if (eNote == MIDI::CS || eNote == MIDI::FS) fStartX -= SharpRatio / 5.0f;
            else if (eNote == MIDI::AS || eNote == MIDI::DS) fStartX += SharpRatio / 5.0f;
        }
        notex_table[i] = m_fNotesX + m_fWhiteCX * (iWhiteKeys + fStartX);
    }
}

float SplashScreen::GetNoteX(key_t iNote) {
    return notex_table[iNote];
}

//-----------------------------------------------------------------------------
// MainScreen GameState object
//-----------------------------------------------------------------------------

wstring GetExePath(void) {
    wchar_t szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameW(NULL, szFilePath, MAX_PATH);
    (wcsrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

MainScreen::MainScreen(wstring sMIDIFile, HWND hWnd, Renderer11* pRenderer) : GameState(hWnd, pRenderer), m_MIDI(sMIDIFile) {
    // Finish off midi processing
    if (!m_MIDI.IsValid()) return;
    m_MIDI.ConnectNotes(); // Order's important here
    m_vEvents.reserve(m_MIDI.GetInfo().iEventCount);

    // Allocate
    m_vTrackSettings.clear();
    m_vTrackSettings.resize(min(m_MIDI.GetInfo().iNumTracks, MaxTrackColors));
    m_vState.clear();
    m_vState.reserve(1 << 10);

    bool IsPostProcessOK = m_MIDI.PostProcess(m_vEvents, &m_vMetaEvents, &m_vTempo, &m_vSignature, &m_vMarkers, &m_vColors);
    if (!IsPostProcessOK) {
        MessageBoxW(hWnd, Errors[GameError::OutOfMemory].c_str(), L"Error", MB_OK);
        return;
    }

    g_LoadingProgress.stage = MIDILoadingProgress::Stage::NCTable;
    g_LoadingProgress.progress = 0;
    g_LoadingProgress.max = m_vEvents.size();
    if (m_vNCTable) {
        delete[] m_vNCTable;
        m_vNCTable = nullptr;
    }
    mms_t iMaxMS = m_MIDI.GetInfo().llTotalMicroSecs / MS;
    m_vNCTable = new idx_t[iMaxMS + static_cast<idx_t>(1LL)]();
    idx_t iNC = 0;
    mms_t iLastMS = -1;
    for (auto pEvent : m_vEvents) {
        if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2())) {
            iNC++;
            if (m_vNCTable) {
                mms_t iThisMS = pEvent->GetAbsMicroSec() / MS;
                if (iLastMS >= 0 && iThisMS > iLastMS) {
                    fill(m_vNCTable + iLastMS + 1LL, m_vNCTable + iThisMS, m_vNCTable[min(max(iLastMS, 0LL), iMaxMS)]);
                }
                m_vNCTable[min(max(iThisMS, 0LL), iMaxMS)] = iNC;
                iLastMS = iThisMS;
            }
        }
        g_LoadingProgress.progress++;
    }
    fill(m_vNCTable + iLastMS + 1LL, m_vNCTable + iMaxMS + 1LL, m_vNCTable[iLastMS]);

    // Initialize
    InitColors();
    InitState();
    UpdateNotePos = true;

    g_LoadingProgress.stage = MIDILoadingProgress::Stage::Done;
}

// Display colors
void MainScreen::InitColors() {
    static Config& config = Config::GetConfig();
    static const VisualSettings& cVisual = config.GetVisualSettings();

    m_csBackground.SetColor(cVisual.iBkgColor, 0.7f, 1.3f);
    m_csKBBackground.SetColor(0x00999999 | (cVisual.iBkgColor & 0xFF000000), 0.4f, 0.0f);
    m_csKBRed.SetColor(cVisual.iBarColor, 0.5f);
    m_csKBWhite.SetColor(0x00FFFFFF | (cVisual.iBarColor & 0xFF000000), 0.8f, 0.6f);
    m_csKBSharp.SetColor(0x00404040 | (cVisual.iBarColor & 0xFF000000), 0.5f, 0.0f);
}

// Init state vars. Only those which validate the date.
void MainScreen::InitState() {
    static Config& config = Config::GetConfig();
    static const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    static const ViewSettings& cView = config.GetViewSettings();
    static const ControlsSettings& cControls = config.GetControlsSettings();

    m_iStartPos = 0;
    m_iPrevStartPos = 0;
    m_iEndPos = -1;
    m_llStartTime = GetMinTime();
    m_bTrackPos = m_bTrackZoom = false;
    m_fTempZoomX = 1.0f;
    m_fTempOffsetX = m_fTempOffsetY = 0.0f;
    m_dFPS = 0.0;
    m_iFPSCount = 0;
    m_llFPSTime = 0;
    m_llPrevTime = m_llStartTime;

    m_fZoomX = cView.GetZoomX();
    m_fOffsetX = cView.GetOffsetX();
    m_fOffsetY = cView.GetOffsetY();
    m_bPaused = true;
    m_bMute = cPlayback.GetMute();
    m_dSpeed = cPlayback.GetSpeed();
    m_dNSpeed = cPlayback.GetNSpeed();
    m_llTimeSpan = static_cast<mms_t>(3.0 * abs(m_dNSpeed) * 1000000);
    IsLastFrameReversed = m_dSpeed < 0;
    IsReversedStateInitialized = false;
    CE_Connected = false;
    CE_DoNextTick = false;
    CE_Responded = false;

    resolution = m_MIDI.GetInfo().iDivision;
    TotalTime = m_MIDI.GetInfo().llTotalMicroSecs + 250000;
    TotalNC = m_MIDI.GetInfo().iNoteCount;
    if (TotalNC < 100000) { strcpy(Difficulty, "EZ Lv.1"); }
    else if (TotalNC < 200000) { strcpy(Difficulty, "EZ Lv.2"); }
    else if (TotalNC < 400000) { strcpy(Difficulty, "EZ Lv.3"); }
    else if (TotalNC < 600000) { strcpy(Difficulty, "EZ Lv.4"); }
    else if (TotalNC < 800000) { strcpy(Difficulty, "EZ Lv.5"); }
    else if (TotalNC < 1000000) { strcpy(Difficulty, "HD Lv.6"); }
    else if (TotalNC < 2000000) { strcpy(Difficulty, "HD Lv.7"); }
    else if (TotalNC < 4000000) { strcpy(Difficulty, "HD Lv.8"); }
    else if (TotalNC < 6000000) { strcpy(Difficulty, "HD Lv.9"); }
    else if (TotalNC < 8000000) { strcpy(Difficulty, "HD Lv.10"); }
    else if (TotalNC < 10000000) { strcpy(Difficulty, "IN Lv.11"); }
    else if (TotalNC < 20000000) { strcpy(Difficulty, "IN Lv.12"); }
    else if (TotalNC < 40000000) { strcpy(Difficulty, "IN Lv.13"); }
    else if (TotalNC < 60000000) { strcpy(Difficulty, "IN Lv.14"); }
    else if (TotalNC < 80000000) { strcpy(Difficulty, "IN Lv.15"); }
    else if (TotalNC < 100000000) { strcpy(Difficulty, "AT Lv.16"); }
    else if (TotalNC < 200000000) { strcpy(Difficulty, "AT Lv.17"); }
    else if (TotalNC < 400000000) { strcpy(Difficulty, "AT Lv.18"); }
    else { strcpy(Difficulty, "SP Lv.?"); }

    m_bDumpFrames = cControls.bDumpFrames;
    if (m_bDumpFrames) {
        RECT rect = {};
        GetWindowRect(g_hWndGfx, &rect);
        win32_t width = rect.right - rect.left;
        win32_t height = rect.bottom - rect.top;

        //Running ffmpeg
        wchar_t buf[1 << 10] = {};
        swprintf(buf, sizeof(buf), L"cd /d \"%s\" && md \"%s\\PianoFX_Framedump\" & start cmd /k ffmpeg -r 60 -f rawvideo -s %dx%d -pix_fmt bgra -i async:\\\\.\\pipe\\PFXdump -c:v h264 -qp 19 -pix_fmt yuv420p \"%s\\PianoFX_Framedump\\Output.mp4\"", GetExePath().c_str(), GetExePath().c_str(), width, height, GetExePath().c_str());
        m_hVideoPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\PFXdump"), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, static_cast<DWORD>(width * height * 4 * 120), 0, 0, nullptr);
        _wsystem(buf);
        ConnectNamedPipe(m_hVideoPipe, NULL);
    }
    InitKeyColor();
    AdvanceIterators(m_llStartTime, true);
}

// Called immediately before changing to this state
GameState::GameError MainScreen::Init() {
    UpdateNotePos = true;
    static Config& config = Config::GetConfig();
    static const AudioSettings& cAudio = config.GetAudioSettings();
    if (cAudio.bKDMAPI) {
        m_OutDevice.OpenKDMAPI();
    }
    else {
        if (cAudio.iOutDevice >= 0)
            m_OutDevice.Open(cAudio.iOutDevice);
    }

    m_OutDevice.Reset();
    m_OutDevice.SetVolume(1.0);
    m_Timer.Init(config.m_bManualTimer || m_bDumpFrames);
    if (m_bDumpFrames) {
        m_Timer.SetFrameRate(60);
    }
    else if (m_Timer.m_bManualTimer) {
        // get the screen's refresh rate
        DWM_TIMING_INFO timing_info;
        memset(&timing_info, 0, sizeof(timing_info));
        timing_info.cbSize = sizeof(timing_info);
        if (FAILED(DwmGetCompositionTimingInfo(NULL, &timing_info))) {
            MessageBox(NULL, L"Failed to get the screen refresh rate! Defaulting to 60hz...", L"", MB_ICONERROR);
            m_Timer.SetFrameRate(60);
        }
        else {
            m_Timer.SetFrameRate(ceil(static_cast<float>(timing_info.rateRefresh.uiNumerator) / static_cast<float>(timing_info.rateRefresh.uiDenominator)));
        }

    }

    return Success;
}

void MainScreen::ColorChannel(track_t iTrack, chan_t iChannel, color_t iColor, bool bRandom) {
    if (bRandom)
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor();
    else
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor(iColor);
}

// Sets to a random color
void ChannelSettings::SetColor() {
    SetColor(Util::RandColor(), 0.6, 0.2);
}

// Flips around windows format (ABGR) -> direct x format (ARGB)
void ChannelSettings::SetColor(color_t iColor, double dDark, double dVeryDark) {
    color_t R = (iColor >> 0) & 0xFF, dR, vdR;
    color_t G = (iColor >> 8) & 0xFF, dG, vdG;
    color_t B = (iColor >> 16) & 0xFF, dB, vdB;
    color_t A = (iColor >> 24) & 0xFF;

    color_t H, S, V;
    Util::RGBtoHSV(R, G, B, H, S, V);
    Util::HSVtoRGB(H, S, min(100, static_cast<color_t>(V * dDark)), dR, dG, dB);
    Util::HSVtoRGB(H, S, min(100, static_cast<color_t>(V * dVeryDark)), vdR, vdG, vdB);

    this->iOrigBGR = iColor;
    this->iPrimaryRGB = (A << 24) | (R << 16) | (G << 8) | (B << 0);
    this->iDarkRGB = (A << 24) | (dR << 16) | (dG << 8) | (dB << 0);
    this->iVeryDarkRGB = (A << 24) | (vdR << 16) | (vdG << 8) | (vdB << 0);
}

void MainScreen::SetChannelSettings(const vector<bool>& vMuted, const vector<bool>& vHidden, const vector<color_t>& vColor) {
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    const vector<MIDITrack*>& vTracks = m_MIDI.GetTracks();

    bool bMuted = vMuted.size() > 0;
    bool bHidden = vHidden.size() > 0;
    bool bColor = vColor.size() > 0;

#ifdef LIMIT_COLORS
    static bool IsEmpty[MaxTrackColors][16];
    memset(IsEmpty, true, sizeof(IsEmpty));
    for (track_t i = 0; i < mInfo.iNumTracks; i++) {
        const MIDITrack::MIDITrackInfo& info = vTracks[i]->GetInfo();
        for (chan_t j = 0; j < MaxChannelColors; j++) {
            if (info.aNoteCount[j] > 0) {
                IsEmpty[i % MaxTrackColors][j] = false;
            }
        }
    }
#endif

    idx_t iPos = 0;
    for (track_t i = 0; i < min(mInfo.iNumTracks, MaxTrackColors); i++) {
        const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
        for (chan_t j = 0; j < MaxChannelColors; j++) {
#ifdef LIMIT_COLORS
            if (!IsEmpty[i][j]) {
#else
            if (mTrackInfo.aNoteCount[j] > 0) {
#endif
                MuteChannel(i, j, bMuted ? vMuted[min(iPos, vMuted.size() - 1)] : false);
                HideChannel(i, j, bHidden ? vHidden[min(iPos, vHidden.size() - 1)] : false);
                if (bColor) {
                    ColorChannel(i, j, vColor[iPos % vColor.size()]);
                }
                else {
                    ColorChannel(i, j, 0, true);
                }
                iPos++;
            }
            }
        }
    }

GameState::GameError MainScreen::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Not thread safe, blah
    static Config& config = Config::GetConfig();
    static PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    static ViewSettings& cView = config.GetViewSettings();
    static const ControlsSettings& cControls = config.GetControlsSettings();
    static const AudioSettings& cAudio = config.GetAudioSettings();

    switch (msg)
    {
        // Commands that were passed straight through because they're more involved than setting a state variable
    case WM_COMMAND:
    {
        winword_t iId = LOWORD(wParam);
        switch (iId)
        {
        case ID_CHANGESTATE:
            m_pNextState = reinterpret_cast<GameState*>(lParam);
            return Success;
        case ID_PLAY_STOP:
            JumpTo(GetMinTime());
            cPlayback.SetStopped(true);
            return Success;
        case ID_PLAY_SKIPFWD:
            JumpTo(static_cast<mms_t>(m_llStartTime + cControls.dFwdBackSecs * 1000000));
            return Success;
        case ID_PLAY_SKIPBACK:
            JumpTo(static_cast<mms_t>(m_llStartTime - cControls.dFwdBackSecs * 1000000));
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            if (FAILED(m_pRenderer->ResetDevice())) return DirectXError;
            return Success;
        case ID_VIEW_MOVEANDZOOM:
            if (cView.GetZoomMove())
            {
                cView.SetOffsetX(cView.GetOffsetX() + m_fTempOffsetX);
                cView.SetOffsetY(cView.GetOffsetY() + m_fTempOffsetY);
                cView.SetZoomX(cView.GetZoomX() * m_fTempZoomX);
                m_fTempOffsetX = 0.0f;
                m_fTempOffsetY = 0.0f;
                m_fTempZoomX = 1.0f;
                cView.SetZoomMove(false);
                return Success;
            }
            else
            {
                cView.SetZoomMove(true);
                return Success;
            }
        case ID_VIEW_RESETMOVEANDZOOM:
            if (m_fTempOffsetX == 0.0f && m_fTempOffsetY == 0.0f && m_fTempZoomX == 1.0f) {
                cView.SetOffsetX(0.0f);
                cView.SetOffsetY(0.0f);
                cView.SetZoomX(1.0f);
            }
            m_fTempOffsetX = 0.0f;
            m_fTempOffsetY = 0.0f;
            m_fTempZoomX = 1.0f;
            UpdateNotePos = true;
            return Success;
        }
        break;
    }
    // These are doubled from MainProcs.cpp. Allows to get rid of the Ctrl requirement for accellerators
    case WM_DEVICECHANGE:
        if (cAudio.bKDMAPI) {
            m_OutDevice.OpenKDMAPI();
        }
        else {
            m_OutDevice.Open(cAudio.iOutDevice);
        }
        break;
    case TBM_SETPOS:
    {
        mms_t llFirstTime = GetMinTime();
        mms_t llLastTime = GetMaxTime();
        bool StartTimer = (JumpTarget == ~0);
        JumpTarget = llFirstTime + ((llLastTime - llFirstTime) * lParam) / 1000;
        if (StartTimer) SetTimer(g_hWnd, IDC_POSNDELAY, nxtdelay, NULL); //Async JumpTo process
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (m_bZoomMove)
        {
            m_ptLastPos.x = (winword_t)LOWORD(lParam);
            m_ptLastPos.y = (winword_t)HIWORD(lParam);
            m_bTrackPos = true;
        }
        return Success;
    }
    case WM_RBUTTONDOWN:
    {
        if (!m_bZoomMove) return Success;
        m_ptLastPos.x = (winword_t)LOWORD(lParam);
        m_ptLastPos.y = (winword_t)HIWORD(lParam);
        m_ptStartZoom.x = static_cast<win32_t>((m_ptLastPos.x - m_fOffsetX - m_fTempOffsetX) / (m_fZoomX * m_fTempZoomX));
        m_ptStartZoom.y = static_cast<win32_t>(m_ptLastPos.y - m_fOffsetY - m_fTempOffsetY);
        m_bTrackZoom = true;
        return Success;
    }
    case WM_CAPTURECHANGED:
        m_bTrackPos = m_bTrackZoom = false;
        return Success;
    case WM_LBUTTONUP:
        m_bTrackPos = false;
        return Success;
    case WM_RBUTTONUP:
        m_bTrackZoom = false;
        return Success;
    case WM_MOUSEMOVE:
    {
        if (!m_bTrackPos && !m_bTrackZoom) return Success;
        winword_t x = LOWORD(lParam);
        winword_t y = HIWORD(lParam);
        winword_t dx = static_cast<winword_t>(x - m_ptLastPos.x);
        winword_t dy = static_cast<winword_t>(y - m_ptLastPos.y);

        if (m_bTrackPos)
        {
            m_fTempOffsetX += dx;
            m_fTempOffsetY += dy;
            UpdateNotePos = true; //Update Note Pos! 
        }
        if (m_bTrackZoom)
        {
            float fOldX = m_fOffsetX + m_fTempOffsetX + m_ptStartZoom.x * m_fZoomX * m_fTempZoomX;
            m_fTempZoomX *= pow(2.0f, dx / 200.0f);
            float fNewX = m_fOffsetX + m_fTempOffsetX + m_ptStartZoom.x * m_fZoomX * m_fTempZoomX;
            m_fTempOffsetX = m_fTempOffsetX - (fNewX - fOldX);
            UpdateNotePos = true; //Update Note Pos! 
        }

        m_ptLastPos.x = x;
        m_ptLastPos.y = y;
        return Success;
    }
    case WM_TIMER:
    {
        if (wParam == IDC_POSNDELAY) {
            KillTimer(g_hWnd, IDC_POSNDELAY);
            if (JumpTarget != ~0) {
                UINT start = GetTickCount();
                JumpTo(JumpTarget, false);
                JumpTarget = ~0;
                UINT end = GetTickCount() + (1 << 2);
                nxtdelay = max(1 << 2, min(end - start, 1 << 10));
            }
        }
        return Success;
    }
    }

    return Success;
}

GameState::GameError MainScreen::Logic() {
    static Config& config = Config::GetConfig();
    static PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    static const ViewSettings& cView = config.GetViewSettings();
    static const VisualSettings& cVisual = config.GetVisualSettings();
    static const VideoSettings& cVideo = config.GetVideoSettings();
    static const ControlsSettings& cControls = config.GetControlsSettings();

    // people are probably going to yell at me if you can't change the bar color during playback
    m_csKBRed.SetColor(cVisual.iBarColor, 0.5f);

    // Detect changes in state
    bool bPaused = cPlayback.GetPaused();
    double dSpeed = cPlayback.GetSpeed();
    double dNSpeed = cPlayback.GetNSpeed();
    bool bMute = cPlayback.GetMute();
    mms_t llTimeSpan = static_cast<mms_t>(3.0 * abs(dNSpeed) * 1000000);
    bool bPausedChanged = (bPaused != m_bPaused);
    bool bMuteChanged = (bMute != m_bMute);
    key_t velthrshld = cControls.iVelocityThreshold % (1 << 7);

    // Set the state
    m_bTickMode = cVideo.bTickBased && !(m_MIDI.GetInfo().iDivision & 0x8000);
    m_bPaused = bPaused;
    m_dSpeed = dSpeed;
    m_dNSpeed = dNSpeed;
    m_bMute = bMute;
    m_llTimeSpan = m_bTickMode ? abs(dNSpeed) * 3000 : llTimeSpan;
    m_dVolume = cPlayback.GetVolume();
    m_bShowKB = cView.GetKeyboard();
    m_bZoomMove = cView.GetZoomMove();
    m_fOffsetX = cView.GetOffsetX();
    m_fOffsetY = cView.GetOffsetY();
    m_fZoomX = cView.GetZoomX();
    if (!m_bZoomMove) m_bTrackPos = m_bTrackZoom = false;
    m_eKeysShown = cVisual.eKeysShown;
    if (m_eKeysShown == VisualSettings::All)
    {
        m_iStartNote = 0;
        m_iEndNote = 127;
    }
    else if (m_eKeysShown == VisualSettings::Song)
    {
        const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
        m_iStartNote = mInfo.iMinNote;
        m_iEndNote = mInfo.iMaxNote;
    }
    else {
        m_iStartNote = max(0, min(127, min(cVisual.iFirstKey, cVisual.iLastKey)));
        m_iEndNote = max(0, min(127, max(cVisual.iFirstKey, cVisual.iLastKey)));
    }
    m_bFlipKeyboard = cVisual.iFirstKey > cVisual.iLastKey && cVisual.eKeysShown != 0 && cVisual.eKeysShown != 1;
    m_bSameWidth = cVideo.bSameWidth;
    m_bRemoveOverlaps = cVideo.bOR;
    m_bMapVel = cVideo.bMapVel;
    m_pRenderer->SetLimitFPS(cVideo.bLimitFPS);
    if (cVisual.iBkgColor != m_csBackground.iOrigBGR) m_csBackground.SetColor(cVisual.iBkgColor, 0.7f, 1.3f);

    if (cVideo.eMarkerEncoding != m_iCurEncoding) {
        m_iCurEncoding = cVideo.eMarkerEncoding;
        ApplyMarker(m_pMarkerData, m_iMarkerSize);
    }

    // Time stuff
    mms_t llMaxTime = GetMaxTime();
    mms_t llMinTime = GetMinTime();
    mms_t llElapsed = m_Timer.GetMicroSecs();
    m_Timer.Start();

    // Compute FPS
    m_llFPSTime += llElapsed;
    m_iFPSCount++;
    if (m_llFPSTime >= 100000)
    {
        m_dFPS = m_iFPSCount / (m_llFPSTime / 1000000.0);
        m_llFPSTime = m_iFPSCount = 0;
    }

    if ((bPausedChanged || bMuteChanged) && (m_bPaused || m_bMute)) m_OutDevice.AllNotesOff();

    // Figure out start and end times for display
    mms_t llOldStartTime = m_llStartTime;
    mms_t llNextStartTime = m_llStartTime + static_cast<mms_t>(llElapsed * m_dSpeed + 0.5);

    if (!m_bPaused) m_llStartTime = llNextStartTime;
    m_iStartTick = GetCurrentTick(m_llStartTime);
    mms_t llEndTime = 0;
    if (m_bTickMode) {
        if (dNSpeed < 0) {
            llEndTime = m_iStartTick - m_llTimeSpan;
        }
        else {
            llEndTime = m_iStartTick + m_llTimeSpan;
        }
    }
    else {
        if (dNSpeed < 0) {
            llEndTime = m_llStartTime - m_llTimeSpan;
        }
        else {
            llEndTime = m_llStartTime + m_llTimeSpan;
        }
    }

    if (abs(llOldStartTime - m_llPrevTime) > 1e+6 / (m_dFPS / 4)) { // Handle time jump from cheat engine
        JumpTo(m_llStartTime, true);
    }
    else {
        m_llPrevTime = m_llStartTime;
    }

    idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
    RenderGlobals();

    // We must advance the end position AFTER start position when drawing in reversed order! 
    // So let's skip this section for now, then come back to do it later. 
    if (dNSpeed >= 0) {
    AdvanceEnd:
        if (m_bTickMode) {
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsT() > llEndTime)) {
                m_iEndPos--; //Make sure we're 10000% not drawing any unnecessary notes! 
            }
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsT() < llEndTime) {
                m_iEndPos++;
            }
        }
        else {
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() > llEndTime)) {
                m_iEndPos--; //Make sure we're 10000% not drawing any unnecessary notes! 
            }
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() < llEndTime) {
                m_iEndPos++;
            }
        }
        if (dNSpeed < 0) goto DoneAdvance;
    }

    // Advance the start position! 
    if (!m_bPaused)
    {
        bool Reverse = m_dSpeed < 0;
        if (Reverse) {
            if (!IsLastFrameReversed) m_iStartPos--;
            IsLastFrameReversed = true;
        }
        else {
            if (IsLastFrameReversed) m_iStartPos++;
            IsLastFrameReversed = false;
        }

        // We want to use a different loop head in different scenario. 
        if (Reverse) goto ReversedLoopCondition; else goto NormalLoopCondition;
    ReversedLoopCondition:
        if (m_iStartPos > 0 && m_vEvents[m_iStartPos]->GetAbsMicroSec() >= m_llStartTime) goto LoopBody; else goto LoopEnd;
    NormalLoopCondition:
        if (m_iStartPos < iEventCount&& m_vEvents[m_iStartPos]->GetAbsMicroSec() <= m_llStartTime) goto LoopBody; else goto LoopEnd;

        // Here comes the loop body! 
    LoopBody:
        {
            const MIDIChannelEvent* pEvent = m_vEvents[m_iStartPos];
            key_t key = pEvent->GetParam1();
            key_t vel = pEvent->GetParam2();
            idx_t ogsistidx = pEvent->GetSisterIdx();
            if (IsNote(pEvent->GetChannelEventType()) && pEvent->HasSister() && Reverse) {
                pEvent = m_vEvents[ogsistidx];
                vel = pEvent->GetParam2();
            }
            if (IsOff(pEvent->GetChannelEventType(), vel) && pEvent->HasSister()) {
                vel = pEvent->GetSister(m_vEvents)->GetParam2();
            }

            if (IsNotNote(pEvent->GetChannelEventType())) {
                if (pEvent->GetChannelEventType() == MIDIChannelEvent::ProgramChange && config.m_bPianoOverride) {
                    key &= 0x00;
                }
                if (pEvent->GetChannelEventType() == MIDIChannelEvent::PitchBend) {
                    m_pBendsValue[pEvent->GetChannel()] = ((vel << 7) | key) - (1 << 13);
                    goto PitchBendUpdate; // Update PB display. 
                }
                if (pEvent->GetChannelEventType() == MIDIChannelEvent::Controller) {
                    if (key == MIDIChannelEvent::RPNType) {
                        Next_is_PBS[pEvent->GetChannel()] = (vel == MIDIChannelEvent::PBSRPNID);
                    }
                    if (key == MIDIChannelEvent::RPNData && Next_is_PBS[pEvent->GetChannel()]) {
                        m_pBendsRange[pEvent->GetChannel()] = vel;

                    PitchBendUpdate: // Update PB display. 
                        float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
                        float ShiftAmount = m_pBendsRange[pEvent->GetChannel()] == 0 ? 0 : m_pBendsValue[pEvent->GetChannel()] / ((1 << 13) / m_pBendsRange[pEvent->GetChannel()]);
                        if (m_bFlipKeyboard) ShiftAmount *= -1;
                        m_pBends[pEvent->GetChannel()] = NoteWidth * ShiftAmount;
                    }
                }
                m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, vel);
            }
            else if (!m_bMute && !m_vTrackSettings[pEvent->GetTrack() % MaxTrackColors].aChannels[pEvent->GetChannel()].bMuted && vel > velthrshld && pEvent->HasSister()) {
                // We're playing a note! 
                if (IsOn(pEvent->GetChannelEventType(), pEvent->GetParam2())) {
                    m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, NoteVelFormula(vel));
                }
                else {
                    m_OutDevice.PlayEvent(off2on(pEvent->GetEventCode()), key, 0x00);
                }
            }
        if (IsNote(pEvent->GetChannelEventType()) && pEvent->HasSister()) {
                idx_t idx = Reverse ? ogsistidx : static_cast<idx_t>(m_iStartPos);
                idx_t sister = IsOn(pEvent->GetChannelEventType(), pEvent->GetParam2()) ? IDX_MAX : (Reverse ? static_cast<idx_t>(m_iStartPos) : ogsistidx);
                if (Reverse) {
                    UpdateStateBackwards(idx, sister);
                }
                else {
                    UpdateState(idx, sister);
                }
            }

            // Advance loop counter and start the next itteration. 
            if (Reverse) {
                m_iStartPos--;
                goto ReversedLoopCondition;
            }
            else {
                m_iStartPos++;
                goto NormalLoopCondition;
            }
        }
    LoopEnd:
        ;
    }

    // Advance the end position for negative note speed. 
    if (dNSpeed < 0) {
        m_iEndPos += (m_iPrevStartPos - m_iEndPos) * 2;
        m_iEndPos = max(m_iEndPos, 0);
        goto AdvanceEnd;
    DoneAdvance:
        m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
    }
    m_iPrevStartPos = m_iStartPos;

    AdvanceIterators(m_llStartTime, false);

    // Update the position slider
    mms_t llFirstTime = GetMinTime();
    mms_t llLastTime = GetMaxTime();
    mms_t llOldPos = ((llOldStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime);
    mms_t llNewPos = ((m_llStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime);
    if (llOldPos != llNewPos) cPlayback.SetPosition(static_cast<win32_t>(llNewPos));

    // Song's over
    if (!m_bPaused && ((m_dSpeed < 0) ? (m_llStartTime < llMinTime) : (m_llStartTime > llMaxTime))) {
        if (m_bDumpFrames) {
            wchar_t buf[1 << 10] = {};
            swprintf(buf, sizeof(buf), L"start \"Result\" \"C:\\Windows\\Explorer.exe\" \"%s\\PianoFX_Framedump\\\"", GetExePath().c_str());
            _wsystem(buf);
            CloseHandle(m_hVideoPipe);
            m_bDumpFrames = false;
        }
        cPlayback.SetPaused(true, true);
    }

    if (m_Timer.m_bManualTimer)
        m_Timer.IncrementFrame();

    // Update root constants
    auto& root_consts = m_pRenderer->GetRootConstants();
    root_consts.deflate = clamp(round(m_fWhiteCX * 0.15f / 2.0f), 1.0f, 3.0f);
    root_consts.notes_y = m_fNotesY;
    root_consts.notes_cy = m_fNotesCY;
    root_consts.white_cx = m_fWhiteCX;
    root_consts.timespan = (float)m_llTimeSpan;

    // Update fixed size constants
    auto& fixed_consts = m_pRenderer->GetFixedSizeConstants();
    memcpy(&fixed_consts.note_x, &notex_table, sizeof(float) * 128);
    if (cVideo.bVisualizePitchBends)
        memcpy(&fixed_consts.bends, &m_pBends, sizeof(float) * 16);
    else
        memset(&fixed_consts.bends, 0, sizeof(float) * 16);

    // Update track colors
    // TODO: Only update track colors lazily
    auto* track_colors = m_pRenderer->GetTrackColors();
    for (track_t i = 0; i < min(m_vTrackSettings.size(), MaxTrackColors); i++) {
        for (chan_t j = 0; j < MaxChannelColors; j++) {
            auto& src = m_vTrackSettings[i].aChannels[j];
            auto& dst = track_colors[i * 16 + j];
            dst.primary = src.iPrimaryRGB;
            dst.dark = src.iDarkRGB;
            dst.darker = src.bHidden ? 0xFFFFFFFF : src.iVeryDarkRGB; // Hack to signal hidden track without checking on CPU
        }
    }

    return Success;
}

void MainScreen::UpdateState(idx_t idx, idx_t sister_idx) {
    if (sister_idx == IDX_MAX) {
        // Note-on: push_back preserves sorted order in forward playback
        m_vState.push_back(idx);
    }
    else {
        // Note-off: binary search + erase
        idx_t pos = LocateElement(m_vState, sister_idx);
        if (pos != IDX_MAX) {
            m_vState.erase(m_vState.begin() + pos);
        }
    }
}

void MainScreen::UpdateStateBackwards(idx_t idx, idx_t sister_idx) {
    if (sister_idx == IDX_MAX) {
        // Note-on in reverse: indices arrive out of order, need sorted insert
        InsertSorted(m_vState, idx);
    }
    else {
        // Note-off in reverse: binary search + erase
        idx_t pos = LocateElement(m_vState, sister_idx);
        if (pos != IDX_MAX) {
            m_vState.erase(m_vState.begin() + pos);
        }
    }
}

void MainScreen::JumpTo(mms_t llStartTime, boolean loadingMode) {
    // Kill the music!
    if (!loadingMode) m_OutDevice.AllNotesOff();

    // Start time. Piece of cake!
    mms_t llFirstTime;
    mms_t llLastTime;
    mms_t llEndTime;
    if (!loadingMode) {
        llFirstTime = GetMinTime();
        llLastTime = GetMaxTime();
        m_llStartTime = min(max(llStartTime, llFirstTime), llLastTime);
    }

    if (m_dNSpeed < 0) {
        if (m_bTickMode) {
            llEndTime = m_iStartTick - m_llTimeSpan;
        }
        else {
            llEndTime = m_llStartTime - m_llTimeSpan;
        }
    }
    else {
        if (m_bTickMode) {
            llEndTime = m_iStartTick + m_llTimeSpan;
        }
        else {
            llEndTime = m_llStartTime + m_llTimeSpan;
        }
    }

    // Find start position...
    auto itBegin = m_vEvents.begin();
    auto itEnd = m_vEvents.end();
    auto itMiddle = lower_bound(itBegin, itEnd, llStartTime, [&](MIDIChannelEvent* lhs, const mms_t rhs) {
        return lhs->GetAbsMicroSec() < rhs;
        });
    // We've found it! Set m_iStartPos to our latest findings now.
    m_iStartPos = itMiddle - m_vEvents.begin();

    // Find the notes that occur simultaneously with the previous note on...
    m_vState.clear();
    if (itMiddle != itEnd)
    {
        // Find the previous note on...
        for (; itMiddle != itBegin; itMiddle--) {
            if (IsOn((*itMiddle)->GetChannelEventType(),(*itMiddle)->GetParam2())) { break; }
        }
        // Found it!
        // It's totally possible that the jump lands before the first note but after a tempo, meta, or time signature event.
        // So we must do an extra check here to ensure we REALLY found a note.
        if (IsOn((*itMiddle)->GetChannelEventType(),(*itMiddle)->GetParam2()) && (*itMiddle)->GetAbsMicroSec() <= m_llStartTime) {
            MIDIChannelEvent* pTargetEvent = *itMiddle;
            if (pTargetEvent->GetSister(m_vEvents)->GetAbsMicroSec() > m_llStartTime) {
                m_vState.push_back(itMiddle - m_vEvents.begin());
            }
            idx_t iFound = 0;
            idx_t iSimultaneous = pTargetEvent->GetSimultaneous();
            if (iSimultaneous > 0 && itMiddle != itBegin) {
                for (itMiddle--; iFound < iSimultaneous && itMiddle != itBegin; itMiddle--)
                {
                    MIDIChannelEvent* pEvent = *itMiddle;
                    if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2())) {
                        if (pEvent->GetSister(m_vEvents)->GetAbsMicroSec() > pTargetEvent->GetAbsMicroSec()) { // > because itMiddle is the max for its time
                            iFound++;
                        }
                        if (pEvent->GetSister(m_vEvents)->GetAbsMicroSec() > m_llStartTime) { // > because we don't care about simultaneous ending notes
                            m_vState.push_back(itMiddle - m_vEvents.begin());
                        }
                    }
                }
                if (iFound != iSimultaneous) {
                    MessageBoxW(NULL, (wstring(L"") + L"Exp: " + to_wstring(iSimultaneous) + L"\nFound:" + to_wstring(iFound)).c_str(), L"Error", MB_OK);
                }
                // If there's only one note, there's no need to reverse.
                // So let's put this statement inside the if check.
                reverse(m_vState.begin(), m_vState.end());
            }
        }
    }

    AdvanceIterators(llStartTime, true);
    m_iStartTick = GetCurrentTick(m_llStartTime);

    // End position: a little tricky. Same as logic code. Only needed for paused jumping.
    if (m_dNSpeed < 0) {
        if (m_bTickMode) {
            m_iEndPos = m_iStartPos + 1;
            idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsT() > llEndTime))
                m_iEndPos--;
            m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
        }
        else {
            m_iEndPos = m_iStartPos + 1;
            idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() > llEndTime))
                m_iEndPos--;
            m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
        }
    }
    else {
        if (m_bTickMode) {
            m_iEndPos = m_iStartPos - 1;
            idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsT() < llEndTime)
                m_iEndPos++;
        }
        else {
            m_iEndPos = m_iStartPos - 1;
            idx_t iEventCount = static_cast<idx_t>(m_vEvents.size());
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() < llEndTime)
                m_iEndPos++;
        }
    }

    if (!loadingMode)
    {
        static PlaybackSettings& cPlayback = Config::GetConfig().GetPlaybackSettings();
        win32_t llNewPos = static_cast<win32_t>(((m_llStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime));
        cPlayback.SetPosition(llNewPos);
    }

    IsLastFrameReversed = false;
    m_llPrevTime = m_llStartTime;
    m_iPrevStartPos = m_iStartPos;
}

void MainScreen::ApplyColor(MIDIMetaEvent * event) {
    const auto size = event->GetDataLen();
    const auto data = event->GetData();
    if (event->GetMetaEventType() == MIDIMetaEvent::GenericTextA &&
        (size == 8 || size == 12) &&
        data[0] == 0x00 && data[1] == 0x0F &&
        (data[2] < 16 || data[2] == 0x7F) &&
        data[3] == 0) {
        unsigned color = ((0xFF - data[7]) << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
        if (data[2] == 0x7F) {
            //all channels
            for (chan_t i = 0; i < MaxChannelColors; i++) {
                auto& chan_settings = m_vTrackSettings[event->GetTrack() % MaxTrackColors].aChannels[i];
                chan_settings.SetColor(color);
                chan_settings.bHidden = data[7] == 0;
            }
        }
        else {
            auto& chan_settings = m_vTrackSettings[event->GetTrack() % MaxTrackColors].aChannels[data[2]];
            chan_settings.SetColor(color);
            chan_settings.bHidden = data[7] == 0;
        }
    }
}

void MainScreen::ApplyMarker(unsigned char* data, msgln_t size) {
    m_pMarkerData = data;
    m_iMarkerSize = size;
    if (data) {
        static Config& config = Config::GetConfig();
        static const VideoSettings& cVideo = config.GetVideoSettings();

        constexpr uint16_t codepages[] = { 1252, 437, 82, 886, 932, 936, CP_UTF8 };

        auto temp_str = new char[size + 1];
        memcpy(temp_str, data, size);
        temp_str[size] = '\0';

        auto wide_len = MultiByteToWideChar(codepages[cVideo.eMarkerEncoding], 0, temp_str, size + 1, NULL, 0);
        auto wide_temp_str = new WCHAR[wide_len];
        MultiByteToWideChar(codepages[cVideo.eMarkerEncoding], 0, temp_str, size + 1, wide_temp_str, wide_len);

        m_sMarker = wstring(wide_temp_str);

        delete[] wide_temp_str;
        delete[] temp_str;
    }
    else {
        m_sMarker = wstring();
    }
}

// Advance program change, tempo, and signature
void MainScreen::AdvanceIterators(mms_t llTime, bool bIsJump) {
    if (bIsJump)
    {
        m_itNextTempo = upper_bound(m_vTempo.begin(), m_vTempo.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
        MIDIMetaEvent* pPrevious = GetPrevious(m_itNextTempo, m_vTempo, 3);
        if (pPrevious)
        {
            MIDI::Parse24Bit(pPrevious->GetData(), 3, (uint32_t*)&m_iMicroSecsPerBeat);
            m_iMicroSecsPerBeat |= !m_iMicroSecsPerBeat;// Clamp to > 0
            m_iLastTempoTick = pPrevious->GetAbsT();
            m_llLastTempoTime = pPrevious->GetAbsMicroSec();
        }
        else
        {
            m_iMicroSecsPerBeat = 500000;
            m_llLastTempoTime = m_iLastTempoTick = 0;
        }
        m_itNextSignature = upper_bound(m_vSignature.begin(), m_vSignature.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
        pPrevious = GetPrevious(m_itNextSignature, m_vSignature, 4);
        if (pPrevious)
        {
            m_iBeatsPerMeasure = pPrevious->GetData()[0];
            m_iBeatType = 1 << pPrevious->GetData()[1];
            m_iClocksPerMet = pPrevious->GetData()[2];
            m_iLastSignatureTick = pPrevious->GetAbsT();
        }
        else
        {
            m_iBeatsPerMeasure = 4;
            m_iBeatType = 4;
            m_iClocksPerMet = 24;
            m_iLastSignatureTick = 0;
        }
        auto itCurMarker = m_itNextMarker;
        m_itNextMarker = upper_bound(m_vMarkers.begin(), m_vMarkers.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
        if (!m_bNextMarkerInited || itCurMarker != m_itNextMarker) {
            m_bNextMarkerInited = true;
            if (m_itNextMarker != m_vMarkers.begin() && (m_itNextMarker - 1)->second != -1) {
                const auto eEvent = m_vMetaEvents[(m_itNextMarker - 1)->second];
                ApplyMarker(eEvent->GetData(), eEvent->GetDataLen());
            }
            else {
                ApplyMarker(nullptr, 0);
            }
        }
        if (m_dSpeed < 0) {
            m_itNextColor = lower_bound(m_vColors.begin(), m_vColors.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
        }
        else {
            m_itNextColor = upper_bound(m_vColors.begin(), m_vColors.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
        }
    }
    else
    {
        if (m_dSpeed < 0) {
            m_itNextTempo = upper_bound(m_vTempo.begin(), m_vTempo.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
            MIDIMetaEvent* pPrevious = GetPrevious(m_itNextTempo, m_vTempo, 3);
            if (pPrevious)
            {
                MIDI::Parse24Bit(pPrevious->GetData(), 3, (uint32_t*)&m_iMicroSecsPerBeat);
                m_iMicroSecsPerBeat |= !m_iMicroSecsPerBeat;// Clamp to > 0
                m_iLastTempoTick = pPrevious->GetAbsT();
                m_llLastTempoTime = pPrevious->GetAbsMicroSec();
            }
            else
            {
                m_iMicroSecsPerBeat = 500000;
                m_llLastTempoTime = m_iLastTempoTick = 0;
            }
            m_itNextSignature = upper_bound(m_vSignature.begin(), m_vSignature.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
            pPrevious = GetPrevious(m_itNextSignature, m_vSignature, 4);
            if (pPrevious)
            {
                m_iBeatsPerMeasure = pPrevious->GetData()[0];
                m_iBeatType = 1 << pPrevious->GetData()[1];
                m_iClocksPerMet = pPrevious->GetData()[2];
                m_iLastSignatureTick = pPrevious->GetAbsT();
            }
            else
            {
                m_iBeatsPerMeasure = 4;
                m_iBeatType = 4;
                m_iClocksPerMet = 24;
                m_iLastSignatureTick = 0;
            }
            auto itCurMarker = m_itNextMarker;
            m_itNextMarker = upper_bound(m_vMarkers.begin(), m_vMarkers.end(), pair<mms_t, idx_t>(llTime, m_vMetaEvents.size()));
            if (!m_bNextMarkerInited || itCurMarker != m_itNextMarker) {
                m_bNextMarkerInited = true;
                if (m_itNextMarker != m_vMarkers.begin() && (m_itNextMarker - 1)->second != -1) {
                    const auto eEvent = m_vMetaEvents[(m_itNextMarker - 1)->second];
                    ApplyMarker(eEvent->GetData(), eEvent->GetDataLen());
                }
                else {
                    ApplyMarker(nullptr, 0);
                }
            }
            for (; m_itNextColor != m_vColors.begin() && (m_itNextColor - 1)->first >= llTime; --m_itNextColor)
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[(m_itNextColor - 1)->second];
                ApplyColor(pEvent);
            }
        }
        else {
            for (; m_itNextTempo != m_vTempo.end() && m_itNextTempo->first <= llTime; ++m_itNextTempo)
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[m_itNextTempo->second];
                if (pEvent->GetDataLen() == 3)
                {
                    MIDI::Parse24Bit(pEvent->GetData(), 3, (uint32_t*)&m_iMicroSecsPerBeat);
                    m_iMicroSecsPerBeat |= !m_iMicroSecsPerBeat;// Clamp to > 0
                    m_iLastTempoTick = pEvent->GetAbsT();
                    m_llLastTempoTime = pEvent->GetAbsMicroSec();
                }
            }
            for (; m_itNextSignature != m_vSignature.end() && m_itNextSignature->first <= llTime; ++m_itNextSignature)
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[m_itNextSignature->second];
                if (pEvent->GetDataLen() == 4)
                {
                    m_iBeatsPerMeasure = pEvent->GetData()[0];
                    m_iBeatType = 1 << pEvent->GetData()[1];
                    m_iClocksPerMet = pEvent->GetData()[2];
                    m_iLastSignatureTick = pEvent->GetAbsT();
                }
            }
            auto itCurMarker = m_itNextMarker;
            while (m_itNextMarker != m_vMarkers.end() && m_itNextMarker->first <= llTime) ++m_itNextMarker;
            if (itCurMarker != m_itNextMarker) {
                if (m_itNextMarker != m_vMarkers.begin() && (m_itNextMarker - 1)->second != -1) {
                    const auto eEvent = m_vMetaEvents[(m_itNextMarker - 1)->second];
                    ApplyMarker(eEvent->GetData(), eEvent->GetDataLen());
                }
                else {
                    ApplyMarker(nullptr, 0);
                }
            }
            for (; m_itNextColor != m_vColors.end() && m_itNextColor->first <= llTime; ++m_itNextColor)
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[m_itNextColor->second];
                ApplyColor(pEvent);
            }
        }
    }
}

// Might change the value of itCurrent
MIDIMetaEvent* MainScreen::GetPrevious(eventvec_t::const_iterator & itCurrent, const eventvec_t & vEventMap, msgln_t iDataLen) {
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    eventvec_t::const_iterator it = itCurrent;
    if (itCurrent != vEventMap.begin())
    {
        while (it != vEventMap.begin())
            if (m_vMetaEvents[(--it)->second]->GetDataLen() == iDataLen)
                return m_vMetaEvents[it->second];
    }
    else if (vEventMap.size() > 0 && itCurrent->first <= mInfo.llFirstNote && m_vMetaEvents[itCurrent->second]->GetDataLen() == iDataLen)
    {
        MIDIMetaEvent* pPrevious = m_vMetaEvents[itCurrent->second];
        ++itCurrent;
        return pPrevious;
    }
    return NULL;
}

// Gets the tick corresponding to llStartTime using current tempo
mtk_t MainScreen::GetCurrentTick(mms_t llStartTime) {
    return GetCurrentTick(llStartTime, m_iLastTempoTick, m_llLastTempoTime, m_iMicroSecsPerBeat);
}

mtk_t MainScreen::GetCurrentTick(mms_t llStartTime, mtk_t iLastTempoTick, mms_t llLastTempoTime, bpm_t iMicroSecsPerBeat)
{
    uint16_t iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000))
    {
        if (llStartTime >= llLastTempoTime)
            return iLastTempoTick + static_cast<mtk_t>((iDivision * (llStartTime - llLastTempoTime)) / iMicroSecsPerBeat);
        else
            return iLastTempoTick - static_cast<mtk_t>((iDivision * (llLastTempoTime - llStartTime) + 1) / iMicroSecsPerBeat) - 1;
    }
    return -1;
}

// Gets the time corresponding to the tick
mms_t MainScreen::GetTickTime(mtk_t iTick) {
    return GetTickTime(iTick, m_iLastTempoTick, m_llLastTempoTime, m_iMicroSecsPerBeat);
}

mms_t MainScreen::GetTickTime(mtk_t iTick, mtk_t iLastTempoTick, mms_t llLastTempoTime, bpm_t iMicroSecsPerBeat) {
    uint16_t iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000))
        return llLastTempoTime + (static_cast<mms_t>(iMicroSecsPerBeat) * (iTick - iLastTempoTick)) / iDivision;
    return -1;
}

// Rounds up to the nearest beat
bpm_t MainScreen::GetBeat(mtk_t iTick, bpm_t iBeatType, mtk_t iLastSignatureTick) {
    uint16_t iDivision = m_MIDI.GetInfo().iDivision;
    mtk_t iTickOffset = iTick - iLastSignatureTick;
    if (iDivision & 0x8000) { return -1; }
    else {
        m_CurBeat = (iTickOffset * iBeatType) / (iDivision * 4);

        if (iTickOffset > 0) {
            return (iTickOffset * iBeatType - 1) / (iDivision * 4) + 1;
        }
        else {
            return (iTickOffset * iBeatType) / (iDivision * 4);
        }
    }

}

// Rounds up to the nearest beat
mtk_t MainScreen::GetBeatTick(mtk_t iTick, bpm_t iBeatType, mtk_t iLastSignatureTick) {
    uint16_t iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000)) { return iLastSignatureTick + (GetBeat(iTick, iBeatType, iLastSignatureTick) * iDivision * 4) / iBeatType; }
    return -1;
}

const float MainScreen::SharpRatio = 0.65f;
const float MainScreen::KBPercent = 0.25f;
const float MainScreen::KeyRatio = 0.1775f;

GameState::GameError MainScreen::Render()
{
    if (FAILED(m_pRenderer->ResetDeviceIfNeeded())) return DirectXError;

    // Update background if it changed
    static Config& config = Config::GetConfig();
    static const VisualSettings& cVisual = config.GetVisualSettings();
    const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    if (cVisual.sBackground != m_sCurBackground || cVisual.sBackground.empty()) {
        m_bBackgroundLoaded = cVisual.sBackground.empty() ? false : m_pRenderer->LoadBackgroundBitmap(cVisual.sBackground);
        m_sCurBackground = cVisual.sBackground;
    }

    if (FAILED(m_pRenderer->ClearAndBeginScene(m_csBackground.iPrimaryRGB))) return DirectXError;

    if (m_fZoomX * m_fTempZoomX != 0.0f) {
        RenderLines();
        RenderNotes();
        if (m_bShowKB) RenderKeys();
    }
    RenderText();

    // Present the backbuffer contents to the display
    if (FAILED(m_pRenderer->EndScene(m_bBackgroundLoaded))) return DirectXError;

    // Frame dump!
    if (config.m_bManualTimer && CE_Connected) {
        // Send a signal to CE and wait for CE to do its job...
        CE_DoNextTick = true;
        while (!CE_Responded) { _mm_pause(); }
        // CE has responded! 
        CE_Responded = false;
    }
    // Dump frame!!!!
    if (m_bDumpFrames && !cPlayback.GetPaused()) {
        // Write to pipe
        HWND hGDI = FindWindowW(L"PFXGDI", NULL);
        if (CE_Connected && IsWindow(hGDI)) {
            char* Frame = new char[m_pRenderer->GetBufferWidth() * m_pRenderer->GetBufferHeight() * 4];
            HRESULT res = GetGDI(hGDI, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight(), Frame);
            if (res == S_OK) {
                WriteFile(m_hVideoPipe, Frame, static_cast<DWORD>(m_pRenderer->GetBufferWidth() * m_pRenderer->GetBufferHeight() * 4), nullptr, nullptr);
                delete[] Frame;
            }
            else if (res == E_NOTIMPL) {
                delete[] Frame;
            }
            else {
                return DirectXError;
            }
        }
        else {
            char* Frame = new char[m_pRenderer->GetBufferWidth() * m_pRenderer->GetBufferHeight() * 4];
            HRESULT res = m_pRenderer->Screenshot(Frame);
            if (res == S_OK) {
                WriteFile(m_hVideoPipe, Frame, static_cast<DWORD>(m_pRenderer->GetBufferWidth() * m_pRenderer->GetBufferHeight() * 4), nullptr, nullptr);
                delete[] Frame;
            }
            else if (res == E_NOTIMPL) {
                delete[] Frame;
            }
            else {
                return DirectXError;
            }
        }
    }
    if (FAILED(m_pRenderer->Present())) return DirectXError;

    return Success;
}

// These used to be created as local variables inside each Render* function, but too much copying of code :/
// Depends on m_llStartTime, m_llTimeSpan, m_eKeysShown, m_iStartNote, m_iEndNote
void MainScreen::RenderGlobals() {
    // Screen X info
    m_fNotesX = m_fOffsetX + m_fTempOffsetX;
    m_fNotesCX = m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX);

    // Keys info
    m_iAllWhiteKeys = MIDI::WhiteCount(m_iStartNote, m_iEndNote + 1);
    float fBuffer = (MIDI::IsSharp(m_iStartNote) ? SharpRatio / 2.0f : 0.0f) +
        (MIDI::IsSharp(m_iEndNote) ? SharpRatio / 2.0f : 0.0f);
    m_fWhiteCX = m_fNotesCX / (m_iAllWhiteKeys + fBuffer);

    // Screen Y info
    m_fNotesY = m_fOffsetY + m_fTempOffsetY;
    if (!m_bShowKB)
        m_fNotesCY = static_cast<float>(m_pRenderer->GetBufferHeight());
    else
    {
        float fMaxKeyCY = m_pRenderer->GetBufferHeight() * KBPercent;
        float fIdealKeyCY = m_fWhiteCX / KeyRatio;
        // .95 for the top vs near. 2.0 for the spacer. .93 for the transition and the red. ESTIMATE.
        fIdealKeyCY = (fIdealKeyCY / 0.95f + 2.0f) / 0.93f;
        m_fNotesCY = floor(m_pRenderer->GetBufferHeight() - min(fIdealKeyCY, fMaxKeyCY) + 0.5f);
    }
    if (m_fZoomX * m_fTempZoomX < 0) {
        m_fNotesY += m_pRenderer->GetBufferHeight() - m_fNotesCY;
    }

    // Round down start time. This is only used for rendering purposes
    if (m_bTickMode) {
        m_llRndStartTime = m_iStartTick;
    }
    else {
        mms_t llMicroSecsPP = static_cast<mms_t>(m_llTimeSpan / m_fNotesCY + 0.5f);
        if (llMicroSecsPP != 0) {
            m_llRndStartTime = m_llStartTime - (m_llStartTime < 0 ? llMicroSecsPP : 0);
            m_llRndStartTime = (m_llRndStartTime / llMicroSecsPP) * llMicroSecsPP;
        }
    }

    if (UpdateNotePos) GenNoteXTable();
}

void MainScreen::RenderLines() {
    if (m_bBackgroundLoaded)
        return;

    // Vertical lines
    for (key_t i = m_iStartNote + 1; i <= m_iEndNote; i++)
        if (!MIDI::IsSharp(i - 1) && !MIDI::IsSharp(i))
        {
            key_t iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
            float fStartX = MIDI::IsSharp(m_iStartNote) * SharpRatio / 2.0f;
            float x = m_fNotesX + m_fWhiteCX * (iWhiteKeys + fStartX);
            x = floor(x + 0.5f); // Needs to be rounded because of the gradient
            m_pRenderer->DrawRect(x - 1.0f, m_fNotesY, 3.0f, m_fNotesCY,
                m_csBackground.iDarkRGB, m_csBackground.iVeryDarkRGB, m_csBackground.iVeryDarkRGB, m_csBackground.iDarkRGB);
        }

    // Horizontal (Hard!)
    uint16_t iDivision = m_MIDI.GetInfo().iDivision;
    // fuck this lmao
    if (!(iDivision & 0x8000))
    {
        // Copy time state vars
        mtk_t iCurrTick = m_iStartTick - 1;
        mms_t llEndTime = (m_bTickMode ? m_iStartTick : m_llStartTime) + m_llTimeSpan;

        // Copy tempo state vars
        mtk_t iLastTempoTick = m_iLastTempoTick;
        bpm_t iMicroSecsPerBeat = m_iMicroSecsPerBeat;
        mms_t llLastTempoTime = m_llLastTempoTime;
        eventvec_t::const_iterator itNextTempo = m_itNextTempo;

        // Copy signature state vars
        mtk_t iLastSignatureTick = m_iLastSignatureTick;
        bpm_t iBeatsPerMeasure = m_iBeatsPerMeasure;
        bpm_t iBeatType = m_iBeatType;
        eventvec_t::const_iterator itNextSignature = m_itNextSignature;

        // Compute initial next beat tick and next beat time
        mms_t llNextBeatTime = 0;
        mtk_t iNextBeatTick = 0;
        do
        {
            iNextBeatTick = GetBeatTick(iCurrTick + 1, iBeatType, iLastSignatureTick);

            // Next beat crosses the next tempo event. handle the event and recalculate next beat time
            while (itNextTempo != m_vTempo.end() && m_vMetaEvents[itNextTempo->second]->GetDataLen() == 3 &&
                iNextBeatTick > m_vMetaEvents[itNextTempo->second]->GetAbsT())
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[itNextTempo->second];
                MIDI::Parse24Bit(pEvent->GetData(), 3, (uint32_t*)&iMicroSecsPerBeat);
                iMicroSecsPerBeat |= !iMicroSecsPerBeat;// Clamp to > 0
                iLastTempoTick = pEvent->GetAbsT();
                llLastTempoTime = pEvent->GetAbsMicroSec();
                ++itNextTempo;
            }
            while (itNextSignature != m_vSignature.end() && m_vMetaEvents[itNextSignature->second]->GetDataLen() == 4 &&
                iNextBeatTick > m_vMetaEvents[itNextSignature->second]->GetAbsT())
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[itNextSignature->second];
                iBeatsPerMeasure = pEvent->GetData()[0];
                iBeatType = 1 << pEvent->GetData()[1];
                iLastSignatureTick = pEvent->GetAbsT();
                iNextBeatTick = GetBeatTick(iLastSignatureTick + 1, iBeatType, iLastSignatureTick);
                ++itNextSignature;
            }

            // Finally render the beat or measure
            bpm_t iNextBeat = GetBeat(iNextBeatTick, iBeatType, iLastSignatureTick);
            bool bIsMeasure = !((iNextBeat < 0 ? -iNextBeat : iNextBeat) % iBeatsPerMeasure);
            llNextBeatTime = GetTickTime(iNextBeatTick, iLastTempoTick, llLastTempoTime, iMicroSecsPerBeat);
            float y = m_fNotesY + m_fNotesCY * (1.0f - ((float)(m_bTickMode ? iNextBeatTick : llNextBeatTime) - m_llRndStartTime) / m_llTimeSpan);
            y = floor(y + 0.5f);
            if (bIsMeasure && y + 1.0f > m_fNotesY)
                m_pRenderer->DrawRect(m_fNotesX, y - 1.0f, m_fNotesCX, 3.0f,
                    m_csBackground.iDarkRGB, m_csBackground.iDarkRGB, m_csBackground.iVeryDarkRGB, m_csBackground.iVeryDarkRGB);

            iCurrTick = iNextBeatTick;
        } while ((m_bTickMode ? iNextBeatTick : llNextBeatTime) <= llEndTime);
        // hopefully no race condition?
    }
}

void MainScreen::RenderNotes() {
    sidx_t iStartPos = m_dNSpeed < 0 ? m_iStartPos - (m_iEndPos - m_iStartPos) + 1 : m_iStartPos;
    sidx_t iEndPos = m_dNSpeed < 0 ? m_iEndPos - (m_iEndPos - m_iStartPos) - 1 : m_iEndPos;

    if (iStartPos < 0 || iEndPos >= m_vEvents.size()) return; // the note speed has been changed after processing these positions but before reaching here. 

    // Ensure that any rects rendered after this point render over the notes
    m_pRenderer->SplitRect();
    InitKeyColor();

    auto PressAndBlend = [&](const MIDIChannelEvent* pEvent) __attribute__((always_inline)) {
        ChannelSettings CS = m_vTrackSettings[pEvent->GetTrack() % MaxTrackColors].aChannels[pEvent->GetChannel()];
        if (!CS.bHidden) {
            if (m_bMapVel) {
                CS.iPrimaryRGB = CS.iPrimaryRGB & 0x00FFFFFF | ((pEvent->GetParam2() ^ 0x7F) << 24);
                CS.iDarkRGB = CS.iDarkRGB & 0x00FFFFFF | ((pEvent->GetParam2() ^ 0x7F) << 24);
                CS.iVeryDarkRGB = CS.iVeryDarkRGB & 0x00FFFFFF | ((pEvent->GetParam2() ^ 0x7F) << 24);
            }
            NoteColor NC = { CS.iPrimaryRGB, CS.iDarkRGB, CS.iVeryDarkRGB };
            BlendNoteColor(&m_pKeyColors[pEvent->GetParam1()], &NC);
            PushKey(pEvent->GetParam1()); // Mark key as pressed. 
        }
    };

    if (Config::GetConfig().GetVideoSettings().bOR) {
        if (m_dNSpeed < 0) {
            for (sidx_t i = iStartPos; i <= iEndPos; i++) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (IsOff(pEvent->GetChannelEventType(),pEvent->GetParam2()) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent->GetSister(m_vEvents));
                }
            }
        }
        else {
            for (sidx_t i = iEndPos; i >= iStartPos; i--) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2()) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent);
                }
            }
        }
        for (vector<idx_t>::reverse_iterator it = m_vState.rbegin(); it != m_vState.rend(); it++) {
            MIDIChannelEvent* pEvent = m_vEvents[*it];
            if (m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                RenderNote(pEvent);
                if (!IsPressed(pEvent->GetParam1())) {
                    PressAndBlend(pEvent);
                }
            }
        }
    }
    else {
        for (vector<idx_t>::iterator it = m_vState.begin(); it != m_vState.end(); it++) {
            MIDIChannelEvent* pEvent = m_vEvents[*it];
            RenderNote(pEvent);
            PressAndBlend(pEvent);
        }
        if (m_dNSpeed < 0) {
            for (sidx_t i = iEndPos; i >= iStartPos; i--) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (IsOff(pEvent->GetChannelEventType(),pEvent->GetParam2()) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent->GetSister(m_vEvents));
                }
            }
        }
        else {
            for (sidx_t i = iStartPos; i <= iEndPos; i++) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (IsOn(pEvent->GetChannelEventType(),pEvent->GetParam2()) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent);
                }
            }
        }
    }
}

void MainScreen::RenderNote(const MIDIChannelEvent * pNote) {
    key_t iNote = pNote->GetParam1();
    chan_t iChannel = pNote->GetChannel();
    track_t iTrack = pNote->GetTrack();
    mms_t llNoteStart = pNote->GetAbsMicroSec();
    mms_t llNoteEnd = pNote->GetSister(m_vEvents)->GetAbsMicroSec();
    key_t iVel = m_bMapVel ? pNote->GetParam2() ^ 0x7F : 0x7F;
    if (m_bTickMode) {
        llNoteStart = pNote->GetAbsT();
        llNoteEnd = pNote->GetSister(m_vEvents)->GetAbsT();
    }
    float fPos = static_cast<float>(
        (m_dNSpeed < 0) != (m_fZoomX * m_fTempZoomX < 0)
        ? -(llNoteStart - m_llRndStartTime) - (llNoteEnd - llNoteStart) + (m_fZoomX * m_fTempZoomX < 0 ? m_llTimeSpan : 0)
        : llNoteStart - m_llRndStartTime + (m_fZoomX * m_fTempZoomX < 0 ? m_llTimeSpan : 0)
        );
    // Watch the magic happen! 
    union { float f; uint32_t i; } uLength;
    uLength.f = static_cast<float>(llNoteEnd - llNoteStart);
    iNote |= ((iVel & 0x01) << 7);
    iChannel |= ((iVel & 0x1E) << 3);
    uLength.i &= 0x7FFFFFFE;
    uLength.i |= (iVel & 0x20) << 26;
    uLength.i |= (iVel >> 6) & 0x01;
    // Push it to the GPU. 
    m_pRenderer->PushNoteData(
        NoteData{
            .key = iNote,
            .channel = iChannel,
            .track = iTrack,
            .pos = fPos,
            .length = uLength.f,
        }
    );
}

void MainScreen::GenNoteXTable() {
    UpdateNotePos = false; //We don't need to do this on every single frame! 
    float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
    for (chan_t ch = 0; ch < MaxChannelColors; ch++) {
        float ShiftAmount = m_pBendsRange[ch] == 0 ? 0 : m_pBendsValue[ch] / ((1 << 13) / m_pBendsRange[ch]);
        if (m_bFlipKeyboard) ShiftAmount *= -1;
        m_pBends[ch] = NoteWidth * ShiftAmount;
    }
    bool Direction = !m_bFlipKeyboard;
    for (key_t i = m_iStartNote; i <= m_iEndNote; i++) {
        if (m_bSameWidth) {
            float fStartNote = m_iStartNote;
            float fEndNote = m_iEndNote;
            float KeyCount = i - (m_iStartNote - !Direction);
            if (MIDI::IsSharp(m_iStartNote - 1)) {
                fStartNote -= 0.5f;
                KeyCount += 0.5f;
            }
            if (MIDI::IsSharp(m_iEndNote + 1)) {
                fEndNote += 0.5f;
            }
            if (Direction) {
                notex_table[i] = m_fNotesX + (m_fNotesCX / ((fEndNote - fStartNote) + 1)) * KeyCount;
            }
            else {
                notex_table[i] = m_fNotesX + (m_fNotesCX - ((m_fNotesCX / ((fEndNote - fStartNote) + 1)) * KeyCount));
            }

        }
        else {
            key_t iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
            float fStartX = (MIDI::IsSharp(m_iStartNote) - MIDI::IsSharp(i)) * SharpRatio / 2.0f;
            if (MIDI::IsSharp(i))
            {
                MIDI::Note eNote = MIDI::NoteVal(i);
                if (eNote == MIDI::CS || eNote == MIDI::FS) fStartX -= SharpRatio / 5.0f;
                else if (eNote == MIDI::AS || eNote == MIDI::DS) fStartX += SharpRatio / 5.0f;
            }
            if (Direction) {
                notex_table[i] = m_fNotesX + m_fWhiteCX * (iWhiteKeys + fStartX);
            }
            else {
                notex_table[i] = m_fNotesX + (m_fNotesCX - (m_fWhiteCX * (iWhiteKeys + fStartX)) - (MIDI::IsSharp(i) ? m_fWhiteCX * SharpRatio : m_fWhiteCX));
            }
        }
    }
}

float MainScreen::GetNoteX(key_t iNote) {
    return notex_table[iNote];
}

void MainScreen::RenderKeys() {
    // Screen info
    float fKeysY = m_fZoomX * m_fTempZoomX < 0 ? m_fNotesY - (m_pRenderer->GetBufferHeight() - m_fNotesCY) : m_fNotesY + m_fNotesCY;
    float fKeysCY = m_pRenderer->GetBufferHeight() - m_fNotesCY;

    float fTransitionPct = .02f;
    float fTransitionCY = max(3.0f, floor(fKeysCY * fTransitionPct + 0.5f));
    float fRedPct = .05f;
    float fRedCY = floor(fKeysCY * fRedPct + 0.5f);
    float fSpacerCY = 2.0f;
    float fTopCY = floor((fKeysCY - fSpacerCY - fRedCY - fTransitionCY) * 0.95f + 0.5f);
    float fNearCY = fKeysCY - fSpacerCY - fRedCY - fTransitionCY - fTopCY;

    // Draw the background
    m_pRenderer->DrawRect(m_fNotesX, fKeysY, m_fNotesCX, fKeysCY, m_csKBBackground.iPrimaryRGB);
    if (m_fZoomX * m_fTempZoomX < 0) {
        m_pRenderer->DrawRect(m_fNotesX, fKeysY + fKeysCY - fTransitionCY, m_fNotesCX, fTransitionCY,
            m_csKBBackground.iVeryDarkRGB, m_csKBBackground.iVeryDarkRGB, m_csBackground.iPrimaryRGB, m_csBackground.iPrimaryRGB);
        m_pRenderer->DrawRect(m_fNotesX, fKeysY + fKeysCY - fTransitionCY - fRedCY, m_fNotesCX, fRedCY,
            m_csKBRed.iPrimaryRGB, m_csKBRed.iPrimaryRGB, m_csKBRed.iDarkRGB, m_csKBRed.iDarkRGB);
        m_pRenderer->DrawRect(m_fNotesX, fKeysY + fKeysCY - fTransitionCY - fRedCY - fSpacerCY, m_fNotesCX, fSpacerCY,
            m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB);
    }
    else {
        m_pRenderer->DrawRect(m_fNotesX, fKeysY, m_fNotesCX, fTransitionCY,
            m_csBackground.iPrimaryRGB, m_csBackground.iPrimaryRGB, m_csKBBackground.iVeryDarkRGB, m_csKBBackground.iVeryDarkRGB);
        m_pRenderer->DrawRect(m_fNotesX, fKeysY + fTransitionCY, m_fNotesCX, fRedCY,
            m_csKBRed.iDarkRGB, m_csKBRed.iDarkRGB, m_csKBRed.iPrimaryRGB, m_csKBRed.iPrimaryRGB);
        m_pRenderer->DrawRect(m_fNotesX, fKeysY + fTransitionCY + fRedCY, m_fNotesCX, fSpacerCY,
            m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB);
    }

    // Keys info
    float fKeyGap = max(1.0f, floor(m_fWhiteCX * 0.05f + 0.5f));
    float fKeyGap1 = fKeyGap - floor(fKeyGap / 2.0f + 0.5f);
    float fSharpCY = fTopCY * 0.67f;
#define FLIP_FORMULA fKeysY + (fKeysCY - fTransitionCY - fRedCY - fSpacerCY) / 2, m_fZoomX * m_fTempZoomX < 0

    // Draw the white keys
    float fCurX = m_fNotesX + (MIDI::IsSharp(m_bFlipKeyboard ? m_iEndNote : m_iStartNote) ? m_fWhiteCX * (SharpRatio / 2.0f - 1.0f) : 0.0f);
    float fCurY = m_fZoomX * m_fTempZoomX < 0 ? fKeysY : fKeysY + fTransitionCY + fRedCY + fSpacerCY;
    for (key_t i = (m_bFlipKeyboard ? m_iEndNote : m_iStartNote); m_bFlipKeyboard ? (i >= m_iStartNote && !(i & 0x80)) : (i <= m_iEndNote); i += (m_bFlipKeyboard ? -1 : 1))
        if (!MIDI::IsSharp(i))
        {
            NoteColor KeyColor = m_pKeyColors[i];
            if (!IsPressed(i))
            {
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY, m_fWhiteCX - fKeyGap, fTopCY + fNearCY,
                    m_csKBWhite.iDarkRGB, m_csKBWhite.iDarkRGB, m_csKBWhite.iPrimaryRGB, m_csKBWhite.iPrimaryRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY, m_fWhiteCX - fKeyGap, fNearCY,
                    m_csKBWhite.iDarkRGB, m_csKBWhite.iDarkRGB, m_csKBWhite.iVeryDarkRGB, m_csKBWhite.iVeryDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY, m_fWhiteCX - fKeyGap, 2.0f,
                    m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBWhite.iVeryDarkRGB, m_csKBWhite.iVeryDarkRGB,
                    FLIP_FORMULA);
            }
            else {
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY, m_fWhiteCX - fKeyGap, fTopCY + fNearCY - 2.0f,
                    KeyColor.iDarkRGB, KeyColor.iDarkRGB, KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY + fNearCY - 2.0f, m_fWhiteCX - fKeyGap, 2.0f, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
            }
            m_pRenderer->DrawRect(floor(fCurX + fKeyGap1 + m_fWhiteCX - fKeyGap + 0.5f), fCurY, fKeyGap, fTopCY + fNearCY,
                m_csKBBackground.iVeryDarkRGB, m_csKBBackground.iPrimaryRGB, m_csKBBackground.iPrimaryRGB, m_csKBBackground.iVeryDarkRGB,
                FLIP_FORMULA);

            fCurX += m_fWhiteCX;
        }

    // Draw the sharps
    float fSharpTop = SharpRatio * 0.7f;
    fCurX = m_fNotesX + (MIDI::IsSharp(m_bFlipKeyboard ? m_iEndNote : m_iStartNote) ? m_fWhiteCX * SharpRatio / 2.0f : 0.0f);
    fCurY = m_fZoomX * m_fTempZoomX < 0 ? fKeysY : fKeysY + fTransitionCY + fRedCY + fSpacerCY;
    for (key_t i = (m_bFlipKeyboard ? m_iEndNote : m_iStartNote); m_bFlipKeyboard ? (i >= m_iStartNote && !(i & 0x80)) : (i <= m_iEndNote); i += (m_bFlipKeyboard ? -1 : 1))
        if (!MIDI::IsSharp(i)) { fCurX += m_fWhiteCX; }
        else
        {
            float fNudgeX = 0.0;
            MIDI::Note eNote = MIDI::NoteVal(i);
            if (eNote == MIDI::CS || eNote == MIDI::FS) fNudgeX = -SharpRatio / 5.0f;
            else if (eNote == MIDI::AS || eNote == MIDI::DS) fNudgeX = SharpRatio / 5.0f;

            const float cx = m_fWhiteCX * SharpRatio;
            const float x = fCurX - m_fWhiteCX * (SharpRatio / 2.0f - (m_bFlipKeyboard ? -fNudgeX : fNudgeX));
            const float fSharpTopX1 = x + m_fWhiteCX * (SharpRatio - fSharpTop) / 2.0f;
            const float fSharpTopX2 = fSharpTopX1 + m_fWhiteCX * fSharpTop;

            NoteColor KeyColor = m_pKeyColors[i];
            if (!IsPressed(i)) {
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY + fSharpCY - fNearCY,
                    fSharpTopX2, fCurY + fSharpCY - fNearCY,
                    x + cx, fCurY + fSharpCY, x, fCurY + fSharpCY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY,
                    fSharpTopX1, fCurY + fSharpCY - fNearCY,
                    x, fCurY + fSharpCY, x, fCurY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX2, fCurY + fSharpCY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY,
                    x + cx, fCurY, x + cx, fCurY + fSharpCY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawRect(fSharpTopX1, fCurY - fNearCY, fSharpTopX2 - fSharpTopX1, fSharpCY, m_csKBSharp.iVeryDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f,
                    fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f,
                    m_csKBSharp.iDarkRGB, m_csKBSharp.iDarkRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.65f,
                    fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.55f,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB,
                    FLIP_FORMULA);
            }
            else {
                const float fNewNear = fNearCY * 0.25f;

                m_pRenderer->DrawSkew(fSharpTopX1, fCurY + fSharpCY - fNewNear,
                    fSharpTopX2, fCurY + fSharpCY - fNewNear,
                    x + cx, fCurY + fSharpCY, x, fCurY + fSharpCY,
                    KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iDarkRGB, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear,
                    fSharpTopX1, fCurY + fSharpCY - fNewNear,
                    x, fCurY + fSharpCY, x, fCurY,
                    KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iDarkRGB, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX2, fCurY + fSharpCY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear,
                    x + cx, fCurY, x + cx, fCurY + fSharpCY,
                    KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iDarkRGB, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawRect(fSharpTopX1, fCurY - fNewNear, fSharpTopX2 - fSharpTopX1, fSharpCY, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f,
                    fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f,
                    KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB,
                    FLIP_FORMULA);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.75f,
                    fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.65f,
                    KeyColor.iPrimaryRGB, KeyColor.iPrimaryRGB, KeyColor.iDarkRGB, KeyColor.iDarkRGB,
                    FLIP_FORMULA);
            }
        }
}

#ifndef StatisticsWidth
#define StatisticsWidth 260
#endif

void MainScreen::RenderText() {
    static Config& config = Config::GetConfig();
    static const VideoSettings& cVideo = config.GetVideoSettings();
    static const ControlsSettings& cControls = config.GetControlsSettings();

    unsigned char Lines = 10; //Basic info
    if (m_MIDI.GetInfo().iDivision & 0x8000) Lines -= 1; // SMPTE
    if (cVideo.bDebug) {
        Lines += 10; //Debug info
    }
    if (cControls.bPhigros) {
        Lines += 4; //Score and level
    }
    if (cControls.bDumpFrames) {
        if (cVideo.bDebug) {
            Lines += 1; //Playback speed
        }
    }
    else {
        if (cVideo.bDebug) {
            Lines += 1; //Playback speed
        }
        else if (m_Timer.m_bManualTimer) {
            Lines += 1; //Playback speed
        }
    }
    if (cControls.bDumpFrames) {
        Lines -= 1; //Don't show FPS when exporting videos
    }

    // Screen info
    RECT rcStatus = { m_pRenderer->GetBufferWidth() - StatisticsWidth, 0, m_pRenderer->GetBufferWidth(), 16 * Lines + 8 };
    RECT rcMsg = { max(0,min(m_pRenderer->GetBufferWidth(),static_cast<LONG>(m_fNotesX))), max(0,min(m_pRenderer->GetBufferHeight(),static_cast<LONG>(m_fNotesY))), max(0,min(m_pRenderer->GetBufferWidth(),static_cast<LONG>(m_fNotesCX + m_fNotesX))), max(0,min(m_pRenderer->GetBufferHeight(),static_cast<LONG>(m_fNotesCY + m_fNotesY))) };
    RECT rcScr = { 0, 0, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight() };

    // Draw the text
    RenderStatus(&rcStatus);
    if (!m_sMarker.empty() && cVideo.bShowMarkers) {
        RenderMarker(m_sMarker.c_str());
    }
    if (m_bZoomMove) {
        RenderMessage(&rcMsg, &rcScr, ZoomMoveMsg, 'L');
    }
    if (CheatEngineCaption[0] != 'C' && CheatEngineCaption[0] != 'L' && CheatEngineCaption[0] != 'R') {
        CheatEngineCaption[0] = 'C';
    }
    if (strnlen(CheatEngineCaption, sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0])) > 1) {
        if (strnlen(CheatEngineCaption, sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0])) < sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0])) {
            RenderMessage(&rcMsg, &rcScr, Utf8ToWString(CaptionContent), CheatEngineCaption[0]);
        }
        else {
            RenderMessage(&rcMsg, &rcScr, Utf8ToWString("The caption has exceeded the maximum acceptable length of " + to_string(sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]) - 2) + " characters. \nAs a result, this caption has been blocked from showing in order to prevent crashing. \nPlease consider writing something slightly shorter. "), 'L');
        }
    }
}

void MainScreen::RenderStatusLine(unsigned char line, const wchar_t* left, const wchar_t* format, ...) {
    if (Config::GetConfig().GetVideoSettings().bDisableUI) return;

    wchar_t buf[1 << 10] = {};
    va_list varargs;
    va_start(varargs, format);
    vswprintf_s(buf, _countof(buf), format, varargs);
    va_end(varargs);

    win32_t TextY = 2 + line * 16;
    win32_t LeftTextX = m_pRenderer->GetBufferWidth() - (StatisticsWidth - 6);
    win32_t RightTextX = m_pRenderer->GetBufferWidth() - 6;
    m_pRenderer->AddText(left, 1 << 4, LeftTextX + 2, TextY, 0x404040, ALIGN_LEFT | ALIGN_TOP);
    m_pRenderer->AddText(left, 1 << 4, LeftTextX, TextY - 2, 0xFFFFFF, ALIGN_LEFT | ALIGN_TOP);
    m_pRenderer->AddText(buf, 1 << 4, RightTextX + 2, TextY, 0x404040, ALIGN_RIGHT | ALIGN_TOP);
    m_pRenderer->AddText(buf, 1 << 4, RightTextX, TextY - 2, 0xFFFFFF, ALIGN_RIGHT | ALIGN_TOP);
}

void MainScreen::RenderStatus(LPRECT prcStatus) {
    Config& config = Config::GetConfig();
    const VisualSettings& cVisual = config.GetVisualSettings();
    const ViewSettings& cView = config.GetViewSettings();
    const VideoSettings& cVideo = config.GetVideoSettings();
    const ControlsSettings& cControls = config.GetControlsSettings();
    const PlaybackSettings& cPlayback = config.GetPlaybackSettings();

    width = m_pRenderer->GetBufferWidth();
    height = m_pRenderer->GetBufferHeight();
    mms_t min = abs(m_llStartTime) / 60000000;
    mms_t sec = (abs(m_llStartTime) % 60000000) / 1000000;
    mms_t cs = (abs(m_llStartTime) % 1000000) / 100000;
    mms_t tmin = TotalTime / 60000000;
    mms_t tsec = (TotalTime % 60000000) / 1000000;
    mms_t tcs = (TotalTime % 1000000) / 100000;
    double tempo = 60000000.0 / m_iMicroSecsPerBeat;
    mms_t iMaxMS = m_MIDI.GetInfo().llTotalMicroSecs / MS;
    uint8_t cur_line = 0;

    if (!cVideo.bDisableUI) {
        m_pRenderer->AddGDIRect(prcStatus->left, prcStatus->top, prcStatus->right - prcStatus->left, prcStatus->bottom - prcStatus->top, 0x80000000);
    }

    TotalTimeFormatted = to_string(TotalTime);
    uint8_t Splits = 0;
    for (signed short i = TotalTimeFormatted.length() - 3; Splits < 2 && i > 0; i -= 3, Splits++)
        TotalTimeFormatted.insert(i, " ");

    llStartTimeFormatted = to_string(abs(m_llStartTime));
    Splits = 0;
    for (signed short i = llStartTimeFormatted.length() - 3; Splits < 2 && i > 0; i -= 3, Splits++)
        llStartTimeFormatted.insert(i, " ");
    if (m_llStartTime < 0)
        llStartTimeFormatted.insert(0, "-");

    polyphony = m_vState.size();
    polyFormatted = to_wstring(polyphony);
    for (signed short i = polyFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        polyFormatted.insert(i, L",");

    nps = m_vNCTable[min(max(m_llStartTime / MS, 0LL), iMaxMS)] - m_vNCTable[min(max((m_llStartTime - S) / MS, 0LL), iMaxMS)];
    npsFormatted = to_wstring(nps);
    for (signed short i = npsFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        npsFormatted.insert(i, L",");

    passed = m_vNCTable[min(max(m_llStartTime / MS, 0LL), iMaxMS)];
    passedFormatted = to_wstring(passed);
    for (signed short i = passedFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        passedFormatted.insert(i, L",");

    RenderStatusLine(cur_line++, StatisticsText1, L"%s", L"v" VersionString);
    RenderStatusLine(cur_line++, StatisticsText2, L"");
    RenderStatusLine(cur_line++, L"", L"");
    RenderStatusLine(cur_line++, StatisticsText3, L"%s%lld:%02d.%d / %lld:%02d.%d",
        m_llStartTime >= 0 ? L"" : L"-",
        min, sec, cs,
        tmin, tsec, tcs);
    if (!(m_MIDI.GetInfo().iDivision & 0x8000)) RenderStatusLine(cur_line++, StatisticsText4, L"%d/%d", m_iStartTick, resolution);
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText5, L"%d", m_llStartTime);
    }
    if (!cControls.bDumpFrames) {
        RenderStatusLine(cur_line++, StatisticsText6, L"%.2lf", m_dFPS);
    }
    if (m_MIDI.GetInfo().iDivision & 0x8000) {
        unsigned short iFramesPerSec = static_cast<unsigned short>(-static_cast<signed char>(m_MIDI.GetInfo().iDivision >> 8)) * static_cast<unsigned short>(100);
        if (iFramesPerSec == 2900) iFramesPerSec = 2997;
        if (iFramesPerSec == 5900) iFramesPerSec = 5994;
        if (iFramesPerSec == 11900) iFramesPerSec = 11988;
        unsigned char iTicksPerFrame = m_MIDI.GetInfo().iDivision & 0xFF;
        iFramesPerSec |= !iFramesPerSec;// Clamp to > 0
        iTicksPerFrame |= !iTicksPerFrame;// Clamp to > 0
        RenderStatusLine(cur_line++, StatisticsText7, L"%d TPF @%.2f FPS", iTicksPerFrame, static_cast<float>(iFramesPerSec) / 100.0f);
    }
    else {
        RenderStatusLine(cur_line++, StatisticsText7, L"%.3lf BPM", tempo);
    }
    if (cControls.bPhigros) {
        RenderStatusLine(cur_line++, StatisticsText8, L"%s", passedFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText9, L"%s", npsFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText10, L"%s", polyFormatted.c_str());
    }
    else {
        RenderStatusLine(cur_line++, StatisticsText11, L"%s", passedFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText12, L"%s", npsFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText13, L"%s", polyFormatted.c_str());
    }
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText14, L"%d", m_pRenderer->GetRenderedNotesCount());
        if (m_bMute) {
            RenderStatusLine(cur_line++, StatisticsText15, L"%s", StatisticsText16);
        }
        else {
            RenderStatusLine(cur_line++, StatisticsText15, L"%.0lf%%", cPlayback.GetVolume() * 100);
        }
    }
    if (cControls.bDumpFrames) {
        if (cVideo.bDebug) {
            RenderStatusLine(cur_line++, StatisticsText17, L"%.0lf%%", cPlayback.GetSpeed() * 100);
        }
    }
    else {
        if (cVideo.bDebug) {
            if (m_Timer.m_bManualTimer) {
                RenderStatusLine(cur_line++, StatisticsText17, L"%.0lf%%", cPlayback.GetSpeed() * (m_dFPS / m_Timer.m_dFramerate) * 100);
            }
            else {
                RenderStatusLine(cur_line++, StatisticsText17, L"%.0lf%%", cPlayback.GetSpeed() * 100);
            }
        }
        else {
            if (m_Timer.m_bManualTimer) {
                RenderStatusLine(cur_line++, StatisticsText17, L"%.0lf%%", (m_dFPS / m_Timer.m_dFramerate) * 100);
            }
        }
    }
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText18, L"%f", cPlayback.GetNSpeed());
        RenderStatusLine(cur_line++, StatisticsText19, L"%f", cView.GetOffsetX() + m_fTempOffsetX);
        RenderStatusLine(cur_line++, StatisticsText20, L"%f", cView.GetOffsetY() + m_fTempOffsetY);
        RenderStatusLine(cur_line++, StatisticsText21, L"%f", cView.GetZoomX() * m_fTempZoomX);
        RenderStatusLine(cur_line++, StatisticsText22, L"%d*%d", width, height);
        RenderStatusLine(cur_line++, StatisticsText23, L"%d~%d", m_bFlipKeyboard ? m_iEndNote : m_iStartNote, m_bFlipKeyboard ? m_iStartNote : m_iEndNote);
        RenderStatusLine(cur_line++, StatisticsText24, L"%d", cControls.iVelocityThreshold);
    }
    if (cControls.bPhigros) {
        RenderStatusLine(cur_line++, StatisticsText25, L"%07.0f", (passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)));
        RenderStatusLine(cur_line++, StatisticsText26, L"%s", Utf8ToWString(Difficulty).c_str());
        RenderStatusLine(cur_line++, StatisticsText27, L"%s", StatisticsText28 L" ");
        if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) == static_cast<float>(1000000)) {
            RenderStatusLine(cur_line++, StatisticsText29, L"");
        }
        else if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) < static_cast<float>(1000000)) {
            if (!cPlayback.GetPaused()) {
                RenderStatusLine(cur_line++, StatisticsText30, L"");
            }
            else {
                RenderStatusLine(cur_line++, StatisticsText31, L"");
            }
        }
        else if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) > static_cast<float>(1000000)) {
            RenderStatusLine(cur_line++, StatisticsText32, L"");
        }
    }
    if (IsWindowVisible(GetConsoleWindow()) && !IsIconic(GetConsoleWindow()) && (FrameCount & 0x0F) == 0) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD pos;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        uint8_t line;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        line = 0;
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << "  "; for (uint8_t i = 0; i < (csbi.dwSize.X - (1 << 4)) / 2; i++) cout << "="; cout << " Debug Info "; for (uint8_t i = 0; i < (csbi.dwSize.X - (1 << 4)) / 2; i++) cout << "="; cout << "  ";
        pos.X = csbi.dwSize.X - 1;
        SetConsoleCursorPosition(hConsole, pos);
        cout << "\n";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    SongLength: " + TotalTimeFormatted + " (" + IntSizeToCE(sizeof(TotalTime)) + " +" + GetAddress(TotalTime) + ")[Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Microseconds: " + llStartTimeFormatted + " (" + IntSizeToCE(sizeof(m_llStartTime)) + " +" + GetAddress(m_llStartTime) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Ticks: " + to_string(m_iStartTick) + " (" + IntSizeToCE(sizeof(m_iStartTick)) + " +" + GetAddress(m_iStartTick) + ")[Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Resolution: " + to_string(resolution) + " (" + IntSizeToCE(sizeof(resolution)) + " +" + GetAddress(resolution) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NoteCount: " + to_string(TotalNC) + " (" + IntSizeToCE(sizeof(TotalNC)) + " +" + GetAddress(TotalNC) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NotesPerSecond: " + to_string(nps) + " (" + IntSizeToCE(sizeof(nps)) + " +" + GetAddress(nps) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Polyphony: " + to_string(polyphony) + " (" + IntSizeToCE(sizeof(polyphony)) + " +" + GetAddress(polyphony) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Passed: " + to_string(passed) + " (" + IntSizeToCE(sizeof(passed)) + " +" + GetAddress(passed) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Volume: " + to_string(cPlayback.GetVolume()) + " (" + FloatSizeToCE(cPlayback.GetVolumeSize()) + " +" + cPlayback.GetVolumeAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Mute: " + to_string(cPlayback.GetMute()) + " (" + IntSizeToCE(cPlayback.GetMuteSize()) + " +" + cPlayback.GetMuteAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    PlaybackSpeed: " + to_string(cPlayback.GetSpeed()) + " (" + FloatSizeToCE(cPlayback.GetSpeedSize()) + " +" + cPlayback.GetSpeedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NoteSpeed: " + to_string(cPlayback.GetNSpeed()) + " (" + FloatSizeToCE(cPlayback.GetNSpeedSize()) + " +" + cPlayback.GetNSpeedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    OffsetX: " + to_string(cView.GetOffsetX()) + " (" + FloatSizeToCE(cView.GetOffsetXSize()) + " +" + cView.GetOffsetXAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    OffsetY: " + to_string(cView.GetOffsetY()) + " (" + FloatSizeToCE(cView.GetOffsetYSize()) + " +" + cView.GetOffsetYAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Zoom: " + to_string(cView.GetZoomX()) + " (" + FloatSizeToCE(cView.GetZoomXSize()) + " +" + cView.GetZoomXAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    SameWidthNotes: " + to_string(cVideo.bSameWidth) + " (" + IntSizeToCE(sizeof(cVideo.bSameWidth)) + " +" + GetAddress(cVideo.bSameWidth) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NoteVelocityMapping: " + to_string(cVideo.bMapVel) + " (" + IntSizeToCE(sizeof(cVideo.bMapVel)) + " +" + GetAddress(cVideo.bMapVel) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    KeyRange: " + to_string(cVisual.iFirstKey) + "~" + to_string(cVisual.iLastKey) + " (" + IntSizeToCE(sizeof(cVisual.iFirstKey)) + " +" + GetAddress(cVisual.iFirstKey) + " ~ " + IntSizeToCE(sizeof(cVisual.iLastKey)) + " +" + GetAddress(cVisual.iLastKey) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    KeyMode: " + to_string(cVisual.eKeysShown) + " (" + IntSizeToCE(sizeof(cVisual.eKeysShown)) + " +" + GetAddress(cVisual.eKeysShown) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    WindowSize: " + to_string(width) + "*" + to_string(height) + " (" + IntSizeToCE(sizeof(width)) + " +" + GetAddress(width) + " * " + IntSizeToCE(sizeof(height)) + " +" + GetAddress(height) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Paused: " + to_string(cPlayback.GetPaused()) + " (" + IntSizeToCE(cPlayback.GetPausedSize()) + " +" + cPlayback.GetPausedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Keyboard: " + to_string(cView.GetKeyboard()) + " (" + IntSizeToCE(cView.GetKeyboardVarSize()) + " +" + cView.GetKeyboardAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    VisualizePitchBends: " + to_string(cVideo.bVisualizePitchBends) + " (" + IntSizeToCE(sizeof(cVideo.bVisualizePitchBends)) + " +" + GetAddress(cVideo.bVisualizePitchBends) + ")[Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    PhigrosMode: " + to_string(cControls.bPhigros) + " (" + IntSizeToCE(sizeof(cControls.bPhigros)) + " +" + GetAddress(cControls.bPhigros) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    ShowMarkers: " + to_string(cVideo.bShowMarkers) + " (" + IntSizeToCE(sizeof(cVideo.bShowMarkers)) + " +" + GetAddress(cVideo.bShowMarkers) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    TickBased: " + to_string(cVideo.bTickBased) + " (" + IntSizeToCE(sizeof(cVideo.bTickBased)) + " +" + GetAddress(cVideo.bTickBased) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    HideStatistics: " + to_string(cVideo.bDisableUI) + " (" + IntSizeToCE(sizeof(cVideo.bDisableUI)) + " +" + GetAddress(cVideo.bDisableUI) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    RemoveOverlaps: " + to_string(cVideo.bOR) + " (" + IntSizeToCE(sizeof(cVideo.bOR)) + " +" + GetAddress(cVideo.bOR) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    LimitFPS: " + to_string(m_pRenderer->GetLimitFPS()) + " (" + IntSizeToCE(sizeof(cVideo.bLimitFPS)) + " +" + GetAddress(cVideo.bLimitFPS) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    VelocityThreshold: " + to_string(cControls.iVelocityThreshold & 0x7F) + " (" + IntSizeToCE(sizeof(cControls.iVelocityThreshold)) + " +" + GetAddress(cControls.iVelocityThreshold) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Caption: \"" + string(strlen(CheatEngineCaption) < sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]) ? CaptionContent : "MAXIMUM LENGTH EXCEEDED! ") + "\" (String[" + to_string(sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]) - 1) + "] +" + GetAddress(CheatEngineCaption[1]) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    CaptionAlignment: '" + string(1, CheatEngineCaption[0]) + "' (String[1] +" + GetAddress(CheatEngineCaption) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ') << "\n";
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    DifficultyText: \"" + string(strlen(Difficulty) < sizeof(Difficulty) / sizeof(Difficulty[0]) ? Difficulty : "MAXIMUM LENGTH EXCEEDED! ") + "\" (String[" + to_string(sizeof(Difficulty) / sizeof(Difficulty[0])) + "] +" + GetAddress(Difficulty) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ');
        pos.X = 0;
        pos.Y = 0;
        SetConsoleCursorPosition(hConsole, pos);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        SMALL_RECT windowSize;
        windowSize.Left = 0;
        windowSize.Top = 0;
        windowSize.Right = csbi.dwSize.X - 1;
        windowSize.Bottom = line - 1;
        COORD bufferSize;
        bufferSize.X = csbi.dwSize.X;
        bufferSize.Y = line;
        SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    }
    FrameCount++;
}

void MainScreen::RenderMarker(const wstring & str) {
    m_pRenderer->AddText(str, (1 << 4) + (1 << 2), 5, 1, 0xFF404040, ALIGN_LEFT | ALIGN_TOP, -1, -1, 4, 0, 0x80000000);
    m_pRenderer->AddText(str, (1 << 4) + (1 << 2), 3, -1, 0xFFFFFFFF, ALIGN_LEFT | ALIGN_TOP);
}

void MainScreen::RenderMessage(LPRECT prcMsg, LPRECT prcScr, const wstring & sMsg, char lnAlign) {
    vector<wstring> Lines;
    {
        size_t s = 0;
        while (s <= sMsg.size()) {
            size_t e = sMsg.find(L'\n', s);
            if (e == wstring::npos) e = sMsg.size();
            Lines.push_back(sMsg.substr(s, e - s));
            s = e + 1;
        }
    }
    const win32_t RectW = (prcMsg->right - prcMsg->left + 3 * (prcScr->right - prcScr->left)) / 4;
    const win32_t RectH = (prcMsg->bottom - prcMsg->top + 3 * (prcScr->bottom - prcScr->top)) / 4;
    const win32_t CenterX = (prcMsg->left + prcMsg->right + 3 * (prcScr->left + prcScr->right)) / 8;
    const win32_t CenterY = (prcMsg->top + prcMsg->bottom + 3 * (prcScr->top + prcScr->bottom)) / 8;
    const win32_t PadX = 6, PadY = 3;
    auto MeasureBlock = [&](uint16_t Size, win32_t& W, win32_t& H) {
        W = 0; H = 0;
        for (auto& Line : Lines) {
            SIZE LineSize = m_pRenderer->CalcTextSize(Line.empty() ? L" " : Line, Size);
            W = max(W, LineSize.cx);
            H += LineSize.cy;
        }
    };
    uint16_t fontsize = (1 << 5) - (1 << 2);
    win32_t BlockW, BlockH;
    MeasureBlock(fontsize, BlockW, BlockH);
    while ((BlockW >= RectW || BlockH >= RectH) && fontsize > (1 << 3)) {
        fontsize--;
        MeasureBlock(fontsize, BlockW, BlockH);
    }
    m_pRenderer->AddGDIRect(CenterX - BlockW / 2 - PadX, CenterY - BlockH / 2 - PadY, BlockW + 2 * PadX, BlockH + 2 * PadY, 0x80000000);
    win32_t CurX = CenterX;
    win32_t CurY = CenterY - BlockH / 2;
    DWORD Alignment = ALIGN_CENTER | ALIGN_TOP;
    if (lnAlign == 'C') {
        CurX = CenterX;
        Alignment = ALIGN_CENTER | ALIGN_TOP;
    }
    if (lnAlign == 'L') {
        CurX = CenterX - BlockW / 2;
        Alignment = ALIGN_LEFT | ALIGN_TOP;
    }
    if (lnAlign == 'R') {
        CurX = CenterX + BlockW / 2;
        Alignment = ALIGN_RIGHT | ALIGN_TOP;
    }
    for (auto& Line : Lines) {
        SIZE LineSize = m_pRenderer->CalcTextSize(Line.empty() ? L" " : Line, fontsize);
        if (!Line.empty()) {
            m_pRenderer->AddText(Line, fontsize, CurX + 2, CurY + 1, 0xFF404040, Alignment);
            m_pRenderer->AddText(Line, fontsize, CurX, CurY, 0xFFFFFFFF, Alignment);
        }
        CurY += LineSize.cy;
    }
}