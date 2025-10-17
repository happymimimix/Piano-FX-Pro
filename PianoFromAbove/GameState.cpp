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
#include <ppl.h>
#include <dwmapi.h>
#include <d3d9types.h>
#include <numeric>

#include "Globals.h"
#include "GameState.h"
#include "Config.h"
#include "resource.h"
#include "Language.h"
#include "ConfigProcs.h"
#include "lzma.h"

const wstring GameState::Errors[] =
{
    L"Success.",
    L"Invalid pointer passed. ",
    L"Error calling DirectX. "
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
        (*pDestObj)->Init();
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
        int iId = LOWORD(wParam);
        switch (iId)
        {
        case ID_CHANGESTATE:
            m_pNextState = reinterpret_cast<GameState*>(lParam);
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            m_pRenderer->ResetDevice();
            return Success;
        }
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
    unsigned int iColor = Config::GetConfig().GetVisualSettings().iBkgColor;
    int R = (iColor >> 0) & 0xFF;
    int G = (iColor >> 8) & 0xFF;
    int B = (iColor >> 16) & 0xFF;
    m_pRenderer->ClearAndBeginScene(D3DCOLOR_XRGB(R, G, B));

    // Clear out the ImGui draw list
    m_pRenderer->BeginText();
    m_pRenderer->EndText();

    // Present the backbuffer contents to the display
    m_pRenderer->EndScene();
    m_pRenderer->Present();

    // Get the current frame
    auto* frame = m_pRenderer->Screenshot();
    //Don't let BitBlt capture a blank screen! 
    HDC hdcGFX = GetDC(g_hWndGfx);
    HDC hdcMem = CreateCompatibleDC(hdcGFX);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcGFX, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight());
    SelectObject(hdcMem, hBitmap);
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = m_pRenderer->GetBufferWidth();
    bmpInfo.bmiHeader.biHeight = -m_pRenderer->GetBufferHeight();
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    SetDIBits(hdcMem, hBitmap, 0, m_pRenderer->GetBufferHeight(), frame, &bmpInfo, DIB_RGB_COLORS);
    BitBlt(hdcGFX, 0, 0, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight(), hdcMem, 0, 0, SRCCOPY);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(g_hWndGfx, hdcGFX);

    return Success;
}

//-----------------------------------------------------------------------------
// SplashScreen GameState object
//-----------------------------------------------------------------------------

SplashScreen::SplashScreen(HWND hWnd, D3D12Renderer* pRenderer, bool enableSplash) : GameState(hWnd, pRenderer) {
    if (enableSplash) {
        HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_SPLASHMIDI), TEXT("MIDI"));
        HGLOBAL hRes = LoadResource(NULL, hResInfo);
        int iSize = SizeofResource(NULL, hResInfo);
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
        m_MIDI.PostProcess(m_vEvents);

        // Allocate
        m_vTrackSettings.resize(m_MIDI.GetInfo().iNumTracks);
        for (int i = 0; i < 128; i++)
            m_vState[i].reserve(128);
    }
    InitState();
    UpdateNotePos = true;
}

void SplashScreen::InitNotes(const vector< MIDIEvent* >& vEvents) {
    //Get only the channel events
    m_vEvents.reserve(vEvents.size());
    for (vector< MIDIEvent* >::const_iterator it = vEvents.begin(); it != vEvents.end(); ++it)
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

    SetChannelSettings(vector< bool >(), vector< bool >(),
        vector< unsigned >(cVisual.colors, cVisual.colors + sizeof(cVisual.colors) / sizeof(cVisual.colors[0])));

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

void SplashScreen::ColorChannel(int iTrack, int iChannel, unsigned int iColor, bool bRandom) {
    if (bRandom)
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor();
    else
        m_vTrackSettings[iTrack].aChannels[iChannel].SetColor(iColor);
}

void SplashScreen::SetChannelSettings(const vector< bool >&, const vector< bool >&, const vector< unsigned >& vColor) {
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    const vector< MIDITrack* >& vTracks = m_MIDI.GetTracks();

    static Config& config = Config::GetConfig();
    static const VisualSettings& cVisual = config.GetVisualSettings();

    int iPos = 0;
    for (uint16_t i = 0; i < mInfo.iNumTracks; i++)
    {
        const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
        for (int j = 0; j < 16; j++)
            if (mTrackInfo.aNoteCount[j] > 0)
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

GameState::GameError SplashScreen::MsgProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam) {
    static Config& config = Config::GetConfig();
    static const AudioSettings& cAudio = config.GetAudioSettings();

    switch (msg)
    {
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        switch (iId)
        {
        case ID_CHANGESTATE:
            m_pNextState = reinterpret_cast<GameState*>(lParam);
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            m_pRenderer->ResetDevice();
            return Success;
        }
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
    // Start new ImGui frame
    m_pRenderer->ImGuiStartFrame();

    static Config& config = Config::GetConfig();
    static PlaybackSettings& cPlayback = config.GetPlaybackSettings();

    // Set the state
    m_bMute = cPlayback.GetMute();
    m_dVolume = cPlayback.GetVolume();

    // Time stuff
    long long llElapsed = m_Timer.GetMicroSecs();
    m_Timer.Start();

    // Figure out start and end times for display
    m_llStartTime = m_llStartTime + llElapsed;
    if (m_llStartTime > m_MIDI.GetInfo().llTotalMicroSecs + 500000) {
        m_llStartTime = m_MIDI.GetInfo().llFirstNote - 1000000;
        m_iStartPos = 0;
        m_iEndPos = -1;
    }
    long long llEndTime = m_llStartTime + TimeSpan;

    // Needs start time to be set. For creating textparticles.
    RenderGlobals();

    // Advance end position
    size_t iEventCount = m_vEvents.size();
    while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() < llEndTime)
        m_iEndPos++;

    // Advance start position updating initial state as we pass stale events
    // Also PLAYS THE MUSIC
    while (m_iStartPos < iEventCount && m_vEvents[m_iStartPos]->GetAbsMicroSec() <= m_llStartTime)
    {
        MIDIChannelEvent* pEvent = m_vEvents[m_iStartPos];
        if (pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOn && pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOff) {
            m_OutDevice.PlayEvent(pEvent->GetEventCode(), pEvent->GetParam1(), pEvent->GetParam2());
        }
        else if (!m_bMute && !m_vTrackSettings[pEvent->GetTrack()].aChannels[pEvent->GetChannel()].bMuted) {
            if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() > 0) {
                m_OutDevice.PlayEvent(pEvent->GetEventCode(), pEvent->GetParam1(), static_cast<unsigned char>(m_dVolume > 1.0 ? static_cast<double>(INT8_MAX) - (static_cast<double>(INT8_MAX) - static_cast<double>(pEvent->GetParam2())) * (2.0 - m_dVolume) : static_cast<double>(pEvent->GetParam2()) * m_dVolume));
            }
            else {
                m_OutDevice.PlayEvent((pEvent->GetEventCode() & 0x0F) | (MIDIChannelEvent::NoteOn << 4), pEvent->GetParam1(), 0x00);
            }
        }
        UpdateState(m_iStartPos);
        m_iStartPos++;
    }

    return Success;
}

// https://github.com/WojciechMula/simd-search/blob/master/sse-binsearch-block.cpp
int sse_bin_search(const std::vector<int>& data, int key) {

    const __m128i keys = _mm_set1_epi32(key);
    __m128i v;

    int limit = data.size() - 1;
    int a = 0;
    int b = limit;

    while (a <= b) {
        const int c = (a + b) / 2;

        if (data[c] == key) {
            return c;
        }

        if (key < data[c]) {
            b = c - 1;

            if (b >= 4) {
                v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[b - 4]));
                v = _mm_cmpeq_epi32(v, keys);
                const uint16_t mask = _mm_movemask_epi8(v);
                if (mask) {
                    return b - 4 + _tzcnt_u32(mask) / 4;
                }
            }
        }
        else {
            a = c + 1;

            if (a + 4 < limit) {
                v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[a]));
                v = _mm_cmpeq_epi32(v, keys);
                const uint16_t mask = _mm_movemask_epi8(v);
                if (mask) {
                    return a + _tzcnt_u32(mask) / 4;
                }
            }
        }
    }

    return -1;
}

void SplashScreen::UpdateState(int iPos) {
    // Event data
    MIDIChannelEvent* pEvent = m_vEvents[iPos];
    if (!pEvent->HasSister()) return;

    MIDIChannelEvent::ChannelEventType eEventType = pEvent->GetChannelEventType();
    int iNote = pEvent->GetParam1();
    int iVelocity = pEvent->GetParam2();

    int iSisterIdx = pEvent->GetSisterIdx();
    auto& note_state = m_vState[iNote];

    // Turn note on
    if (eEventType == MIDIChannelEvent::NoteOn && iVelocity > 0)
        note_state.push_back(iPos);
    else
    {
        // binary search
        auto pos = sse_bin_search(note_state, iSisterIdx);
        if (pos != -1)
            note_state.erase(note_state.begin() + pos);
    }
}

const float SplashScreen::SharpRatio = 0.65f;

GameState::GameError SplashScreen::Render() {
    if (FAILED(m_pRenderer->ResetDeviceIfNeeded())) return DirectXError;

    // Clear the backbuffer to a blue color
    unsigned int iColor = Config::GetConfig().GetVisualSettings().iBkgColor;
    int R = (iColor >> 0) & 0xFF;
    int G = (iColor >> 8) & 0xFF;
    int B = (iColor >> 16) & 0xFF;
    m_pRenderer->ClearAndBeginScene(D3DCOLOR_XRGB(R, G, B));
    RenderNotes();

    // Present the backbuffer contents to the display
    m_pRenderer->EndSplashScene();
    m_pRenderer->Present();

    // Get the current frame
    auto* frame = m_pRenderer->Screenshot();
    //Don't let BitBlt capture a blank screen! 
    HDC hdcGFX = GetDC(g_hWndGfx);
    HDC hdcMem = CreateCompatibleDC(hdcGFX);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcGFX, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight());
    SelectObject(hdcMem, hBitmap);
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = m_pRenderer->GetBufferWidth();
    bmpInfo.bmiHeader.biHeight = -m_pRenderer->GetBufferHeight();
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    SetDIBits(hdcMem, hBitmap, 0, m_pRenderer->GetBufferHeight(), frame, &bmpInfo, DIB_RGB_COLORS);
    BitBlt(hdcGFX, 0, 0, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight(), hdcMem, 0, 0, SRCCOPY);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(g_hWndGfx, hdcGFX);

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
    long long llMicroSecsPP = static_cast<long long>(TimeSpan / m_fNotesCY + 0.5f);
    m_llRndStartTime = m_llStartTime - (m_llStartTime < 0 ? llMicroSecsPP : 0);
    m_llRndStartTime = (m_llRndStartTime / llMicroSecsPP) * llMicroSecsPP;

    if (UpdateNotePos) GenNoteXTable();
}

void SplashScreen::RenderNotes() {
    // Do we have any notes to render?
    if (m_iEndPos < 0 || m_iStartPos >= static_cast<int>(m_vEvents.size()))
        return;

    for (int i = m_iStartNote; i <= m_iEndNote; i++) {
        if (!MIDI::IsSharp(i)) {
            for (vector< int >::iterator it = (m_vState[i]).begin(); it != (m_vState[i]).end(); it++) {
                RenderNote(m_vEvents[*it]);
            }
        }
    }
    for (int i = m_iStartPos; i <= m_iEndPos; i++) {
        MIDIChannelEvent* pEvent = m_vEvents[i];
        if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn &&
            pEvent->GetParam2() > 0 && pEvent->HasSister() &&
            !MIDI::IsSharp(pEvent->GetParam1()) &&
            m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
            RenderNote(pEvent);
        }
    }
    for (int i = m_iStartNote; i <= m_iEndNote; i++) {
        if (MIDI::IsSharp(i)) {
            for (vector< int >::iterator it = (m_vState[i]).begin(); it != (m_vState[i]).end(); it++) {
                RenderNote(m_vEvents[*it]);
            }
        }
    }
    for (int i = m_iStartPos; i <= m_iEndPos; i++) {
        MIDIChannelEvent* pEvent = m_vEvents[i];
        if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn &&
            pEvent->GetParam2() > 0 && pEvent->HasSister() &&
            MIDI::IsSharp(pEvent->GetParam1()) &&
            m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
            RenderNote(pEvent);
        }
    }
}

void SplashScreen::RenderNote(MIDIChannelEvent* pNote) {
    int iNote = pNote->GetParam1();
    int iTrack = pNote->GetTrack();
    int iChannel = pNote->GetChannel();
    long long llNoteStart = pNote->GetAbsMicroSec();
    long long llNoteEnd = llNoteStart + pNote->GetLength();

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
    int iAlpha1 = static_cast<int>(0xFF * (m_fNotesCY - y) / m_fNotesCY);
    iAlpha1 = max(iAlpha1, 0);
    int iAlpha2 = min(iAlpha1, 0xFF);
    iAlpha1 <<= 24;
    iAlpha2 <<= 24;
    m_pRenderer->DrawRect(x, y - cy, cx, cy, csTrack.iVeryDarkRGB | iAlpha1);
    m_pRenderer->DrawRect(x + fDeflate, y - cy + fDeflate,
        cx - fDeflate * 2.0f, cy - fDeflate * 2.0f,
        csTrack.iPrimaryRGB | iAlpha1, csTrack.iDarkRGB | iAlpha1, csTrack.iDarkRGB | iAlpha2, csTrack.iPrimaryRGB | iAlpha2);
}

void SplashScreen::GenNoteXTable() {
    UpdateNotePos = false;
    for (int i = m_iStartNote; i <= m_iEndNote; i++) {
        int iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
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

float SplashScreen::GetNoteX(int iNote) {
    return notex_table[iNote];
}

//-----------------------------------------------------------------------------
// MainScreen GameState object
//-----------------------------------------------------------------------------

string GetExePath(void) {
    char szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

MainScreen::MainScreen(wstring sMIDIFile, State eGameMode, HWND hWnd, D3D12Renderer* pRenderer) :
    GameState(hWnd, pRenderer), m_MIDI(sMIDIFile), m_eGameMode(eGameMode) {
    // Finish off midi processing
    if (!m_MIDI.IsValid()) return;
    m_MIDI.ConnectNotes(); // Order's important here
    m_vEvents.reserve(m_MIDI.GetInfo().iEventCount);

    // Allocate
    m_vTrackSettings.resize(m_MIDI.GetInfo().iNumTracks);
    for (auto note_state : m_vState)
        note_state.reserve(m_MIDI.GetInfo().iNumTracks * 16);

    m_MIDI.PostProcess(m_vEvents, &m_vMetaEvents, &m_vTempo, &m_vSignature, &m_vMarkers, &m_vColors);

    g_LoadingProgress.stage = MIDILoadingProgress::Stage::NCTable;
    g_LoadingProgress.progress = 0;
    g_LoadingProgress.max = m_vEvents.size();
    if (m_vNCTable) {
        delete[] m_vNCTable;
        m_vNCTable = nullptr;
    }
    long long iMaxMS = m_MIDI.GetInfo().llTotalMicroSecs / MS;
    m_vNCTable = new long[iMaxMS + static_cast<signed long long>(1LL)]();
    int iNC = 0;
    long long iLastMS = -1;
    for (auto pEvent : m_vEvents) {
        if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() > 0) {
            iNC++;
            if (m_vNCTable) {
                long long iThisMS = pEvent->GetAbsMicroSec() / MS;
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
    m_csKBBackground.SetColor(0x00999999, 0.4f, 0.0f);
    m_csKBRed.SetColor(cVisual.iBarColor, 0.5f);
    m_csKBWhite.SetColor(0x00FFFFFF, 0.8f, 0.6f);
    m_csKBSharp.SetColor(0x00404040, 0.5f, 0.0f);
}

// Init state vars. Only those which validate the date.
void MainScreen::InitState() {
    static Config& config = Config::GetConfig();
    static const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    static const ViewSettings& cView = config.GetViewSettings();
    static const VideoSettings& cVideo = config.GetVideoSettings();

    m_eGameMode = Practice;
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
    m_llTimeSpan = static_cast<long long>(3.0 * abs(m_dNSpeed) * 1000000);
    IsLastFrameReversed = m_dSpeed < 0;
    IsReversedStateInitialized = false;

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

    m_bDumpFrames = cVideo.bDumpFrames;
    if (m_bDumpFrames) {
        RECT rect = {};
        GetWindowRect(g_hWndGfx, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        //Running ffmpeg
        char buf[1 << 10] = {};
        snprintf(buf, sizeof(buf), "cd /d \"%s\" && md \"%s\\PianoFX_Framedump\" & start cmd /k ffmpeg -r 60 -f rawvideo -s %dx%d -pix_fmt bgra -i async:\\\\.\\pipe\\PFXdump -c:v h264 -qp 1 -pix_fmt yuv420p \"%s\\PianoFX_Framedump\\Output.mp4\"", GetExePath().c_str(), GetExePath().c_str(), width, height, GetExePath().c_str());
        m_hVideoPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\PFXdump"), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, static_cast<DWORD>(width * height * 4 * 120), 0, 0, nullptr);
        system(buf);
        ConnectNamedPipe(m_hVideoPipe, NULL);
    }

    memset(m_pNoteState, -1, sizeof(m_pNoteState));

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

    for (auto& work : m_vThreadWork)
        work.reserve(1 << 16);

    return Success;
}

void MainScreen::ColorChannel(int iTrack, int iChannel, unsigned int iColor, bool bRandom) {
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
void ChannelSettings::SetColor(unsigned int iColor, double dDark, double dVeryDark) {
    int R = (iColor >> 0) & 0xFF, dR, vdR;
    int G = (iColor >> 8) & 0xFF, dG, vdG;
    int B = (iColor >> 16) & 0xFF, dB, vdB;
    int A = (iColor >> 24) & 0xFF;

    int H, S, V;
    Util::RGBtoHSV(R, G, B, H, S, V);
    Util::HSVtoRGB(H, S, min(100, static_cast<int>(V * dDark)), dR, dG, dB);
    Util::HSVtoRGB(H, S, min(100, static_cast<int>(V * dVeryDark)), vdR, vdG, vdB);

    this->iOrigBGR = iColor;
    this->iPrimaryRGB = (A << 24) | (R << 16) | (G << 8) | (B << 0);
    this->iDarkRGB = (A << 24) | (dR << 16) | (dG << 8) | (dB << 0);
    this->iVeryDarkRGB = (A << 24) | (vdR << 16) | (vdG << 8) | (vdB << 0);
}

ChannelSettings* MainScreen::GetChannelSettings(int iTrack) {
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    const vector< MIDITrack* >& vTracks = m_MIDI.GetTracks();

    int iPos = 0;
    for (uint16_t i = 0; i < mInfo.iNumTracks; i++)
    {
        const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
        for (uint8_t j = 0; j < 16; j++)
            if (mTrackInfo.aNoteCount[j] > 0)
            {
                if (iPos == iTrack) return &m_vTrackSettings[i].aChannels[j];
                iPos++;
            }
    }
    return NULL;
}

void MainScreen::SetChannelSettings(const vector< bool >& vMuted, const vector< bool >& vHidden, const vector< unsigned >& vColor) {
    const vector< MIDITrack* >& vTracks = m_MIDI.GetTracks();

    bool bMuted = vMuted.size() > 0;
    bool bHidden = vHidden.size() > 0;
    bool bColor = vColor.size() > 0;

    size_t iPos = 0;
    for (int i = 0; i < (int)vTracks.size(); i++) {
        const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
        for (int j = 0; j < 16; j++) {
            if (mTrackInfo.aNoteCount[j] > 0) {
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
        int iId = LOWORD(wParam);
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
            JumpTo(static_cast<long long>(m_llStartTime + cControls.dFwdBackSecs * 1000000));
            return Success;
        case ID_PLAY_SKIPBACK:
            JumpTo(static_cast<long long>(m_llStartTime - cControls.dFwdBackSecs * 1000000));
            return Success;
        case ID_VIEW_RESETDEVICE:
            UpdateNotePos = true;
            m_pRenderer->ResetDevice();
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
        long long llFirstTime = GetMinTime();
        long long llLastTime = GetMaxTime();
        bool StartTimer = (JumpTarget == ~0);
        JumpTarget = llFirstTime + ((llLastTime - llFirstTime) * lParam) / 1000;
        if (StartTimer) SetTimer(g_hWnd, IDC_POSNDELAY, nxtdelay, NULL); //Async JumpTo process
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (m_bZoomMove)
        {
            m_ptLastPos.x = (SHORT)LOWORD(lParam);
            m_ptLastPos.y = (SHORT)HIWORD(lParam);
            m_bTrackPos = true;
        }
        return Success;
    }
    case WM_RBUTTONDOWN:
    {
        if (!m_bZoomMove) return Success;
        m_ptLastPos.x = (SHORT)LOWORD(lParam);
        m_ptLastPos.y = (SHORT)HIWORD(lParam);
        m_ptStartZoom.x = static_cast<int>((m_ptLastPos.x - m_fOffsetX - m_fTempOffsetX) / (m_fZoomX * m_fTempZoomX));
        m_ptStartZoom.y = static_cast<int>(m_ptLastPos.y - m_fOffsetY - m_fTempOffsetY);
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
        short x = LOWORD(lParam);
        short y = HIWORD(lParam);
        short dx = static_cast<short>(x - m_ptLastPos.x);
        short dy = static_cast<short>(y - m_ptLastPos.y);

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
                UINT end = GetTickCount() + (1<<2);
                nxtdelay = max(1 << 2, min(end - start, 1 << 10));
            }
        }
        return Success;
    }
    }

    return Success;
}

GameState::GameError MainScreen::Logic(void) {
    // Start new ImGui frame
    m_pRenderer->ImGuiStartFrame();

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
    long long llTimeSpan = static_cast<long long>(3.0 * abs(dNSpeed) * 1000000);
    bool bPausedChanged = (bPaused != m_bPaused);
    bool bMuteChanged = (bMute != m_bMute);
    uint8_t velthrshld = cControls.iVelocityThreshold % (1 << 7);

    // Set the state
    m_bTickMode = cVideo.bTickBased;
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
    m_iStartNote = max(0, min(127, min(cVisual.iFirstKey, cVisual.iLastKey)));
    m_iEndNote = max(0, min(127, max(cVisual.iFirstKey, cVisual.iLastKey)));
    m_bFlipKeyboard = cVisual.iFirstKey > cVisual.iLastKey && cVisual.eKeysShown != 0 && cVisual.eKeysShown != 1;
    m_pRenderer->SetLimitFPS(cVideo.bLimitFPS);
    if (cVisual.iBkgColor != m_csBackground.iOrigBGR) m_csBackground.SetColor(cVisual.iBkgColor, 0.7f, 1.3f);

    if (cVideo.eMarkerEncoding != m_iCurEncoding) {
        m_iCurEncoding = cVideo.eMarkerEncoding;
        ApplyMarker(m_pMarkerData, m_iMarkerSize);
    }

    // Time stuff
    long long llMaxTime = GetMaxTime();
    long long llMinTime = GetMinTime();
    long long llElapsed = m_Timer.GetMicroSecs();
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
    long long llOldStartTime = m_llStartTime;
    long long llNextStartTime = m_llStartTime + static_cast<long long>(llElapsed * m_dSpeed + 0.5);

    if (!m_bPaused) m_llStartTime = llNextStartTime;
    m_iStartTick = GetCurrentTick(m_llStartTime);
    long long llEndTime = 0;
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

    long long iEventCount = m_vEvents.size();
    RenderGlobals();

    // Advance end position
    if (dNSpeed >= 0) {
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
    }

    // Only want to advance start positions when unpaused becuase advancing startpos "consumes" the events
    if (!m_bPaused)
    {
        // Advance start position updating initial state as we pass stale events
        // Also PLAYS THE MUSIC
        for (auto& work : m_vThreadWork) work.clear();
        if (m_dSpeed < 0) {
            if (!IsLastFrameReversed) m_iStartPos--;
            IsLastFrameReversed = true;
            while (m_iStartPos > 0 && m_vEvents[m_iStartPos]->GetAbsMicroSec() >= m_llStartTime)
            {
                MIDIChannelEvent* pEvent = m_vEvents[m_iStartPos];
                MIDIChannelEvent* pOrigEvent = m_vEvents[m_iStartPos];
                if ((pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn || pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOff) && pEvent->HasSister()) {
                    pEvent = pEvent->GetSister(m_vEvents);
                }
                MIDIChannelEvent* pChannelEvent = reinterpret_cast<MIDIChannelEvent*>(pEvent);
                unsigned char key = pEvent->GetParam1();
                unsigned char vel = pEvent->GetParam2();
                unsigned char keyog = pOrigEvent->GetParam1();
                unsigned char velog = pOrigEvent->GetParam2();

                if (pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOn && pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOff) {
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::ProgramChange && config.m_bPianoOverride) {
                        pEvent->SetParam1(0);
                    }
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::PitchBend) {
                        m_pBendsValue[pEvent->GetChannel()] = ((vel << 7) | key) - (1 << 13);
                        float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
                        float ShiftAmount = m_pBendsRange[pEvent->GetChannel()] == 0 ? 0 : m_pBendsValue[pEvent->GetChannel()] / ((1 << 13) / m_pBendsRange[pEvent->GetChannel()]);
                        if (m_bFlipKeyboard) ShiftAmount *= -1;
                        m_pBends[pEvent->GetChannel()] = NoteWidth * ShiftAmount;
                    }
                    if (pChannelEvent->GetChannelEventType() == MIDIChannelEvent::Controller) {
                        if (key == 100) { Next_is_PBS[pEvent->GetChannel()] = (vel == 0); }//[101] RPN LSB | 0:Pitch Bend Sensitivity
                        if (key == 6 && Next_is_PBS[pEvent->GetChannel()]) {
                            m_pBendsRange[pEvent->GetChannel()] = vel;
                            float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
                            float ShiftAmount = m_pBendsRange[pEvent->GetChannel()] == 0 ? 0 : m_pBendsValue[pEvent->GetChannel()] / ((1 << 13) / m_pBendsRange[pEvent->GetChannel()]);
                            if (m_bFlipKeyboard) ShiftAmount *= -1;
                            m_pBends[pEvent->GetChannel()] = NoteWidth * ShiftAmount;
                        }
                    }
                    m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, vel);
                }
                else if (!m_bMute && !m_vTrackSettings[pEvent->GetTrack()].aChannels[pEvent->GetChannel()].bMuted) {
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && vel > 0) {
                        if (vel > velthrshld) m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, static_cast<unsigned char>(m_dVolume > 1.0 ? static_cast<double>(INT8_MAX) - (static_cast<double>(INT8_MAX) - static_cast<double>(vel)) * (2.0 - m_dVolume) : static_cast<double>(vel) * m_dVolume));
                    }
                    else {
                        m_OutDevice.PlayEvent((pEvent->GetEventCode() & 0x0F) | (MIDIChannelEvent::NoteOn << 4), key, 0x00);
                    }
                }
                if ((pOrigEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn || pOrigEvent->GetChannelEventType() == MIDIChannelEvent::NoteOff) && pOrigEvent->HasSister())
                {
                    m_vThreadWork[keyog].push_back({
                        .idx = pOrigEvent->GetSisterIdx(),
                        .sister_idx = (pOrigEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && velog > 0) ? (unsigned)m_iStartPos : ~0,
                        });
                }
                m_iStartPos--;
            }
            concurrency::parallel_for(size_t(0), size_t(128), [&](int key) {
                for (const auto& work : m_vThreadWork[key])
                    UpdateStateBackwards(key, work);
                });
        }
        else {
            if (IsLastFrameReversed) m_iStartPos++;
            IsLastFrameReversed = false;
            while (m_iStartPos < iEventCount && m_vEvents[m_iStartPos]->GetAbsMicroSec() <= m_llStartTime)
            {
                MIDIChannelEvent* pEvent = m_vEvents[m_iStartPos];
                MIDIChannelEvent* pChannelEvent = reinterpret_cast<MIDIChannelEvent*>(pEvent);
                unsigned char key = pEvent->GetParam1();
                unsigned char vel = pEvent->GetParam2();

                if (pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOn && pEvent->GetChannelEventType() != MIDIChannelEvent::NoteOff) {
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::ProgramChange && config.m_bPianoOverride) {
                        pEvent->SetParam1(0);
                    }
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::PitchBend) {
                        m_pBendsValue[pEvent->GetChannel()] = ((vel << 7) | key) - (1 << 13);
                        float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
                        float ShiftAmount = m_pBendsRange[pEvent->GetChannel()] == 0 ? 0 : m_pBendsValue[pEvent->GetChannel()] / ((1 << 13) / m_pBendsRange[pEvent->GetChannel()]);
                        if (m_bFlipKeyboard) ShiftAmount *= -1;
                        m_pBends[pEvent->GetChannel()] = NoteWidth * ShiftAmount;
                    }
                    if (pChannelEvent->GetChannelEventType() == MIDIChannelEvent::Controller) {
                        if (key == 100) { Next_is_PBS[pEvent->GetChannel()] = (vel == 0); }//[101] RPN LSB | 0:Pitch Bend Sensitivity
                        if (key == 6 && Next_is_PBS[pEvent->GetChannel()]) {
                            m_pBendsRange[pEvent->GetChannel()] = vel;
                            float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
                            float ShiftAmount = m_pBendsRange[pEvent->GetChannel()] == 0 ? 0 : m_pBendsValue[pEvent->GetChannel()] / ((1 << 13) / m_pBendsRange[pEvent->GetChannel()]);
                            if (m_bFlipKeyboard) ShiftAmount *= -1;
                            m_pBends[pEvent->GetChannel()] = NoteWidth * ShiftAmount;
                        }
                    }
                    m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, vel);
                }
                else if (!m_bMute && !m_vTrackSettings[pEvent->GetTrack()].aChannels[pEvent->GetChannel()].bMuted) {
                    if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && vel > 0) {
                        if (vel > velthrshld) m_OutDevice.PlayEvent(pEvent->GetEventCode(), key, static_cast<unsigned char>(m_dVolume > 1.0 ? static_cast<double>(INT8_MAX) - (static_cast<double>(INT8_MAX) - static_cast<double>(vel)) * (2.0 - m_dVolume) : static_cast<double>(vel) * m_dVolume));
                    }
                    else {
                        m_OutDevice.PlayEvent((pEvent->GetEventCode() & 0x0F) | (MIDIChannelEvent::NoteOn << 4), key, 0x00);
                    }
                }
                if ((pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn || pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOff) && pEvent->HasSister())
                {
                    m_vThreadWork[key].push_back({
                        .idx = (unsigned)m_iStartPos,
                        .sister_idx = (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && vel > 0) ? ~0 : pEvent->GetSisterIdx(),
                        });
                }
                m_iStartPos++;
            }
            concurrency::parallel_for(size_t(0), size_t(128), [&](int key) {
                for (const auto& work : m_vThreadWork[key])
                    UpdateState(key, work);
                });
        }
    }

    // Advance end position AFTER advancing start for negative note speed
    if (dNSpeed < 0) {
        m_iEndPos += (m_iPrevStartPos - m_iEndPos) * 2;
        m_iEndPos = max(m_iEndPos, 0);
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
        m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
        m_iPrevStartPos = m_iStartPos;
    }

    AdvanceIterators(m_llStartTime, false);

    // Update the position slider
    long long llFirstTime = GetMinTime();
    long long llLastTime = GetMaxTime();
    long long llOldPos = ((llOldStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime);
    long long llNewPos = ((m_llStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime);
    if (llOldPos != llNewPos) cPlayback.SetPosition(static_cast<int>(llNewPos));

    // Song's over
    if (!m_bPaused && ((m_dSpeed < 0) ? (m_llStartTime < llMinTime) : (m_llStartTime > llMaxTime))) {
        if (m_bDumpFrames) {
            char buf[1 << 10] = {};
            snprintf(buf, sizeof(buf), "start \"Result\" \"C:\\Windows\\Explorer.exe\" \"%s\\PianoFX_Framedump\\\"", GetExePath().c_str());
            system(buf);
            CloseHandle(m_hVideoPipe);
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
    for (size_t i = 0; i < m_vTrackSettings.size(); i++) {
        for (size_t j = 0; j < 16; j++) {
            auto& src = m_vTrackSettings[i].aChannels[j];
            auto& dst = track_colors[i * 16 + j];
            dst.primary = src.iPrimaryRGB;
            dst.dark = src.iDarkRGB;
            dst.darker = src.bHidden ? 0xFFFFFFFF : src.iVeryDarkRGB; // Hack to signal hidden track without checking on CPU
        }
    }

    return Success;
}

void MainScreen::UpdateState(int key, const thread_work_t& work) {
    auto& note_state = m_vState[key];
    if (work.sister_idx == UINT32_MAX) {
        note_state.push_back(work.idx);
        m_pNoteState[key] = work.idx;
    }
    else {
        // binary search
        auto pos = sse_bin_search(note_state, work.sister_idx);
        if (pos != -1)
            note_state.erase(note_state.begin() + pos);

        if (note_state.size() == 0)
            m_pNoteState[key] = -1;
        else
            m_pNoteState[key] = note_state.back();
    }
}

void InsertSorted(std::vector<int>& vec, int value) {
    auto it = std::lower_bound(vec.begin(), vec.end(), value);
    vec.insert(it, value);
}

void MainScreen::UpdateStateBackwards(int key, const thread_work_t& work) {
    auto& note_state = m_vState[key];
    if (work.sister_idx == UINT32_MAX) {
        InsertSorted(note_state, work.idx);
        m_pNoteState[key] = work.idx;
    }
    else {
        // binary search
        auto pos = sse_bin_search(note_state, work.sister_idx);
        if (pos != -1)
            note_state.erase(note_state.begin() + pos);

        if (note_state.size() == 0)
            m_pNoteState[key] = -1;
        else
            m_pNoteState[key] = note_state.back();
    }
}

void MainScreen::JumpTo(long long llStartTime, boolean loadingMode) {
    // Kill the music!
    if (!loadingMode) m_OutDevice.AllNotesOff();

    // Start time. Piece of cake!
    long long llFirstTime;
    long long llLastTime;
    long long llEndTime;
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

    // Start position and current state: hard!
    auto itBegin = m_vEvents.begin();
    auto itEnd = m_vEvents.end();
    // Want lower bound to minimize simultaneous complexity
    auto itMiddle = lower_bound(itBegin, itEnd, llStartTime, [&](MIDIChannelEvent* lhs, const long long rhs) {
        return lhs->GetAbsMicroSec() < rhs;
        });

    // Start position
    m_iStartPos = (long long)m_vEvents.size();
    if (itMiddle != itEnd && itMiddle - m_vEvents.begin() < m_iStartPos)
        m_iStartPos = itMiddle - m_vEvents.begin();

    // Need to scan up to the next note on event
    for (; itMiddle != itEnd; itMiddle++) {
        if ((*itMiddle)->GetChannelEventType() == MIDIChannelEvent::NoteOn && (*itMiddle)->GetParam2() > 0)
            break;
    }

    // Find the notes that occur simultaneously with the previous note on
    for (auto& note_state : m_vState)
        note_state.clear();
    memset(m_pNoteState, -1, sizeof(m_pNoteState));

    if (itMiddle != itBegin)//SLOWEST SECTION!!! 
    {
        // Need to scan down to the last note on event
        auto itPrev = itMiddle - 1;
        for (; itPrev != itBegin; itPrev--) {
            if ((*itPrev)->GetChannelEventType() == MIDIChannelEvent::NoteOn && (*itPrev)->GetParam2() > 0)
                break;
        }

        unsigned iFound = 0;
        unsigned iSimultaneous = (*itPrev)->GetSimultaneous() + 1;
        for (std::vector<MIDIChannelEvent*>::reverse_iterator it(itMiddle); iFound < iSimultaneous && it != m_vEvents.rend(); ++it)
        {
            auto idx = m_vEvents.size() - 1 - (it - m_vEvents.rbegin());
            MIDIChannelEvent* pEvent = m_vEvents[idx];
            if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() > 0 && pEvent->HasSister()) {
                MIDIChannelEvent* pSister = pEvent->GetSister(m_vEvents);
                if (pSister->GetAbsMicroSec() > (*itPrev)->GetAbsMicroSec()) // > because itMiddle is the max for its time
                    iFound++;
                if (pSister->GetAbsMicroSec() > llStartTime) // > because we don't care about simultaneous ending notes
                {
                    (m_vState[pEvent->GetParam1()]).push_back(idx);
                    if (m_pNoteState[pEvent->GetParam1()] < 0)
                        m_pNoteState[pEvent->GetParam1()] = idx;
                }
            }
        }
        for (auto& note_state : m_vState)
            reverse(note_state.begin(), note_state.end());
    }

    AdvanceIterators(llStartTime, true);
    m_iStartTick = GetCurrentTick(m_llStartTime);

    // End position: a little tricky. Same as logic code. Only needed for paused jumping.
    if (m_dNSpeed < 0) {
        if (m_bTickMode) {
            m_iEndPos = m_iStartPos + 1;
            auto iEventCount = (long long)m_vEvents.size();
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsT() > llEndTime))
                m_iEndPos--;
            m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
        }
        else {
            m_iEndPos = m_iStartPos + 1;
            auto iEventCount = (long long)m_vEvents.size();
            while (m_iEndPos > 0 && (m_iEndPos + 1 >= iEventCount || m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() > llEndTime))
                m_iEndPos--;
            m_iEndPos += (m_iStartPos - m_iEndPos) * 2;
        }
    }
    else {
        if (m_bTickMode) {
            m_iEndPos = m_iStartPos - 1;
            auto iEventCount = (long long)m_vEvents.size();
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsT() < llEndTime)
                m_iEndPos++;
        }
        else {
            m_iEndPos = m_iStartPos - 1;
            auto iEventCount = (long long)m_vEvents.size();
            while (m_iEndPos + 1 < iEventCount && m_vEvents[m_iEndPos + 1]->GetAbsMicroSec() < llEndTime)
                m_iEndPos++;
        }
    }

    if (!loadingMode)
    {
        static PlaybackSettings& cPlayback = Config::GetConfig().GetPlaybackSettings();
        long long llNewPos = ((m_llStartTime - llFirstTime) * 1000) / (llLastTime - llFirstTime);
        cPlayback.SetPosition(static_cast<int>(llNewPos));
    }

    IsLastFrameReversed = false;
    m_llPrevTime = m_llStartTime;
    m_iPrevStartPos = m_iStartPos;
}

void MainScreen::ApplyColor(MIDIMetaEvent* event) {
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
            for (int i = 0; i < 16; i++) {
                auto& chan_settings = m_vTrackSettings[event->GetTrack()].aChannels[i];
                chan_settings.SetColor(color);
                chan_settings.bHidden = data[7] == 0;
            }
        }
        else {
            auto& chan_settings = m_vTrackSettings[event->GetTrack()].aChannels[data[2]];
            chan_settings.SetColor(color);
            chan_settings.bHidden = data[7] == 0;
        }
    }
}
void MainScreen::ApplyMarker(unsigned char* data, size_t size) {
    m_pMarkerData = data;
    m_iMarkerSize = size;
    if (data) {
        static Config& config = Config::GetConfig();
        static const VideoSettings& cVideo = config.GetVideoSettings();

        constexpr int codepages[] = { 1252, 437, 82, 886, 932, 936, CP_UTF8 };

        auto temp_str = new char[size + 1];
        memcpy(temp_str, data, size);
        temp_str[size] = '\0';

        if (codepages[cVideo.eMarkerEncoding] != CP_UTF8) {
            // Yes, I have to convert to wide and then back to UTF-8...
            auto wide_len = MultiByteToWideChar(codepages[cVideo.eMarkerEncoding], 0, temp_str, size + 1, NULL, 0);
            auto wide_temp_str = new WCHAR[wide_len];
            MultiByteToWideChar(codepages[cVideo.eMarkerEncoding], 0, temp_str, size + 1, wide_temp_str, wide_len);

            auto utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_temp_str, -1, 0, 0, 0, 0);
            auto utf8_temp_str = new char[utf8_len];
            WideCharToMultiByte(CP_UTF8, 0, wide_temp_str, -1, utf8_temp_str, utf8_len, 0, 0);

            m_sMarker = std::string(utf8_temp_str);
            delete[] wide_temp_str;
            delete[] utf8_temp_str;
        }
        else {
            m_sMarker = temp_str;
        }
        delete[] temp_str;
    }
    else {
        m_sMarker = std::string();
    }
}

// Advance program change, tempo, and signature
void MainScreen::AdvanceIterators(long long llTime, bool bIsJump) {
    if (bIsJump)
    {
        m_itNextTempo = upper_bound(m_vTempo.begin(), m_vTempo.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
        MIDIMetaEvent* pPrevious = GetPrevious(m_itNextTempo, m_vTempo, 3);
        if (pPrevious)
        {
            MIDI::Parse24Bit(pPrevious->GetData(), 3, &m_iMicroSecsPerBeat);
            m_iLastTempoTick = pPrevious->GetAbsT();
            m_llLastTempoTime = pPrevious->GetAbsMicroSec();
        }
        else
        {
            m_iMicroSecsPerBeat = 500000;
            m_llLastTempoTime = m_iLastTempoTick = 0;
        }
        m_itNextSignature = upper_bound(m_vSignature.begin(), m_vSignature.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
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
        m_itNextMarker = upper_bound(m_vMarkers.begin(), m_vMarkers.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
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
            m_itNextColor = lower_bound(m_vColors.begin(), m_vColors.end(), pair<long long, int>(llTime, m_vMetaEvents.size()));
        }
        else {
            m_itNextColor = upper_bound(m_vColors.begin(), m_vColors.end(), pair<long long, int>(llTime, m_vMetaEvents.size()));
        }
    }
    else
    {
        if (m_dSpeed < 0) {
            m_itNextTempo = upper_bound(m_vTempo.begin(), m_vTempo.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
            MIDIMetaEvent* pPrevious = GetPrevious(m_itNextTempo, m_vTempo, 3);
            if (pPrevious)
            {
                MIDI::Parse24Bit(pPrevious->GetData(), 3, &m_iMicroSecsPerBeat);
                m_iLastTempoTick = pPrevious->GetAbsT();
                m_llLastTempoTime = pPrevious->GetAbsMicroSec();
            }
            else
            {
                m_iMicroSecsPerBeat = 500000;
                m_llLastTempoTime = m_iLastTempoTick = 0;
            }
            m_itNextSignature = upper_bound(m_vSignature.begin(), m_vSignature.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
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
            m_itNextMarker = upper_bound(m_vMarkers.begin(), m_vMarkers.end(), pair< long long, int >(llTime, m_vMetaEvents.size()));
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
                    MIDI::Parse24Bit(pEvent->GetData(), 3, &m_iMicroSecsPerBeat);
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
MIDIMetaEvent* MainScreen::GetPrevious(eventvec_t::const_iterator& itCurrent,
    const eventvec_t& vEventMap, int iDataLen) {
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
int  MainScreen::GetCurrentTick(long long llStartTime) {
    return GetCurrentTick(llStartTime, m_iLastTempoTick, m_llLastTempoTime, m_iMicroSecsPerBeat);
}

int  MainScreen::GetCurrentTick(long long llStartTime, int iLastTempoTick, long long llLastTempoTime, int iMicroSecsPerBeat) {
    int iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000))
    {
        if (llStartTime >= llLastTempoTime)
            return iLastTempoTick + static_cast<int>((iDivision * (llStartTime - llLastTempoTime)) / iMicroSecsPerBeat);
        else
            return iLastTempoTick - static_cast<int>((iDivision * (llLastTempoTime - llStartTime) + 1) / iMicroSecsPerBeat) - 1;
    }
    return -1;
}

// Gets the time corresponding to the tick
long long MainScreen::GetTickTime(int iTick) {
    return GetTickTime(iTick, m_iLastTempoTick, m_llLastTempoTime, m_iMicroSecsPerBeat);
}

long long MainScreen::GetTickTime(int iTick, int iLastTempoTick, long long llLastTempoTime, int iMicroSecsPerBeat) {
    int iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000))
        return llLastTempoTime + (static_cast<long long>(iMicroSecsPerBeat) * (iTick - iLastTempoTick)) / iDivision;
    //else
    //    return llLastTempoTime + ( 1000000LL * ( iTick - iLastTempoTick ) ) / iTicksPerSecond;
    return -1;
}

// Rounds up to the nearest beat
int MainScreen::GetBeat(int iTick, int iBeatType, int iLastSignatureTick) {
    int iDivision = m_MIDI.GetInfo().iDivision;
    int iTickOffset = iTick - iLastSignatureTick;
    if (!(iDivision & 0x8000))
    {
        m_CurBeat = (iTickOffset * iBeatType) / (iDivision * 4);

        if (iTickOffset > 0)
            return (iTickOffset * iBeatType - 1) / (iDivision * 4) + 1;
        else
            return (iTickOffset * iBeatType) / (iDivision * 4);
    }

    return -1;
}

// Rounds up to the nearest beat
int MainScreen::GetBeatTick(int iTick, int iBeatType, int iLastSignatureTick) {
    int iDivision = m_MIDI.GetInfo().iDivision;
    if (!(iDivision & 0x8000))
        return iLastSignatureTick + (GetBeat(iTick, iBeatType, iLastSignatureTick) * iDivision * 4) / iBeatType;
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

    m_pRenderer->ClearAndBeginScene(m_csBackground.iPrimaryRGB);
    if (m_fZoomX * m_fTempZoomX != 0.0f) {
        RenderLines();
        RenderNotes();
        if (m_bShowKB) RenderKeys();
    }
    RenderText();

    // Present the backbuffer contents to the display
    m_pRenderer->EndScene(m_bBackgroundLoaded);
    m_pRenderer->Present();

    // Get the current frame
    auto* frame = m_pRenderer->Screenshot();
    // Dump frame!!!!
    if (m_bDumpFrames && !cPlayback.GetPaused()) {
        // Write to pipe
        WriteFile(m_hVideoPipe, frame, static_cast<DWORD>(m_pRenderer->GetBufferWidth() * m_pRenderer->GetBufferHeight() * 4), nullptr, nullptr);
    }
    if (!m_bDumpFrames) { //Stop wasting time on GDI shit when rendering to video. 
        //Don't let BitBlt capture a blank screen! 
        HDC hdcGFX = GetDC(g_hWndGfx);
        HDC hdcMem = CreateCompatibleDC(hdcGFX);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcGFX, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight());
        SelectObject(hdcMem, hBitmap);
        BITMAPINFO bmpInfo = {};
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = m_pRenderer->GetBufferWidth();
        bmpInfo.bmiHeader.biHeight = -m_pRenderer->GetBufferHeight();
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = 32;
        bmpInfo.bmiHeader.biCompression = BI_RGB;
        SetDIBits(hdcMem, hBitmap, 0, m_pRenderer->GetBufferHeight(), frame, &bmpInfo, DIB_RGB_COLORS);
        BitBlt(hdcGFX, 0, 0, m_pRenderer->GetBufferWidth(), m_pRenderer->GetBufferHeight(), hdcMem, 0, 0, SRCCOPY);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(g_hWndGfx, hdcGFX);
    }

    return Success;
}

// These used to be created as local variables inside each Render* function, but too much copying of code :/
// Depends on m_llStartTime, m_llTimeSpan, m_eKeysShown, m_iStartNote, m_iEndNote
void MainScreen::RenderGlobals() {
    // Midi info
    const MIDI::MIDIInfo& mInfo = m_MIDI.GetInfo();
    if (m_eKeysShown == VisualSettings::All)
    {
        m_iStartNote = 0;
        m_iEndNote = 127;
    }
    else if (m_eKeysShown == VisualSettings::Song)
    {
        m_iStartNote = mInfo.iMinNote;
        m_iEndNote = mInfo.iMaxNote;
    }

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
        long long llMicroSecsPP = static_cast<long long>(m_llTimeSpan / m_fNotesCY + 0.5f);
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
    for (int i = m_iStartNote + 1; i <= m_iEndNote; i++)
        if (!MIDI::IsSharp(i - 1) && !MIDI::IsSharp(i))
        {
            int iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
            float fStartX = MIDI::IsSharp(m_iStartNote) * SharpRatio / 2.0f;
            float x = m_fNotesX + m_fWhiteCX * (iWhiteKeys + fStartX);
            x = floor(x + 0.5f); // Needs to be rounded because of the gradient
            m_pRenderer->DrawRect(x - 1.0f, m_fNotesY, 3.0f, m_fNotesCY,
                m_csBackground.iDarkRGB, m_csBackground.iVeryDarkRGB, m_csBackground.iVeryDarkRGB, m_csBackground.iDarkRGB);
        }

    // Horizontal (Hard!)
    int iDivision = m_MIDI.GetInfo().iDivision;
    // fuck this lmao
    if (!(iDivision & 0x8000))
    {
        // Copy time state vars
        int iCurrTick = m_iStartTick - 1;
        long long llEndTime = (m_bTickMode ? m_iStartTick : m_llStartTime) + m_llTimeSpan;

        // Copy tempo state vars
        uint32_t iLastTempoTick = m_iLastTempoTick;
        uint32_t iMicroSecsPerBeat = m_iMicroSecsPerBeat;
        long long llLastTempoTime = m_llLastTempoTime;
        eventvec_t::const_iterator itNextTempo = m_itNextTempo;

        // Copy signature state vars
        int iLastSignatureTick = m_iLastSignatureTick;
        int iBeatsPerMeasure = m_iBeatsPerMeasure;
        int iBeatType = m_iBeatType;
        eventvec_t::const_iterator itNextSignature = m_itNextSignature;

        // Compute initial next beat tick and next beat time
        long long llNextBeatTime = 0;
        int iNextBeatTick = 0;
        do
        {
            iNextBeatTick = GetBeatTick(iCurrTick + 1, iBeatType, iLastSignatureTick);

            // Next beat crosses the next tempo event. handle the event and recalculate next beat time
            while (itNextTempo != m_vTempo.end() && m_vMetaEvents[itNextTempo->second]->GetDataLen() == 3 &&
                iNextBeatTick > m_vMetaEvents[itNextTempo->second]->GetAbsT())
            {
                MIDIMetaEvent* pEvent = m_vMetaEvents[itNextTempo->second];
                MIDI::Parse24Bit(pEvent->GetData(), 3, &iMicroSecsPerBeat);
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
            int iNextBeat = GetBeat(iNextBeatTick, iBeatType, iLastSignatureTick);
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
    long long iStartPos = m_dNSpeed < 0 ? m_iStartPos - (m_iEndPos - m_iStartPos) + 1 : m_iStartPos;
    long long iEndPos = m_dNSpeed < 0 ? m_iEndPos - (m_iEndPos - m_iStartPos) : m_iEndPos;

    if (iStartPos < 0 || iEndPos >= m_vEvents.size()) return; // the note speed has been changed after processing these positions but before reaching here. 

    // Ensure that any rects rendered after this point render over the notes
    m_pRenderer->SplitRect();

    if (Config::GetConfig().GetVideoSettings().bOR) {
        if (m_dNSpeed < 0) {
            for (int i = iStartPos; i <= iEndPos; i++) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if ((pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOff || (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() == 0)) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent->GetSister(m_vEvents));
                }
            }
        }
        else {
            for (int i = iEndPos; i >= iStartPos; i--) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() > 0 && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent);
                }
            }
        }
        for (int i = m_iStartNote; i <= m_iEndNote; i++) {
            for (vector< int >::reverse_iterator it = (m_vState[i]).rbegin(); it != (m_vState[i]).rend(); it++) {
                RenderNote(m_vEvents[*it]);
            }
        }
    }
    else {
        for (int i = m_iStartNote; i <= m_iEndNote; i++) {
            for (vector< int >::iterator it = (m_vState[i]).begin(); it != (m_vState[i]).end(); it++) {
                RenderNote(m_vEvents[*it]);
            }
        }
        if (m_dNSpeed < 0) {
            for (int i = iEndPos; i >= iStartPos; i--) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if ((pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOff || (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() == 0)) && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent->GetSister(m_vEvents));
                }
            }
        }
        else {
            for (int i = iStartPos; i <= iEndPos; i++) {
                MIDIChannelEvent* pEvent = m_vEvents[i];
                if (pEvent->GetChannelEventType() == MIDIChannelEvent::NoteOn && pEvent->GetParam2() > 0 && pEvent->HasSister() &&
                    m_iStartNote <= pEvent->GetParam1() && pEvent->GetParam1() <= m_iEndNote) {
                    RenderNote(pEvent);
                }
            }
        }
    }
}

void MainScreen::RenderNote(const MIDIChannelEvent* pNote) {
    int iNote = pNote->GetParam1();
    int iTrack = pNote->GetTrack();
    int iChannel = pNote->GetChannel();
    long long llNoteStart = pNote->GetAbsMicroSec();
    long long llNoteEnd = llNoteStart + pNote->GetLength();
    if (m_bTickMode) {
        llNoteStart = pNote->GetAbsT();
        llNoteEnd = pNote->GetSister(m_vEvents)->GetAbsT();
    }
    m_pRenderer->PushNoteData(
        NoteData{
            .key = (uint8_t)iNote,
            .channel = (uint8_t)iChannel,
            .track = (uint16_t)iTrack,
            .pos = static_cast<float>((m_dNSpeed < 0) != (m_fZoomX * m_fTempZoomX < 0) ? -(llNoteStart - m_llRndStartTime) - (llNoteEnd - llNoteStart) + (m_fZoomX * m_fTempZoomX < 0 ? m_llTimeSpan : 0) : llNoteStart - m_llRndStartTime + (m_fZoomX * m_fTempZoomX < 0 ? m_llTimeSpan : 0)),
            .length = static_cast<float>(llNoteEnd - llNoteStart),
        }
    );
}

void MainScreen::GenNoteXTable() {
    UpdateNotePos = false; //We don't need to do this on every single frame! 
    float NoteWidth = (m_pRenderer->GetBufferWidth() * abs(m_fZoomX) * abs(m_fTempZoomX)) / (m_iEndNote - m_iStartNote);
    for (char ch = 0; ch < 16; ch++) {
        float ShiftAmount = m_pBendsRange[ch] == 0 ? 0 : m_pBendsValue[ch] / ((1 << 13) / m_pBendsRange[ch]);
        if (m_bFlipKeyboard) ShiftAmount *= -1;
        m_pBends[ch] = NoteWidth * ShiftAmount;
    }
    if (m_bFlipKeyboard) {
        for (int i = m_iStartNote; i <= m_iEndNote; i++) {
            if (Config::GetConfig().GetVideoSettings().bSameWidth) {
                float fStartNote = m_iStartNote;
                float fEndNote = m_iEndNote;
                float KeyCount = i - (m_iStartNote - 1);
                if (MIDI::IsSharp(m_iStartNote - 1)) {
                    fStartNote -= 0.5f;
                    KeyCount += 0.5f;
                }
                if (MIDI::IsSharp(m_iEndNote + 1)) {
                    fEndNote += 0.5f;
                }
                notex_table[i] = m_fNotesX + (m_fNotesCX - ((m_fNotesCX / ((fEndNote - fStartNote) + 1)) * KeyCount));
            }
            else {
                int iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
                float fStartX = (MIDI::IsSharp(m_iStartNote) - MIDI::IsSharp(i)) * SharpRatio / 2.0f;
                if (MIDI::IsSharp(i))
                {
                    MIDI::Note eNote = MIDI::NoteVal(i);
                    if (eNote == MIDI::CS || eNote == MIDI::FS) fStartX -= SharpRatio / 5.0f;
                    else if (eNote == MIDI::AS || eNote == MIDI::DS) fStartX += SharpRatio / 5.0f;
                }
                notex_table[i] = m_fNotesX + (m_fNotesCX - (m_fWhiteCX * (iWhiteKeys + fStartX)) - (MIDI::IsSharp(i) ? m_fWhiteCX * SharpRatio : m_fWhiteCX));
            }
        }
    }
    else {
        for (int i = m_iStartNote; i <= m_iEndNote; i++) {
            if (Config::GetConfig().GetVideoSettings().bSameWidth) {
                float fStartNote = m_iStartNote;
                float fEndNote = m_iEndNote;
                float KeyCount = i - m_iStartNote;
                if (MIDI::IsSharp(m_iStartNote - 1)) {
                    fStartNote -= 0.5f;
                    KeyCount += 0.5f;
                }
                if (MIDI::IsSharp(m_iEndNote + 1)) {
                    fEndNote += 0.5f;
                }
                notex_table[i] = m_fNotesX + (m_fNotesCX / ((fEndNote - fStartNote) + 1)) * KeyCount;
            }
            else {
                int iWhiteKeys = MIDI::WhiteCount(m_iStartNote, i);
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
    }
}

float MainScreen::GetNoteX(int iNote) {
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

    // Draw the white keys
    float fCurX = m_fNotesX + (MIDI::IsSharp(m_bFlipKeyboard ? m_iEndNote : m_iStartNote) ? m_fWhiteCX * SharpRatio / 2.0f : 0.0f);
    float fCurY = m_fZoomX * m_fTempZoomX < 0 ? fKeysY : fKeysY + fTransitionCY + fRedCY + fSpacerCY;
    for (int i = (m_bFlipKeyboard ? m_iEndNote : m_iStartNote); m_bFlipKeyboard ? (i >= m_iStartNote) : (i <= m_iEndNote); i += (m_bFlipKeyboard ? -1 : 1))
        if (!MIDI::IsSharp(i))
        {
            if (m_pNoteState[i] == -1)
            {
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY, m_fWhiteCX - fKeyGap, fTopCY + fNearCY,
                    m_csKBWhite.iDarkRGB, m_csKBWhite.iDarkRGB, m_csKBWhite.iPrimaryRGB, m_csKBWhite.iPrimaryRGB);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY, m_fWhiteCX - fKeyGap, fNearCY,
                    m_csKBWhite.iDarkRGB, m_csKBWhite.iDarkRGB, m_csKBWhite.iVeryDarkRGB, m_csKBWhite.iVeryDarkRGB);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY, m_fWhiteCX - fKeyGap, 2.0f,
                    m_csKBBackground.iDarkRGB, m_csKBBackground.iDarkRGB, m_csKBWhite.iVeryDarkRGB, m_csKBWhite.iVeryDarkRGB);
            }
            else
            {
                const MIDIChannelEvent* pEvent = m_vEvents[m_pNoteState[i]];
                const int iTrack = pEvent->GetTrack();
                const int iChannel = pEvent->GetChannel();

                ChannelSettings& csKBWhite = m_vTrackSettings[iTrack].aChannels[iChannel];
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY, m_fWhiteCX - fKeyGap, fTopCY + fNearCY - 2.0f,
                    csKBWhite.iDarkRGB, csKBWhite.iDarkRGB, csKBWhite.iPrimaryRGB, csKBWhite.iPrimaryRGB);
                m_pRenderer->DrawRect(fCurX + fKeyGap1, fCurY + fTopCY + fNearCY - 2.0f, m_fWhiteCX - fKeyGap, 2.0f, csKBWhite.iDarkRGB);
            }
            m_pRenderer->DrawRect(floor(fCurX + fKeyGap1 + m_fWhiteCX - fKeyGap + 0.5f), fCurY, fKeyGap, fTopCY + fNearCY,
                m_csKBBackground.iVeryDarkRGB, m_csKBBackground.iPrimaryRGB, m_csKBBackground.iPrimaryRGB, m_csKBBackground.iVeryDarkRGB);

            fCurX += m_fWhiteCX;
        }

    // Draw the sharps
    float fSharpTop = SharpRatio * 0.7f;
    fCurX = m_fNotesX + (MIDI::IsSharp(m_bFlipKeyboard ? m_iEndNote : m_iStartNote) ? m_fWhiteCX * SharpRatio / 2.0f : 0.0f);
    fCurY = m_fZoomX * m_fTempZoomX < 0 ? fKeysY + fKeysCY - fSharpCY - fTransitionCY - fRedCY - fSpacerCY : fKeysY + fTransitionCY + fRedCY + fSpacerCY;
    for (int i = (m_bFlipKeyboard ? m_iEndNote : m_iStartNote); m_bFlipKeyboard ? (i >= m_iStartNote) : (i <= m_iEndNote); i += (m_bFlipKeyboard ? -1 : 1))
        if (!MIDI::IsSharp(i))
            fCurX += m_fWhiteCX;
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

            if (m_pNoteState[i] == -1)
            {
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY + fSharpCY - fNearCY,
                    fSharpTopX2, fCurY + fSharpCY - fNearCY,
                    x + cx, fCurY + fSharpCY, x, fCurY + fSharpCY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY,
                    fSharpTopX1, fCurY + fSharpCY - fNearCY,
                    x, fCurY + fSharpCY, x, fCurY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX2, fCurY + fSharpCY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY,
                    x + cx, fCurY, x + cx, fCurY + fSharpCY,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB);
                m_pRenderer->DrawRect(fSharpTopX1, fCurY - fNearCY, fSharpTopX2 - fSharpTopX1, fSharpCY, m_csKBSharp.iVeryDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f,
                    fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f,
                    m_csKBSharp.iDarkRGB, m_csKBSharp.iDarkRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.35f,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.45f,
                    fSharpTopX2, fCurY - fNearCY + fSharpCY * 0.65f,
                    fSharpTopX1, fCurY - fNearCY + fSharpCY * 0.55f,
                    m_csKBSharp.iPrimaryRGB, m_csKBSharp.iPrimaryRGB, m_csKBSharp.iVeryDarkRGB, m_csKBSharp.iVeryDarkRGB);
            }
            else
            {
                const MIDIChannelEvent* pEvent = m_vEvents[m_pNoteState[i]];
                const int iTrack = pEvent->GetTrack();
                const int iChannel = pEvent->GetChannel();

                const float fNewNear = fNearCY * 0.25f;

                const ChannelSettings& csKBSharp = m_vTrackSettings[iTrack].aChannels[iChannel];
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY + fSharpCY - fNewNear,
                    fSharpTopX2, fCurY + fSharpCY - fNewNear,
                    x + cx, fCurY + fSharpCY, x, fCurY + fSharpCY,
                    csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iDarkRGB, csKBSharp.iDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear,
                    fSharpTopX1, fCurY + fSharpCY - fNewNear,
                    x, fCurY + fSharpCY, x, fCurY,
                    csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iDarkRGB, csKBSharp.iDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX2, fCurY + fSharpCY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear,
                    x + cx, fCurY, x + cx, fCurY + fSharpCY,
                    csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iDarkRGB, csKBSharp.iDarkRGB);
                m_pRenderer->DrawRect(fSharpTopX1, fCurY - fNewNear, fSharpTopX2 - fSharpTopX1, fSharpCY, csKBSharp.iDarkRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f,
                    fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f,
                    csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB);
                m_pRenderer->DrawSkew(fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.25f,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.35f,
                    fSharpTopX2, fCurY - fNewNear + fSharpCY * 0.75f,
                    fSharpTopX1, fCurY - fNewNear + fSharpCY * 0.65f,
                    csKBSharp.iPrimaryRGB, csKBSharp.iPrimaryRGB, csKBSharp.iDarkRGB, csKBSharp.iDarkRGB);
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

    int Lines = 10; //Basic info
    if (cVideo.bDebug) {
        Lines += 10; //Debug info
    }
    if (cControls.bPhigros) {
        Lines += 4; //Score and level
    }
    if (cVideo.bDumpFrames) {
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
    if (cVideo.bDumpFrames) {
        Lines -= 1; //Don't show FPS when exporting videos
    }

    // Screen info
    RECT rcStatus = { m_pRenderer->GetBufferWidth() - StatisticsWidth, 0, m_pRenderer->GetBufferWidth(), 16 * Lines + 10 };

    int iMsgCY = 200;
    RECT rcMsg = { 0, static_cast<int>(m_pRenderer->GetBufferHeight() * (1.0f - KBPercent) - iMsgCY) / 2 };
    rcMsg.right = m_pRenderer->GetBufferWidth();
    rcMsg.bottom = rcMsg.top + iMsgCY;

    // Draw the text
    m_pRenderer->BeginText();
    RenderStatus(&rcStatus);
    if (!m_sMarker.empty() && cVideo.bShowMarkers) {
        RenderMarker(m_sMarker.c_str());
    }
    if (m_bZoomMove) {
        RenderMessage(&rcMsg, ZoomMoveMsg);
    }
    if (strlen(CheatEngineCaption) > 0) {
        if (strlen(CheatEngineCaption) < sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0])) {
            RenderMessage(&rcMsg, static_cast<string>(CheatEngineCaption).c_str());
        }
        else {
            RenderMessage(&rcMsg, ("The caption has exceeded the maximum acceptable length of " + to_string(sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0])) + " characters. \nAs a result, this caption has been blocked from showing in order to prevent crashing. \nPlease consider writing something slightly shorter. ").c_str());
        }
    }
    m_pRenderer->EndText();
}

void MainScreen::RenderStatusLine(int line, const char* left, const char* format, ...) {
    va_list varargs;
    va_start(varargs, format);

    if (Config::GetConfig().GetVideoSettings().bDisableUI) return;

    char buf[1 << 10] = {};
    vsnprintf_s(buf, sizeof(buf), format, varargs);

    auto draw_list = m_pRenderer->GetDrawList();
    ImVec2 left_pos = ImVec2(m_pRenderer->GetBufferWidth() - (StatisticsWidth - 5), 3 + line * 16);
    ImVec2 right_pos = ImVec2(m_pRenderer->GetBufferWidth() - ImGui::CalcTextSize(buf, NULL, false, -1.0f, 1 << 4).x - 6, 3 + line * 16);
    draw_list->AddText(NULL, 1 << 4, ImVec2(left_pos.x + 2, left_pos.y + 1), 0xFF404040, left);
    draw_list->AddText(NULL, 1 << 4, ImVec2(left_pos.x, left_pos.y), 0xFFFFFFFF, left);
    draw_list->AddText(NULL, 1 << 4, ImVec2(right_pos.x + 2, right_pos.y + 1), 0xFF404040, buf);
    draw_list->AddText(NULL, 1 << 4, ImVec2(right_pos.x, right_pos.y), 0xFFFFFFFF, buf);

    va_end(varargs);
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
    auto min = abs(m_llStartTime) / 60000000;
    auto sec = (abs(m_llStartTime) % 60000000) / 1000000;
    auto cs = (abs(m_llStartTime) % 1000000) / 100000;
    auto tmin = TotalTime / 60000000;
    auto tsec = (TotalTime % 60000000) / 1000000;
    auto tcs = (TotalTime % 1000000) / 100000;
    auto tempo = 60000000.0 / m_iMicroSecsPerBeat;
    long long iMaxMS = m_MIDI.GetInfo().llTotalMicroSecs / MS;
    int cur_line = 0;

    if (!cVideo.bDisableUI) {
        m_pRenderer->GetDrawList()->AddRectFilled(ImVec2(prcStatus->left, prcStatus->top), ImVec2(prcStatus->right, prcStatus->bottom), 0x80000000);
    }

    llStartTimeFormatted = std::to_string(abs(m_llStartTime));
    for (int i = llStartTimeFormatted.length() - 3; i > 0; i -= 3)
        llStartTimeFormatted.insert(i, " ");
    if (m_llStartTime < 0)
        llStartTimeFormatted.insert(0, "-");

    TotalTimeFormatted = std::to_string(TotalTime);
    for (int i = TotalTimeFormatted.length() - 3; i > 0; i -= 3)
        TotalTimeFormatted.insert(i, " ");

    polyphony = transform_reduce(begin(m_vState),end(m_vState),0,plus<>(),[](const vector<int>& state){return state.size();});
    polyFormatted = std::to_string(polyphony);
    for (int i = polyFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        polyFormatted.insert(i, ",");

    nps = m_vNCTable[min(max(m_llStartTime / MS, 0LL), iMaxMS)] - m_vNCTable[min(max((m_llStartTime - S) / MS, 0LL), iMaxMS)];
    npsFormatted = std::to_string(nps);
    for (int i = npsFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        npsFormatted.insert(i, ",");
    
    passed = m_vNCTable[min(max(m_llStartTime / MS, 0LL), iMaxMS)];
    passedFormatted = std::to_string(passed);
    for (int i = passedFormatted.length() - DigitSeparate; i > 0; i -= DigitSeparate)
        passedFormatted.insert(i, ",");

    RenderStatusLine(cur_line++, StatisticsText1, ("v" + RVersionString).c_str());
    RenderStatusLine(cur_line++, StatisticsText2, "");
    RenderStatusLine(cur_line++, "", "");
    RenderStatusLine(cur_line++, StatisticsText3, "%s%lld:%02d.%d / %lld:%02d.%d",
        m_llStartTime >= 0 ? "" : "-",
        min, sec, cs,
        tmin, tsec, tcs);
    RenderStatusLine(cur_line++, StatisticsText4, "%d/%d", m_iStartTick, resolution);
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText5, llStartTimeFormatted.c_str());
    }
    if (!cVideo.bDumpFrames) {
        RenderStatusLine(cur_line++, StatisticsText6, "%.2lf", m_dFPS);
    }
    RenderStatusLine(cur_line++, StatisticsText7, "%.3lf bpm", tempo);
    if (cControls.bPhigros) {
        RenderStatusLine(cur_line++, StatisticsText8, passedFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText9, npsFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText10, polyFormatted.c_str());
    }
    else {
        RenderStatusLine(cur_line++, StatisticsText11, passedFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText12, npsFormatted.c_str());
        RenderStatusLine(cur_line++, StatisticsText13, polyFormatted.c_str());
    }
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText14, "%lld", m_pRenderer->GetRenderedNotesCount());
        if (m_bMute) {
            RenderStatusLine(cur_line++, StatisticsText15, StatisticsText16);
        }
        else {
            RenderStatusLine(cur_line++, StatisticsText15, "%.0lf%%", cPlayback.GetVolume() * 100);
        }
    }
    if (cVideo.bDumpFrames) {
        if (cVideo.bDebug) {
            RenderStatusLine(cur_line++, StatisticsText17, "%.0lf%%", cPlayback.GetSpeed() * 100);
        }
    }
    else {
        if (cVideo.bDebug) {
            if (m_Timer.m_bManualTimer) {
                RenderStatusLine(cur_line++, StatisticsText17, "%.0lf%%", cPlayback.GetSpeed() * (m_dFPS / m_Timer.m_dFramerate) * 100);
            }
            else {
                RenderStatusLine(cur_line++, StatisticsText17, "%.0lf%%", cPlayback.GetSpeed() * 100);
            }
        }
        else {
            if (m_Timer.m_bManualTimer) {
                RenderStatusLine(cur_line++, StatisticsText17, "%.0lf%%", (m_dFPS / m_Timer.m_dFramerate) * 100);
            }
        }
    }
    if (cVideo.bDebug) {
        RenderStatusLine(cur_line++, StatisticsText18, "%f", cPlayback.GetNSpeed());
        RenderStatusLine(cur_line++, StatisticsText19, "%f", cView.GetOffsetX() + m_fTempOffsetX);
        RenderStatusLine(cur_line++, StatisticsText20, "%f", cView.GetOffsetY() + m_fTempOffsetY);
        RenderStatusLine(cur_line++, StatisticsText21, "%f", cView.GetZoomX() * m_fTempZoomX);
        RenderStatusLine(cur_line++, StatisticsText22, "%d*%d", width, height);
        RenderStatusLine(cur_line++, StatisticsText23, "%d~%d", m_bFlipKeyboard ? m_iEndNote : m_iStartNote, m_bFlipKeyboard ? m_iStartNote : m_iEndNote);
        RenderStatusLine(cur_line++, StatisticsText24, "%d", cControls.iVelocityThreshold);
    }
    if (cControls.bPhigros) {
        RenderStatusLine(cur_line++, StatisticsText25, "%07.0f", (passed == static_cast<long long>(TotalNC) ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)));
        RenderStatusLine(cur_line++, StatisticsText26, Difficulty);
        RenderStatusLine(cur_line++, StatisticsText27, StatisticsText28);
        if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) == static_cast<float>(1000000)) {
            RenderStatusLine(cur_line++, StatisticsText29, "");
        }
        else if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) < static_cast<float>(1000000)) {
            if (!cPlayback.GetPaused()) {
                RenderStatusLine(cur_line++, StatisticsText30, "");
            }
            else {
                RenderStatusLine(cur_line++, StatisticsText31, "");
            }
        }
        else if (static_cast<float>(passed == TotalNC ? 1000000 : floor(static_cast<float>(passed) / static_cast<float>(TotalNC) * 1000000)) > static_cast<float>(1000000)) {
            RenderStatusLine(cur_line++, StatisticsText32, "");
        }
    }
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    uint8_t line;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    if (FrameCount % (1 << 4) == 0) {
        line = 0;
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "  "; for (uint8_t i = 0; i < (csbi.dwSize.X - (1 << 4)) / 2; i++) cout << "="; cout << " Debug Info "; for (uint8_t i = 0; i < (csbi.dwSize.X - (1 << 4)) / 2; i++) cout << "="; cout << "  ";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    SongLength: " << TotalTimeFormatted << " (Qword +" + GetAddress(TotalTime) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Microseconds: " << llStartTimeFormatted << " (Qword +" + GetAddress(m_llStartTime) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Ticks: " << m_iStartTick << " (Integer +" + GetAddress(m_iStartTick) + ")[Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Resolution: " << resolution << " (SmallInteger +" + GetAddress(resolution) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NoteCount: " << TotalNC << " (Integer +" + GetAddress(TotalNC) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NotesPerSecond: " << nps << " (Integer +" + GetAddress(nps) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Polyphony: " << polyphony << " (Integer +" + GetAddress(polyphony) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Passed: " << passed << " (Integer +" + GetAddress(passed) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Volume: " << cPlayback.GetVolume() << " (Double +" + cPlayback.GetVolumeAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Mute: " << cPlayback.GetMute() << " (ShortInteger +" + cPlayback.GetMuteAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    PlaybackSpeed: " << cPlayback.GetSpeed() << " (Double +" + cPlayback.GetSpeedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    NoteSpeed: " << cPlayback.GetNSpeed() << " (Double +" + cPlayback.GetNSpeedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Offset-X: " << cView.GetOffsetX() << " (Float +" + cView.GetOffsetXAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Offset-Y: " << cView.GetOffsetY() << " (Float +" + cView.GetOffsetYAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Zoom: " << cView.GetZoomX() << " (Float +" + cView.GetZoomXAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    SameWidthNotes: " << cVideo.bSameWidth << " (ShortInteger +" + GetAddress(cVideo.bSameWidth) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    KeyRange: " << cVisual.iFirstKey << "~" << cVisual.iLastKey << " (Integer +" + GetAddress(cVisual.iFirstKey) + " ~ Integer +" + GetAddress(cVisual.iLastKey) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    KeyMode: " << static_cast<short>(cVisual.eKeysShown) << " (ShortInteger +" + GetAddress(cVisual.eKeysShown) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    WindowSize: " << width << "*" << height << " (Integer +" + GetAddress(width) + " * Integer +" + GetAddress(height) + ") [Read Only]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Paused: " << cPlayback.GetPaused() << " (ShortInteger +" + cPlayback.GetPausedAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Keyboard: " << cView.GetKeyboard() << " (ShortInteger +" + cView.GetKeyboardAddress() + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    VisualizePitchBends: " << cVideo.bVisualizePitchBends << " (ShortInteger +" + GetAddress(cVideo.bVisualizePitchBends) + ")[Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    PhigrosMode: " << cControls.bPhigros << " (ShortInteger +" + GetAddress(cControls.bPhigros) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    ShowMarkers: " << cVideo.bShowMarkers << " (ShortInteger +" + GetAddress(cVideo.bShowMarkers) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    TickBased: " << cVideo.bTickBased << " (ShortInteger +" + GetAddress(cVideo.bTickBased) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    HideStatistics: " << cVideo.bDisableUI << " (ShortInteger +" + GetAddress(cVideo.bDisableUI) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    RemoveOverlaps: " << cVideo.bOR << " (ShortInteger +" + GetAddress(cVideo.bOR) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    LimitFPS: " << m_pRenderer->GetLimitFPS() << " (ShortInteger +" + GetAddress(cVideo.bLimitFPS) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    VelocityThreshold: " << static_cast<int>(cControls.iVelocityThreshold % (1<<7)) << " (ShortInteger +" + GetAddress(cControls.iVelocityThreshold) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    Caption: \"" << (strlen(CheatEngineCaption) < sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]) ? CheatEngineCaption : "MAXIMUM LENGTH EXCEEDED! ") << "\" (String[" << sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]) << "] +" + GetAddress(CheatEngineCaption) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        SetConsoleCursorPosition(hConsole, pos);
        cout << "    DifficultyText: \"" << (strlen(Difficulty) < sizeof(Difficulty) / sizeof(Difficulty[0]) ? Difficulty : "MAXIMUM LENGTH EXCEEDED! ") << "\" (String[" << sizeof(Difficulty) / sizeof(Difficulty[0]) << "] +" + GetAddress(Difficulty) + ") [Read / Write]";
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X, ' ');
        pos.X = 0;
        pos.Y = line; line++;
        SetConsoleCursorPosition(hConsole, pos);
        cout << string(csbi.dwSize.X - 1, ' ');
    }
    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(hConsole, pos);
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = csbi.dwSize.X - 1;
    windowSize.Bottom = line - 1;
    COORD bufferSize;
    bufferSize.X = csbi.dwSize.X;
    bufferSize.Y = max(line, csbi.dwSize.Y);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    FrameCount++;
}

void MainScreen::RenderMarker(const char* str) {
    ImVec2 size = ImGui::CalcTextSize(str, NULL, false, -1.0f, (1 << 4) + (1 << 2));
    size.x += 12;
    size.y += 6;

    auto draw_list = m_pRenderer->GetDrawList();
    draw_list->AddRectFilled(ImVec2(0, 0), size, 0x80000000);
    draw_list->AddText(NULL, (1 << 4) + (1 << 2), ImVec2(6 + 2, 3 + 1), 0xFF404040, str);
    draw_list->AddText(NULL, (1 << 4) + (1 << 2), ImVec2(6, 3), 0xFFFFFFFF, str);
}

void MainScreen::RenderMessage(LPRECT prcMsg, const char* sMsg) {
    uint16_t fontsize = (1 << 5) - (1 << 2);
    while (
        (
            ImGui::CalcTextSize(sMsg, NULL, false, -1.0f, fontsize).x >= m_pRenderer->GetBufferWidth()
            ||
            ImGui::CalcTextSize(sMsg, NULL, false, -1.0f, fontsize).y >= m_pRenderer->GetBufferHeight()
            )
        &&
        fontsize > (1 << 3)
        ) {
        fontsize--;
    }
    ImVec2 textSize = ImGui::CalcTextSize(sMsg, NULL, false, -1.0f, fontsize);
    ImVec2 messageSize;
    messageSize.x = prcMsg->left + (prcMsg->right - prcMsg->left - textSize.x) / 2;
    messageSize.y = prcMsg->top + (prcMsg->bottom - prcMsg->top - textSize.y) / 2;
    auto draw_list = m_pRenderer->GetDrawList();
    draw_list->AddRectFilled(
        ImVec2(messageSize.x - 6, messageSize.y - 3),
        ImVec2(messageSize.x + textSize.x + 6, messageSize.y + textSize.y + 3),
        0x80000000
    );
    draw_list->AddText(
        NULL,
        fontsize,
        ImVec2(messageSize.x + 2, messageSize.y + 1),
        0xFF404040,
        sMsg
    );
    draw_list->AddText(
        NULL,
        fontsize,
        ImVec2(messageSize.x, messageSize.y),
        0xFFFFFFFF,
        sMsg
    );
}