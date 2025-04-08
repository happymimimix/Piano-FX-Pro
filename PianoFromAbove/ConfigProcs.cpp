/*************************************************************************************************
*
* File: ConfigProcs.cpp
*
* Description: Implements configuration GUI functions. Mostly window procs. Not C++ :/
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <TChar.h>
#include <shlobj.h>
#include <Uxtheme.h>
#include <Vsstyle.h>
#include <Dbt.h>

#include "ConfigProcs.h"
#include "MainProcs.h"
#include "Globals.h"
#include "resource.h"
#include "Language.h"
#include "imgui/Fonts.h"
#include "imgui/imguiCompressedFont2GDI.h"
#include "GameState.h"

HFONT PHIFON_GDI;

VOID DoPreferences(HWND hWndOwner)
{
    int pDialogs[] = { IDD_PP1_VISUAL, IDD_PP2_AUDIO, IDD_PP3_VIDEO, IDD_PP4_CONTROLS };
    DLGPROC pProcs[] = { VisualProc, AudioProc, VideoProc, ControlsProc };
    LPCWSTR pTitles[] = { Property1Title, Property2Title, Property3Title, Property4Title };
    PROPSHEETPAGE psp[sizeof(pDialogs) / sizeof(int)];
    PROPSHEETHEADER psh;

    for (size_t i = 0; i < sizeof(psp) / sizeof(PROPSHEETPAGE); i++)
    {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE;
        psp[i].hInstance = g_hInstance;
        psp[i].pszTemplate = MAKEINTRESOURCE(pDialogs[i]);
        psp[i].pszIcon = NULL;
        psp[i].pfnDlgProc = pProcs[i];
        psp[i].pszTitle = pTitles[i];
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP;
    psh.hwndParent = hWndOwner;
    psh.hInstance = g_hInstance;
    psh.pszIcon = NULL;
    psh.pszCaption = PropertyWindowTitle;
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

VOID Changed(HWND hWnd)
{
    SendMessage(GetParent(hWnd), PSM_CHANGED, (WPARAM)hWnd, 0);
}

INT_PTR WINAPI VisualProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        const VisualSettings& cVisual = Config::GetConfig().GetVisualSettings();
        HWND hWndFirstKey = GetDlgItem(hWnd, IDC_FIRSTKEY);
        HWND hWndLastKey = GetDlgItem(hWnd, IDC_LASTKEY);
        // Enumerate the keys
        for (int i = 0; i < 128; i++)
        {
            SendMessage(hWndFirstKey, CB_ADDSTRING, i, (LPARAM)MIDI::NoteName(i).c_str());
            SendMessage(hWndLastKey, CB_ADDSTRING, i, (LPARAM)MIDI::NoteName(i).c_str());
        }
        // Set values
        CheckRadioButton(hWnd, IDC_SHOWALLKEYS, IDC_SHOWCUSTOMKEYS, IDC_SHOWALLKEYS + (cVisual.eKeysShown > cVisual.Custom ? cVisual.All : cVisual.eKeysShown));
        SendMessage(hWnd, WM_COMMAND, IDC_SHOWALLKEYS + (cVisual.eKeysShown > cVisual.Custom ? cVisual.All : cVisual.eKeysShown), 0);
        SendMessage(hWndFirstKey, CB_SETCURSEL, cVisual.iFirstKey, 0);
        SendMessage(hWndLastKey, CB_SETCURSEL, cVisual.iLastKey, 0);
        CheckDlgButton(hWnd, IDC_RANDOMIZE, cVisual.bRandomizeColor);
        for (int i = 0; i < IDC_COLOR16 - IDC_COLOR1 + 1; i++)
            EnableWindow(GetDlgItem(hWnd, IDC_COLOR1 + i), !IsDlgButtonChecked(hWnd, IDC_RANDOMIZE));
        // Colors
        for (int i = 0; i < IDC_COLOR16 - IDC_COLOR1 + 1; i++)
            SetWindowLongPtr(GetDlgItem(hWnd, IDC_COLOR1 + i), GWLP_USERDATA, cVisual.colors[i]);
        SetWindowLongPtr(GetDlgItem(hWnd, IDC_BKGCOLOR), GWLP_USERDATA, cVisual.iBkgColor);
        SetWindowLongPtr(GetDlgItem(hWnd, IDC_BARCOLOR), GWLP_USERDATA, cVisual.iBarColor);
        SetDlgItemTextW(hWnd, IDC_BACKGROUND, cVisual.sBackground.c_str());
        return TRUE;
    }
    // Draws the colored buttons
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        if ((pdis->CtlID < IDC_COLOR1 || pdis->CtlID > IDC_COLOR16) && pdis->CtlID != IDC_BKGCOLOR && pdis->CtlID != IDC_BARCOLOR)
            return FALSE;

        SetTextColor(pdis->hDC, RGB(255, 255, 255));
        SetBkMode(pdis->hDC, TRANSPARENT);
        SetDCBrushColor(pdis->hDC, IsDlgButtonChecked(hWnd, IDC_RANDOMIZE) && pdis->CtlID >= IDC_COLOR1 && pdis->CtlID <= IDC_COLOR16 ? 0x7f7f7f : (COLORREF)GetWindowLongPtr(pdis->hwndItem, GWLP_USERDATA));
        FillRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(DC_BRUSH));
        if (IsDlgButtonChecked(hWnd, IDC_RANDOMIZE) && pdis->CtlID >= IDC_COLOR1 && pdis->CtlID <= IDC_COLOR16)
        {
            if (!PHIFON_GDI) {
                PHIFON_GDI = imguiFont2GDI(PHIFON_compressed_data, PHIFON_compressed_size, -MulDiv(LargeFontSize, GetDeviceCaps(pdis->hDC, LOGPIXELSY), 72));
            }
            SelectObject(pdis->hDC, PHIFON_GDI);
            DrawText(pdis->hDC, L"?", -1, &pdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        return TRUE;
    }
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        Changed(hWnd);
        switch (iId)
        {
        case IDC_SHOWCUSTOMKEYS:
            EnableWindow(GetDlgItem(hWnd, IDC_FIRSTKEY), TRUE);
            EnableWindow(GetDlgItem(hWnd, IDC_THROUGH), TRUE);
            EnableWindow(GetDlgItem(hWnd, IDC_LASTKEY), TRUE);
            return TRUE;
        case IDC_SHOWALLKEYS: case IDC_SHOWSONGKEYS:
            EnableWindow(GetDlgItem(hWnd, IDC_FIRSTKEY), FALSE);
            EnableWindow(GetDlgItem(hWnd, IDC_THROUGH), FALSE);
            EnableWindow(GetDlgItem(hWnd, IDC_LASTKEY), FALSE);
            return TRUE;
            // Color buttons. Pop up color choose dialog and set color.
        case IDC_COLOR1: case IDC_COLOR2: case IDC_COLOR3:
        case IDC_COLOR4: case IDC_COLOR5: case IDC_COLOR6:
        case IDC_COLOR7: case IDC_COLOR8: case IDC_COLOR9:
        case IDC_COLOR10: case IDC_COLOR11: case IDC_COLOR12:
        case IDC_COLOR13: case IDC_COLOR14: case IDC_COLOR15:
        case IDC_COLOR16:
        case IDC_BKGCOLOR:
        case IDC_BARCOLOR:
        {
            static COLORREF acrCustClr[16] = { 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
                                               0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF };
            HWND hWndBtn = (HWND)lParam;

            // Choose and save the color
            CHOOSECOLOR cc = {};
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hWnd;
            cc.lpCustColors = (LPDWORD)acrCustClr;
            cc.rgbResult = (COLORREF)GetWindowLongPtr(hWndBtn, GWLP_USERDATA);
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (!ChooseColor(&cc)) return TRUE;

            // Draw the button (indirect)
            SetWindowLongPtr(hWndBtn, GWLP_USERDATA, cc.rgbResult);
            InvalidateRect(hWndBtn, nullptr, FALSE);
            return TRUE;
        }
        case IDC_RANDOMIZE:
        {
            for (int i = 0; i < IDC_COLOR16 - IDC_COLOR1 + 1; i++)
                EnableWindow(GetDlgItem(hWnd, IDC_COLOR1 + i), !IsDlgButtonChecked(hWnd, IDC_RANDOMIZE));
            //InvalidateRect(hWnd, nullptr, FALSE);
            return TRUE;
        }
        case IDC_RESTOREDEFAULTS:
        {
            VisualSettings cVisual = Config::GetConfig().GetVisualSettings();
            cVisual.LoadDefaultColors();
            for (int i = 0; i < IDC_COLOR16 - IDC_COLOR1 + 1; i++)
                SetWindowLongPtr(GetDlgItem(hWnd, IDC_COLOR1 + i), GWLP_USERDATA, cVisual.colors[i]);
            SetWindowLongPtr(GetDlgItem(hWnd, IDC_BKGCOLOR), GWLP_USERDATA, cVisual.iBkgColor);
            SetWindowLongPtr(GetDlgItem(hWnd, IDC_BARCOLOR), GWLP_USERDATA, cVisual.iBarColor);
            InvalidateRect(hWnd, nullptr, FALSE);
            return TRUE;
        }
        case IDC_BACKGROUNDBROWSE: 
        {
            OPENFILENAME ofn = {};
            TCHAR sFilename[1<<10] = { 0 };
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = TEXT("Image files\0*.png;*.jpg;*.jpeg\0");
            ofn.lpstrFile = sFilename;
            ofn.nMaxFile = sizeof(sFilename) / sizeof(TCHAR);
            ofn.lpstrTitle = OpenBackgroundFileTitle;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (GetOpenFileName(&ofn))
                SetDlgItemTextW(hWnd, IDC_BACKGROUND, sFilename);
            return TRUE;
        }
        case IDC_BACKGROUNDRESET: 
        {
            Changed(hWnd);
            SetDlgItemTextW(hWnd, IDC_BACKGROUND, L"");
            return TRUE;
        }
        }
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        switch (lpnmhdr->code)
        {
            // OK or Apply button pressed
        case PSN_APPLY:
        {
            // Get a copy of the config to overwrite the settings
            Config& config = Config::GetConfig();
            VisualSettings cVisual = config.GetVisualSettings();

            // VisualSettings struct
            cVisual.eKeysShown = (IsDlgButtonChecked(hWnd, IDC_SHOWALLKEYS) ? cVisual.All :
                IsDlgButtonChecked(hWnd, IDC_SHOWSONGKEYS) ? cVisual.Song :
                IsDlgButtonChecked(hWnd, IDC_SHOWCUSTOMKEYS) ? cVisual.Custom :
                cVisual.All);
            cVisual.iFirstKey = (int)SendMessage(GetDlgItem(hWnd, IDC_FIRSTKEY), CB_GETCURSEL, 0, 0);
            cVisual.iLastKey = (int)SendMessage(GetDlgItem(hWnd, IDC_LASTKEY), CB_GETCURSEL, 0, 0);
            cVisual.bRandomizeColor = IsDlgButtonChecked(hWnd, IDC_RANDOMIZE);
            for (int i = 0; i < IDC_COLOR16 - IDC_COLOR1 + 1; i++)
                cVisual.colors[i] = (int)GetWindowLongPtr(GetDlgItem(hWnd, IDC_COLOR1 + i), GWLP_USERDATA);
            cVisual.iBkgColor = (int)GetWindowLongPtr(GetDlgItem(hWnd, IDC_BKGCOLOR), GWLP_USERDATA);
            cVisual.iBarColor = (int)GetWindowLongPtr(GetDlgItem(hWnd, IDC_BARCOLOR), GWLP_USERDATA);
            wchar_t background[1 << 10]{};
            GetWindowTextW(GetDlgItem(hWnd, IDC_BACKGROUND), background, 1 << 10);
            cVisual.sBackground = background;

            // Report success and return
            config.SetVisualSettings(cVisual);
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);

            // Save settings
            config.SaveConfigValues();
            return TRUE;
        }
        }
        break;
    }
    }

    return FALSE;
}

INT_PTR WINAPI AudioProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        const AudioSettings& cAudio = Config::GetConfig().GetAudioSettings();
        Config::GetConfig().LoadMIDIDevices();

        HWND hWndOutDevs = GetDlgItem(hWnd, IDC_MIDIOUT);
        while (SendMessage(hWndOutDevs, LB_DELETESTRING, 0, 0) > 0);
        for (vector< wstring >::const_iterator it = cAudio.vMIDIOutDevices.begin(); it != cAudio.vMIDIOutDevices.end(); ++it)
            SendMessage(hWndOutDevs, LB_ADDSTRING, 0, (LPARAM)(it->c_str()));
        SendMessage(hWndOutDevs, LB_SETCURSEL, cAudio.iOutDevice, 0);

        CheckDlgButton(hWnd, IDC_KDMAPI, cAudio.bKDMAPI);
        return TRUE;
    }
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        int iCode = HIWORD(wParam);
        if (iCode == LBN_SELCHANGE || (iId == IDC_KDMAPI && iCode == BN_CLICKED))
            Changed(hWnd);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        switch (lpnmhdr->code)
        {
            // OK or Apply button pressed
        case PSN_APPLY:
        {
            // Get a copy of the config to overwrite the settings
            Config& config = Config::GetConfig();
            AudioSettings cAudio = config.GetAudioSettings();

            // Get the values
            cAudio.iOutDevice = (int)SendDlgItemMessage(hWnd, IDC_MIDIOUT, LB_GETCURSEL, 0, 0);
            cAudio.bKDMAPI = IsDlgButtonChecked(hWnd, IDC_KDMAPI);
            cAudio.sDesiredOut = cAudio.vMIDIOutDevices[cAudio.iOutDevice];

            // Set the values
            bool bChanged = false;
            if (cAudio.bKDMAPI) {
                if (cAudio.bKDMAPI != config.GetAudioSettings().bKDMAPI) {
                    bChanged = true;
                }
            }
            else {
                if (cAudio.bKDMAPI != config.GetAudioSettings().bKDMAPI || cAudio.iOutDevice != config.GetAudioSettings().iOutDevice) {
                    bChanged = true;
                }
            }
            if (bChanged)
                HandOffMsg(WM_DEVICECHANGE, 0, 0);

            // Report success and return
            config.SetAudioSettings(cAudio);
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);

            // Save settings
            config.SaveConfigValues();
            return TRUE;
        }
        }
        break;
    }
    }
    return FALSE;
}

INT_PTR WINAPI VideoProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        // Config to fill out the form
        const VideoSettings& cVideo = Config::GetConfig().GetVideoSettings();

        CheckDlgButton(hWnd, IDC_TICKBASED, cVideo.bTickBased);
        CheckDlgButton(hWnd, IDC_PITCHBENDS, cVideo.bVisualizePitchBends);
        CheckDlgButton(hWnd, IDC_SAMEWIDTH, cVideo.bSameWidth);
        CheckDlgButton(hWnd, IDC_MARKERS, cVideo.bShowMarkers);
        const wchar_t* codepages[] = { L"CP-1252 (Western)", L"CP-437 (American)", L"CP-82 (Korean)", L"CP-886 (Taiwan)", L"CP-932 (Japanese)", L"CP-936 (Chinese)", L"UTF-8" };
        for (size_t i = 0; i < sizeof(codepages) / sizeof(const wchar_t*); i++)
            SendMessage(GetDlgItem(hWnd, IDC_MARKERENC), CB_ADDSTRING, i, (LPARAM)codepages[i]);
        SendMessage(GetDlgItem(hWnd, IDC_MARKERENC), CB_SETCURSEL, cVideo.eMarkerEncoding, 0);
        CheckDlgButton(hWnd, IDC_LIMITFPS, cVideo.bLimitFPS);
        CheckDlgButton(hWnd, IDC_FFMPEG, cVideo.bDumpFrames);
        CheckDlgButton(hWnd, IDC_DEBUG, cVideo.bDebug);
        CheckDlgButton(hWnd, IDC_DISABLEUI, cVideo.bDisableUI);

        return TRUE;
    }
    case WM_COMMAND:
        Changed(hWnd);
        break;
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        switch (lpnmhdr->code)
        {
            // OK or Apply button pressed
        case PSN_APPLY:
        {
            // Get a copy of the config to overwrite the settings
            Config& config = Config::GetConfig();
            VideoSettings cVideo = config.GetVideoSettings();
            cVideo.bTickBased = IsDlgButtonChecked(hWnd, IDC_TICKBASED);
            cVideo.bVisualizePitchBends = IsDlgButtonChecked(hWnd, IDC_PITCHBENDS);
            cVideo.bSameWidth = IsDlgButtonChecked(hWnd, IDC_SAMEWIDTH);
            cVideo.bShowMarkers = IsDlgButtonChecked(hWnd, IDC_MARKERS);
            cVideo.eMarkerEncoding = (VideoSettings::MarkerEncoding)SendMessage(GetDlgItem(hWnd, IDC_MARKERENC), CB_GETCURSEL, 0, 0);
            cVideo.bLimitFPS = (IsDlgButtonChecked(hWnd, IDC_LIMITFPS));
            cVideo.bDumpFrames = IsDlgButtonChecked(hWnd, IDC_FFMPEG);
            cVideo.bDebug = (IsDlgButtonChecked(hWnd, IDC_DEBUG));
            cVideo.bDisableUI = (IsDlgButtonChecked(hWnd, IDC_DISABLEUI));

            // Report success and return
            config.SetVideoSettings(cVideo);
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);

            // Save settings
            config.SaveConfigValues();
            return TRUE;
        }
        }
        break;
    }
    }
    return FALSE;
}

INT_PTR WINAPI ControlsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        // Config to fill out the form
        const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();

        HWND hWndFwdBack = GetDlgItem(hWnd, IDC_LRARROWS);
        HWND hWndSpeedPct = GetDlgItem(hWnd, IDC_UDARROWS);
        // Edit boxes
        TCHAR buf[32];
        _stprintf_s(buf, TEXT("%g"), cControls.dFwdBackSecs);
        SetWindowText(hWndFwdBack, buf);
        _stprintf_s(buf, TEXT("%g"), cControls.dSpeedUpPct);
        SetWindowText(hWndSpeedPct, buf);
        CheckDlgButton(hWnd, IDC_SHOWCONTROLS, cControls.bAlwaysShowControls);
        CheckDlgButton(hWnd, IDC_PHIGROS, cControls.bPhigros);
        SetDlgItemTextW(hWnd, IDC_SPLASHMIDI, cControls.sSplashMIDI.c_str());

        return TRUE;
    }
    case WM_COMMAND: {
        int iId = LOWORD(wParam);
        Changed(hWnd);
        switch (iId)
        {
        case IDC_SPLASHBROWSE: {
            OPENFILENAME ofn = {};
            TCHAR sFilename[1024] = { 0 };
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = TEXT("MIDI Files (*.mid, *.mid.xz)\0*.mid;*.xz\0");
            ofn.lpstrFile = sFilename;
            ofn.nMaxFile = sizeof(sFilename) / sizeof(TCHAR);
            ofn.lpstrTitle = OpenSplashFileTitle;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (GetOpenFileName(&ofn))
                SetDlgItemTextW(hWnd, IDC_SPLASHMIDI, sFilename);
            return TRUE;
        }
        case IDC_SPLASHRESET: {
            Changed(hWnd);
            SetDlgItemTextW(hWnd, IDC_SPLASHMIDI, L"");
            return TRUE;
        }
        }
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        switch (lpnmhdr->code)
        {
            // Spin boxes. Just increment or decrement
        case UDN_DELTAPOS:
            switch (lpnmhdr->idFrom)
            {
            case IDC_LRARROWSSPIN:
            {
                TCHAR buf[32];
                LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;
                HWND hWndFwdBack = GetDlgItem(hWnd, IDC_LRARROWS);
                double dOldVal = 0;
                int len = GetWindowText(hWndFwdBack, buf, 32);
                if (len > 0 && _stscanf_s(buf, TEXT("%lf"), &dOldVal) == 1)
                {
                    double dNewVal = dOldVal - lpnmud->iDelta * .1;
                    _stprintf_s(buf, TEXT("%g"), dNewVal);
                    SetWindowText(hWndFwdBack, buf);
                }
                return TRUE;
            }
            case IDC_UDARROWSSPIN:
            {
                TCHAR buf[32];
                LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;
                HWND hWndSpeedPct = GetDlgItem(hWnd, IDC_UDARROWS);
                double dOldVal = 0;
                int len = GetWindowText(hWndSpeedPct, buf, 32);
                if (len > 0 && _stscanf_s(buf, TEXT("%lf"), &dOldVal) == 1)
                {
                    double dNewVal = dOldVal - lpnmud->iDelta;
                    _stprintf_s(buf, TEXT("%g"), dNewVal);
                    SetWindowText(hWndSpeedPct, buf);
                }
                return TRUE;
            }
            }
            break;
            // OK or Apply button pressed
        case PSN_APPLY:
        {
            // Get a copy of the config to overwrite the settings
            Config& config = Config::GetConfig();
            ControlsSettings cControls = config.GetControlsSettings();

            // Edit boxes
            TCHAR buf[1<<10];
            double dEditVal = 0;

            HWND hWndFwdBack = GetDlgItem(hWnd, IDC_LRARROWS);
            int len = GetWindowText(hWndFwdBack, buf, 32);
            if (len > 0 && _stscanf_s(buf, TEXT("%lf"), &dEditVal) == 1)
                cControls.dFwdBackSecs = dEditVal;
            else
            {
                MessageBox(hWnd, TEXT("Please specify a numeric value! "), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
                PostMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)hWndFwdBack, TRUE);
                SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_INVALID);
                return TRUE;
            }

            HWND hWndSpeedPct = GetDlgItem(hWnd, IDC_UDARROWS);
            len = GetWindowText(hWndSpeedPct, buf, 32);
            if (len > 0 && _stscanf_s(buf, TEXT("%lf"), &dEditVal) == 1)
                cControls.dSpeedUpPct = dEditVal;
            else
            {
                MessageBox(hWnd, TEXT("Please specify a numeric value! "), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
                PostMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)hWndSpeedPct, TRUE);
                SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_INVALID);
                return TRUE;
            }

            cControls.bAlwaysShowControls = IsDlgButtonChecked(hWnd, IDC_SHOWCONTROLS);
            cControls.bPhigros = IsDlgButtonChecked(hWnd, IDC_PHIGROS);
            wchar_t splash[1 << 10]{};
            GetWindowTextW(GetDlgItem(hWnd, IDC_SPLASHMIDI), splash, 1 << 10);
            cControls.sSplashMIDI = splash;

            // Report success and return
            config.SetControlsSettings(cControls);
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);

            // Save settings
            config.SaveConfigValues();
            return TRUE;
        }
        }
        break;
    }
    }

    return FALSE;
}

BOOL GetCustomSettings(MainScreen* pGameState)
{
    INT_PTR iDlgResult = DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_TRACKSETTINGS),
        g_hWnd, TracksProc, (LPARAM)pGameState);
    return iDlgResult == IDOK;
}

INT_PTR WINAPI TracksProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static const VisualSettings& cVisual = Config::GetConfig().GetVisualSettings();
    static vector< bool > vMuted, vHidden; // Would rather be part of control, but no subitem lparam available
    static vector< unsigned > vColors;

    switch (msg)
    {
    case WM_INITDIALOG:
    {
        HWND hWndTracks = GetDlgItem(hWnd, IDC_TRACKS);
        TCHAR buf[MAX_PATH];
        SendMessage(hWndTracks, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_DOUBLEBUFFER);

        // Get data
        MainScreen* pGameState = (MainScreen*)lParam;
        const MIDI& midi = pGameState->GetMIDI();
        const MIDI::MIDIInfo& mInfo = midi.GetInfo();
        const vector< MIDITrack* >& vTracks = midi.GetTracks();
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pGameState);

        // Fill out the static text vals
        SetWindowText(GetDlgItem(hWnd, IDC_FILE), mInfo.sFilename.c_str() + mInfo.sFilename.find_last_of(L'\\') + 1);
        SetWindowText(GetDlgItem(hWnd, IDC_FOLDER), mInfo.sFilename.substr(0, mInfo.sFilename.find_last_of(L'\\')).c_str());
        _stprintf_s(buf, TEXT("%llu"), mInfo.iNoteCount);
        SetWindowText(GetDlgItem(hWnd, IDC_NOTES), buf);
        _stprintf_s(buf, TEXT("%lld:%02.0lf"), mInfo.llTotalMicroSecs / 60000000,
            (mInfo.llTotalMicroSecs % 60000000) / 1000000.0);
        SetWindowText(GetDlgItem(hWnd, IDC_LENGTH), buf);

        // Initialize the state vars
        vMuted.resize(mInfo.iNumChannels);
        vHidden.resize(mInfo.iNumChannels);
        vColors.resize(mInfo.iNumChannels);
        for (size_t i = 0; i < mInfo.iNumChannels; i++)
        {
            vMuted[i] = vHidden[i] = false;
            if (cVisual.bRandomizeColor) {
                vColors[i] = Util::RandColor();
            }
            else {
                vColors[i] = cVisual.colors[i % 16];
            }
        }

        // Set up the columns of the list view
        RECT rcTracks;
        GetClientRect(hWndTracks, &rcTracks);
        int aFmt[6] = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER };
        int aCx[6] = { 40, rcTracks.right - 50 * 5, 60, 50, 50, 50 };
        CONST TCHAR* aText[6] = { TEXT("Track"), TEXT("Instrument"), TEXT("Notes"), TEXT("Muted"), TEXT("Hidden"), TEXT("Color") };

        LVCOLUMN lvc = {};
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
        for (size_t i = 0; i < sizeof(aFmt) / sizeof(int); i++)
        {
            lvc.fmt = aFmt[i];
            lvc.cx = aCx[i];
            lvc.pszText = (TCHAR*)aText[i];
            SendMessage(hWndTracks, LVM_INSERTCOLUMN, i, (LPARAM)&lvc);
        }

        // Set rows of the list view
        LVITEM lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.pszText = buf;

        int iPos = 0;
        for (uint16_t i = 0; i < mInfo.iNumTracks; i++)
        {
            const MIDITrack::MIDITrackInfo& mTrackInfo = vTracks[i]->GetInfo();
            for (int j = 0; j < 16; j++)
                if (mTrackInfo.aNoteCount[j] > 0)
                {
                    lvi.iSubItem = 0;
                    _stprintf_s(buf, TEXT("%d"), iPos + 1);
                    lvi.iItem = (int)SendMessage(hWndTracks, LVM_INSERTITEM, 0, (LPARAM)&lvi);

                    lvi.iSubItem++;
                    _stprintf_s(buf, TEXT("%s"), j == 9 ? TEXT("Drums") : MIDI::Instruments[mTrackInfo.aProgram[j]].c_str());
                    SendMessage(hWndTracks, LVM_SETITEM, 0, (LPARAM)&lvi);

                    lvi.iSubItem++;
                    _stprintf_s(buf, TEXT("%llu"), mTrackInfo.aNoteCount[j]);
                    SendMessage(hWndTracks, LVM_SETITEM, 0, (LPARAM)&lvi);

                    lvi.iItem++;
                    iPos++;
                }
        }

        if (GetWindowLongPtr(hWndTracks, GWL_STYLE) & WS_VSCROLL)
            SendMessage(hWndTracks, LVM_SETCOLUMNWIDTH, 1, aCx[1] - 17);

        RECT rcItem = { LVIR_BOUNDS };
        SendMessage(hWndTracks, LVM_GETITEMRECT, 0, (LPARAM)&rcItem);

        if (Config::GetConfig().GetVideoSettings().bDumpFrames) {
            SendMessage(GetDlgItem(hWnd, IDC_NOLAG), BM_SETCHECK, BST_CHECKED, 0);
            EnableWindow(GetDlgItem(hWnd, IDC_NOLAG), FALSE);
        }
        else {
            SendMessage(GetDlgItem(hWnd, IDC_NOLAG), BM_SETCHECK, BST_UNCHECKED, 0);
            EnableWindow(GetDlgItem(hWnd, IDC_NOLAG), TRUE);
        }
        SendMessage(GetDlgItem(hWnd, IDC_PIANO), BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(GetDlgItem(hWnd, IDC_PIANO), TRUE);

        return TRUE;
    }
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        if (lpnmhdr->idFrom == IDC_TRACKS)
        {
            switch (lpnmhdr->code)
            {
                // Prevent's item selection
            case LVN_ITEMCHANGING:
            {
                LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
                if ((pnmv->uChanged & LVIF_STATE) &&
                    (pnmv->uNewState & LVIS_SELECTED) != (pnmv->uOldState & LVIS_SELECTED))
                {
                    SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
                    return TRUE;
                }
                break;
            }
            // Turn all on or all off or reset colors
            case LVN_COLUMNCLICK:
            {
                LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)lParam;
                bool bAllChecked = true;
                switch (lpnmlv->iSubItem)
                {
                case 3:
                    for (size_t i = 0; i < vMuted.size(); i++) bAllChecked &= vMuted[i];
                    for (size_t i = 0; i < vMuted.size(); i++) vMuted[i] = !bAllChecked;
                    InvalidateRect(lpnmlv->hdr.hwndFrom, NULL, FALSE);
                    return TRUE;
                case 4:
                    for (size_t i = 0; i < vHidden.size(); i++) bAllChecked &= vHidden[i];
                    for (size_t i = 0; i < vHidden.size(); i++) vHidden[i] = !bAllChecked;
                    InvalidateRect(lpnmlv->hdr.hwndFrom, NULL, FALSE);
                    return TRUE;
                case 5:
                    for (size_t i = 0; i < vColors.size(); i++)
                        if (cVisual.bRandomizeColor) {
                            vColors[i] = Util::RandColor();
                        }
                        else {
                            vColors[i] = cVisual.colors[i % 16];
                        }
                    InvalidateRect(lpnmlv->hdr.hwndFrom, NULL, FALSE);
                    return TRUE;
                }
                return TRUE;
            }
            // Toggle checkboxes or select color
            case NM_CLICK:
            case NM_DBLCLK:
            {
                // Have to manually figure out the corresponding item. Silly.
                LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lpnmhdr;
                LVHITTESTINFO lvhti = { lpnmia->ptAction };
                SendMessage(lpnmia->hdr.hwndFrom, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvhti);
                if (lvhti.iItem < 0 || lvhti.iItem >= (int)vMuted.size()) return FALSE;

                RECT rcItem = { LVIR_BOUNDS };
                SendMessage(lpnmia->hdr.hwndFrom, LVM_GETITEMRECT, lvhti.iItem, (LPARAM)&rcItem);
                switch (lvhti.iSubItem)
                {
                case 3:
                    vMuted[lvhti.iItem] = !vMuted[lvhti.iItem];
                    InvalidateRect(lpnmia->hdr.hwndFrom, &rcItem, FALSE);
                    return TRUE;
                case 4:
                    vHidden[lvhti.iItem] = !vHidden[lvhti.iItem];
                    InvalidateRect(lpnmia->hdr.hwndFrom, &rcItem, FALSE);
                    return TRUE;
                case 5:
                {
                    static COLORREF acrCustClr[16] = { 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF,
                                                       0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF };
                    CHOOSECOLOR cc = {};
                    cc.lStructSize = sizeof(cc);
                    cc.hwndOwner = hWnd;
                    cc.lpCustColors = (LPDWORD)acrCustClr;
                    cc.rgbResult = vColors[lvhti.iItem];
                    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
                    if (!ChooseColor(&cc)) return TRUE;
                    vColors[lvhti.iItem] = cc.rgbResult;
                    InvalidateRect(lpnmia->hdr.hwndFrom, &rcItem, FALSE);
                    return TRUE;
                }
                }
                break;
            }
            // Draw the checkboxes and colors
            case NM_CUSTOMDRAW:
            {
                LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
                LPNMLVCUSTOMDRAW lpnmlvcd = (LPNMLVCUSTOMDRAW)lParam;
                HWND hWndTracks = GetDlgItem(hWnd, IDC_TRACKS);
                RECT rcTracks;
                GetClientRect(hWndTracks, &rcTracks);
                int aCx[6] = { 40, rcTracks.right - 50 * 5, 60, 50, 50, 50 };
                for (int i = 0; i < sizeof(aCx) / sizeof(int); i++) {
                    SendMessage(hWndTracks, LVM_SETCOLUMNWIDTH, i, aCx[i]); //Please don't resize this, it makes the interface look very ridiculous! 
                }
                switch (lpnmcd->dwDrawStage)
                {
                case CDDS_PREPAINT: case CDDS_ITEMPREPAINT:
                    SetWindowLongPtr(hWnd, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW);
                    return TRUE;
                case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                    if (lpnmlvcd->iSubItem >= 3 && lpnmlvcd->iSubItem <= 5)
                    {
                        // Figure out size. Too big a rect is fine: will be clipped
                        RECT rcOut;
                        int iBmpSize = lpnmcd->rc.bottom - lpnmcd->rc.top - 2;
                        rcOut.left = lpnmcd->rc.left + (lpnmcd->rc.right - lpnmcd->rc.left - iBmpSize) / 2;
                        rcOut.top = lpnmcd->rc.top + (lpnmcd->rc.bottom - lpnmcd->rc.top - iBmpSize) / 2;
                        rcOut.right = rcOut.left + iBmpSize;
                        rcOut.bottom = rcOut.top + iBmpSize;

                        // Drawing. Checkbox, checkbox, color
                        if (lpnmlvcd->iSubItem == 3)
                            DrawFrameControl(lpnmcd->hdc, &rcOut, DFC_BUTTON,
                                DFCS_BUTTONCHECK | (vMuted[lpnmcd->dwItemSpec] ? DFCS_CHECKED : 0));
                        else if (lpnmlvcd->iSubItem == 4)
                            DrawFrameControl(lpnmcd->hdc, &rcOut, DFC_BUTTON,
                                DFCS_BUTTONCHECK | (vHidden[lpnmcd->dwItemSpec] ? DFCS_CHECKED : 0));
                        else
                        {
                            SetDCBrushColor(lpnmcd->hdc, lpnmlvcd->clrFace);
                            FillRect(lpnmcd->hdc, &lpnmcd->rc, (HBRUSH)GetStockObject(DC_BRUSH));
                            InflateRect(&lpnmcd->rc, -1, -1);
                            SetDCBrushColor(lpnmcd->hdc, vColors[lpnmcd->dwItemSpec]);
                            FillRect(lpnmcd->hdc, &lpnmcd->rc, (HBRUSH)GetStockObject(DC_BRUSH));
                            DrawEdge(lpnmcd->hdc, &lpnmcd->rc, BDR_SUNKENINNER, BF_RECT);
                        }
                        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, CDRF_SKIPDEFAULT);
                    }
                    else
                        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, CDRF_DODEFAULT);
                    return TRUE;
                }
                break;
            }
            }
        }
        break;
    }
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        switch (iId)
        {
        case IDOK:
        {
            MainScreen* pGameState = (MainScreen*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            pGameState->SetChannelSettings(vMuted, vHidden, vColors);

            Config::GetConfig().m_bManualTimer = IsDlgButtonChecked(hWnd, IDC_NOLAG);
            Config::GetConfig().m_bPianoOverride = IsDlgButtonChecked(hWnd, IDC_PIANO);
        }
        case IDCANCEL:
            EndDialog(hWnd, iId);
            return TRUE;
        }
        break;
    }
    case WM_CLOSE:
    {
        EndDialog(hWnd, IDCANCEL);
        return TRUE;
    }
    }

    return FALSE;
}
