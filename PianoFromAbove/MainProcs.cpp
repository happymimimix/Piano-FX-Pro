﻿/*************************************************************************************************
*
* File: MainProcs.cpp
*
* Description: Implements the main GUI functions. Mostly just window procs.
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <TChar.h>
#include <shlobj.h>
#include <Dbt.h>
#include <psapi.h>

#include <set>
#include <thread>

#include "MainProcs.h"
#include "ConfigProcs.h"
#include "Globals.h"
#include "resource.h"
#include "Language.h"

#include "GameState.h"
#include "Config.h"

static WNDPROC g_pPrevBarProc; // Have to override the toolbar proc to make controls transparent

VOID SizeWindows(int iMainWidth, int iMainHeight);
INT_PTR SetResolutionProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM) {
    switch (msg) {
    case WM_INITDIALOG: {
        RECT rect = {};
        GetWindowRect(g_hWndGfx, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        char buf[1 << 10] = {};

        _snprintf_s(buf, sizeof(buf), "%d", width);
        SetWindowTextA(GetDlgItem(hwnd, IDC_WIDTH), buf);
        _snprintf_s(buf, sizeof(buf), "%d", height);
        SetWindowTextA(GetDlgItem(hwnd, IDC_HEIGHT), buf);

        return true;
    }
    case WM_COMMAND: {
        int iId = LOWORD(wparam);
        switch (iId) {
        case IDOK: {
            char buf[1 << 10] = {};
            GetWindowTextA(GetDlgItem(hwnd, IDC_WIDTH), buf, sizeof(buf));
            int width = atoi(buf);
            GetWindowTextA(GetDlgItem(hwnd, IDC_HEIGHT), buf, sizeof(buf));
            int height = atoi(buf);

            if (Config::GetConfig().GetViewSettings().GetControls()) {
                RECT rcBarDlg;
                GetWindowRect(g_hWndBar, &rcBarDlg);
                height += rcBarDlg.bottom - rcBarDlg.top;
            }

            RECT adjusted = { 0, 0, width, height };
            AdjustWindowRectEx(&adjusted, GetWindowLongA(g_hWnd, GWL_STYLE), TRUE, GetWindowLongA(g_hWnd, GWL_EXSTYLE));
            SetWindowPos(g_hWnd, NULL, 0, 0, adjusted.right - adjusted.left, adjusted.bottom - adjusted.top, SWP_NOMOVE);

            [[clang::fallthrough]];
        }
        case IDCANCEL: {
            EndDialog(hwnd, iId);
            return TRUE;
        }
        }
        return false;
    }
    case WM_CLOSE: {
        EndDialog(hwnd, IDCANCEL);
        return true;
    }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static PlaybackSettings& cPlayback = Config::GetConfig().GetPlaybackSettings();
    static ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    static bool bInSizeMove = false;

    switch (msg)
    {
    case WM_WINDOWPOSCHANGING:
        // Allow the window to be larger than the screen. 
        return 0;
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        switch (iId)
        {
        case IDOK:
        {
            return 0;
        }
        case ID_FILE_PRACTICE:
        {
            CheckActivity(TRUE);
            // Get the file(s) to add
            OPENFILENAME ofn = {};
            TCHAR sFilename[1 << 10] = { 0 };
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = TEXT("MIDI Files (*.mid, *.mid.xz)\0*.mid;*.xz\0");
            ofn.lpstrFile = sFilename;
            ofn.nMaxFile = sizeof(sFilename) / sizeof(TCHAR);
            ofn.lpstrTitle = OpenFileTitle;
            ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (GetOpenFileName(&ofn)) {
                if (cPlayback.GetPlayMode()) { //Close the current file first before opening another one! 
                    cPlayback.SetPlayMode(GameState::Intro, true);
                    cPlayback.SetPlayable(false, true);
                    cPlayback.SetPosition(0);
                    SetWindowText(g_hWnd, MainWindowTitle1 L" v" LVersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle5 MainWindowTitle7);
                    HandOffMsg(WM_COMMAND, ID_CHANGESTATE, (LPARAM)new IntroScreen(NULL, NULL));
                }
                PlayFile(sFilename);
            }
            return 0;
        }
        case ID_FILE_CLOSEFILE:
        {
            if (!cPlayback.GetPlayMode()) break;
            cPlayback.SetPlayMode(GameState::Intro, true);
            cPlayback.SetPlayable(false, true);
            cPlayback.SetPosition(0);
            SetWindowText(g_hWnd, MainWindowTitle1 L" v" LVersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle5 MainWindowTitle7);
            HandOffMsg(WM_COMMAND, ID_CHANGESTATE, (LPARAM)new IntroScreen(NULL, NULL));
            return 0;
        }
        case ID_PLAY_PLAY:
            if (cPlayback.GetPlayMode() && iId == ID_PLAY_PLAY) cPlayback.SetPaused(false, true);
            return 0;
        case ID_PLAY_PAUSE:
            if (cPlayback.GetPlayMode()) cPlayback.SetPaused(true, true);
            return 0;
        case ID_PLAY_PLAYPAUSE:
            if (cPlayback.GetPlayMode()) cPlayback.TogglePaused(true);
            return 0;
        case ID_PLAY_STOP:
            if (cPlayback.GetPlayMode()) HandOffMsg(msg, wParam, lParam);
            return 0;
        case ID_PLAY_SKIPFWD: case ID_PLAY_SKIPBACK:
            if (cPlayback.GetPlayMode()) HandOffMsg(msg, wParam, lParam);
            return 0;
        case ID_PLAY_INCREASERATE:
            cPlayback.SetSpeed(cPlayback.GetSpeed() * (1.0 + cControls.dSpeedUpPct / 100.0), true);
            return 0;
        case ID_PLAY_DECREASERATE:
            cPlayback.SetSpeed(cPlayback.GetSpeed() / (1.0 + cControls.dSpeedUpPct / 100.0), true);
            return 0;
        case ID_PLAY_RESETRATE:
            cPlayback.SetSpeed(1.0, true);
            return 0;
        case ID_PLAY_NFASTER:
            cPlayback.SetNSpeed(cPlayback.GetNSpeed() / (1.0 + cControls.dSpeedUpPct / 100.0), true);
            return 0;
        case ID_PLAY_NSLOWER:
            cPlayback.SetNSpeed(cPlayback.GetNSpeed() * (1.0 + cControls.dSpeedUpPct / 100.0), true);
            return 0;
        case ID_PLAY_NRESET:
            cPlayback.SetNSpeed(0.25, true);
            return 0;
        case ID_PLAY_VOLUMEUP:
            cPlayback.SetVolume(min(cPlayback.GetVolume() + 0.1, 1.0), true);
            return 0;
        case ID_PLAY_VOLUMEDOWN:
            cPlayback.SetVolume(max(cPlayback.GetVolume() - 0.1, 0.0), true);
            return 0;
        case ID_PLAY_MUTE:
            cPlayback.ToggleMute(true);
            return 0;
        case ID_VIEW_CONTROLS:
            cView.ToggleControls(true);
            return 0;
        case ID_VIEW_KEYBOARD:
            cView.ToggleKeyboard(true);
            return 0;
        case ID_VIEW_ALWAYSONTOP:
            cView.ToggleOnTop(true);
            return 0;
        case ID_VIEW_FULLSCREEN:
            cView.ToggleFullScreen(true);
            return 0;
        case ID_VIEW_MOVEANDZOOM:
            HandOffMsg(msg, wParam, lParam);
            return 0;
        case ID_VIEW_RESETMOVEANDZOOM:
            HandOffMsg(msg, wParam, lParam);
            return 0;
        case ID_VIEW_SETWINDOWSIZE:
            DialogBox(NULL, MAKEINTRESOURCE(IDD_SETRESOLUTION), g_hWnd, SetResolutionProc);
            return 0;
        case ID_OPTIONS_PREFERENCES:
            CheckActivity(TRUE);
            DoPreferences(hWnd);
            return 0;
        case ID_HELP_ABOUT:
            DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), g_hWnd, AboutProc);
            return 0;
        case ID_GAMEERROR:
            MessageBoxW(hWnd, GameState::Errors[lParam].c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION);
            return 0;
        case ID_STUDIO:
            char ProgramPath[MAX_PATH + 1] = {};
            GetModuleFileNameA(NULL, ProgramPath, MAX_PATH);
            string Command = "start \"Piano-FX Studio v";
            Command += VersionString;
            Command += "\" \"";
            Command += ProgramPath;
            Command += "\" ";
            Command += "OPEN PFXSTUDIO";
            system(Command.c_str());
            return 0;
        }
        break;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) != WA_INACTIVE)
            SetFocus(g_hWndGfx);
        return  0;
    case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
        {
            if (cPlayback.GetPlayMode() && !cPlayback.GetPaused())
                return 0;
        }
        break;
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
        lpmmi->ptMinTrackSize.x = 640;
        lpmmi->ptMinTrackSize.y = 480;
        return 0;
    }
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED) return 0;
        SizeWindows(LOWORD(lParam), HIWORD(lParam));

        if (wParam != SIZE_MAXIMIZED && !cView.GetFullScreen())
        {
            RECT rcMain;
            GetWindowRect(hWnd, &rcMain);
            cView.SetMainSize(rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);
        }
        if (!bInSizeMove)
            HandOffMsg(WM_COMMAND, ID_VIEW_RESETDEVICE, 0);
        return 0;
    case WM_MOVE:
    {
        RECT rcMain;
        GetWindowRect(hWnd, &rcMain);
        cView.SetMainPos(rcMain.left, rcMain.top);
        return 0;
    }
    case WM_ENTERSIZEMOVE:
        bInSizeMove = true;
        return 0;
    case WM_EXITSIZEMOVE:
        HandOffMsg(WM_COMMAND, ID_VIEW_RESETDEVICE, 0);
        bInSizeMove = false;
        return 0;
    case WM_DEVICECHANGE:
        Config::GetConfig().LoadMIDIDevices();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_DROPFILES:
        if (!wParam)
            return 0;
        auto drop = (HDROP)wParam;
        // only allow 1 file
        if (DragQueryFile(drop, 0xFFFFFFFF, NULL, 0) != 1)
            return 0;
        // it's 2020, so no MAX_PATH!
        std::vector<wchar_t> filename;
        filename.resize(DragQueryFile(drop, 0, NULL, 0) + 1);
        DragQueryFile(drop, 0, filename.data(), filename.size());
        PlayFile(filename.data());
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Used in place of GetMenu because the menu gets detached when in full screen
HMENU GetMainMenu()
{
    static HMENU hMenu = NULL;
    if (!hMenu) hMenu = GetMenu(g_hWnd);
    return hMenu;
}

VOID SizeWindows(int iMainWidth, int iMainHeight)
{
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    int iBarHeight = 0, iLibWidth = 0;
    UINT swpFlags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER;

    if (!iMainWidth || !iMainHeight)
    {
        RECT rcMain;
        GetClientRect(g_hWnd, &rcMain);
        iMainWidth = rcMain.right;
        iMainHeight = rcMain.bottom;
    }

    HDWP hdwp = BeginDeferWindowPos(cView.GetControls() + 1);
    if (cView.GetControls())
    {
        RECT rcBarDlg;
        GetWindowRect(g_hWndBar, &rcBarDlg);
        iBarHeight = rcBarDlg.bottom - rcBarDlg.top;
        if (hdwp) hdwp = DeferWindowPos(hdwp, g_hWndBar, NULL, 0, 0, iMainWidth, iBarHeight, swpFlags);
    }
    if (cView.GetFullScreen() && !cControls.bAlwaysShowControls) iBarHeight = 0;
    if (cView.GetFullScreen()) iLibWidth = 0;
    if (hdwp) hdwp = DeferWindowPos(hdwp, g_hWndGfx, NULL, iLibWidth, iBarHeight, iMainWidth - iLibWidth, iMainHeight - iBarHeight, swpFlags);
    if (hdwp) EndDeferWindowPos(hdwp);
}

LRESULT WINAPI GfxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Static vars for poping up the lib and bar in full screen mode
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    static bool bShowBar;
    static int iBarHeight;

    static bool bTrack = false, bTrackL = false, bTrackR = false;

    if ((msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) || (msg >= WM_KEYFIRST && msg <= WM_KEYLAST) ||
        msg == WM_CAPTURECHANGED || msg == WM_MOUSELEAVE)
        HandOffMsg(msg, wParam, lParam);

    switch (msg)
    {
    case WM_CREATE:
        ShowKeyboard(cView.GetKeyboard());
        return 0;
    case WM_LBUTTONDOWN: case WM_RBUTTONDOWN:
        SetFocus(hWnd);
        if (!bTrackR && !bTrackL) SetCapture(hWnd);
        if (msg == WM_LBUTTONDOWN) bTrackL = true;
        else bTrackR = true;
        return 0;
    case WM_LBUTTONUP: case WM_RBUTTONUP:
        if (msg == WM_LBUTTONUP) bTrackL = false;
        else bTrackR = false;
        if (!bTrackR && !bTrackL) ReleaseCapture();
        if (cView.GetZoomMove()) return 0;
        break;
    case WM_CAPTURECHANGED:
        bTrackR = bTrackL = false;
        return 0;
    case WM_CONTEXTMENU:
    {
        POINT ptContext = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        HWND hWndContext = (HWND)wParam;
        if (hWndContext != g_hWndGfx || cView.GetZoomMove()) break;

        // VK_APPS or Shift F10 were pressed. Figure out where to display the menu
        if (ptContext.x < 0 && ptContext.y < 0)
        {
            RECT rcGfx;
            POINT ptMouse;
            GetCursorPos(&ptMouse);
            GetWindowRect(g_hWndGfx, &rcGfx);

            if (PtInRect(&rcGfx, ptMouse))
                ptContext = ptMouse;
            else
            {
                ptContext.x = rcGfx.left;
                ptContext.y = rcGfx.top;
            }
        }
        return 0;
    }
    // Send me everything except the tab character
    case WM_GETDLGCODE:
    {
        LRESULT lr = DLGC_WANTARROWS | DLGC_WANTCHARS;
        if (lParam && wParam != VK_TAB) lr |= DLGC_WANTMESSAGE;
        return lr;
    }
    // Pop up the library/controls if mouse is close enough
    case WM_MOUSEMOVE:
    {
        if (!bTrack)
        {
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, HOVER_DEFAULT };
            TrackMouseEvent(&tme);
            bTrack = true;
        }

        short iXPos = LOWORD(lParam);
        short iYPos = HIWORD(lParam);
        POINT pt = { iXPos, iYPos };
        ClientToScreen(hWnd, &pt);
        CheckActivity(TRUE, &pt);

        if (cView.GetFullScreen())
        {
            if (!iBarHeight)
            {
                RECT rcBar;
                GetWindowRect(g_hWndBar, &rcBar);
                iBarHeight = rcBar.bottom - rcBar.top;
            }

            if (!cControls.bAlwaysShowControls)
            {
                if (iYPos < iBarHeight && cView.GetControls())
                {
                    ShowWindow(g_hWndBar, SW_SHOWNA);
                    bShowBar = true;
                }
                else if (bShowBar)
                {
                    ShowWindow(g_hWndBar, SW_HIDE);
                    SetFocus(g_hWndGfx);
                    bShowBar = false;
                }
            }
        }
        break;
    }
    case WM_MOUSELEAVE:
        bTrack = false;
        break;
    case WM_DESTROY:
        g_bGfxDestroyed = true;
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID CopyMenuState(HMENU hMenuSrc, HMENU hMenuDest)
{
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STATE | MIIM_CHECKMARKS | MIIM_ID;
    int iCount = GetMenuItemCount(hMenuSrc);
    for (int i = 0; i < iCount; i++)
        if (GetMenuItemInfo(hMenuSrc, i, TRUE, &mii))
            SetMenuItemInfo(hMenuDest, mii.wID, FALSE, &mii);
}

// Override of the toolbar proc to draw transparent controls
LRESULT WINAPI BarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static PlaybackSettings& cPlayback = Config::GetConfig().GetPlaybackSettings();

    switch (msg)
    {
        // WM_HSCROLL doesn't percolate up, so got to handle it here
    case WM_HSCROLL:
    {
        int iCode = LOWORD(wParam);
        if (iCode == TB_LINEUP || iCode == TB_LINEDOWN || iCode == TB_PAGEUP ||
            iCode == TB_PAGEDOWN || iCode == TB_THUMBTRACK || iCode == TB_THUMBPOSITION)
        {
            HWND hWndTrackbar = (HWND)lParam;
            int iPos = (iCode == TB_THUMBTRACK || iCode == TB_THUMBPOSITION ? HIWORD(wParam) : (int)SendMessage(hWndTrackbar, TBM_GETPOS, 0, 0));
            int iId = GetDlgCtrlID(hWndTrackbar);
            switch (iId)
            {
            case IDC_VOLUME:
                cPlayback.SetVolume(iPos / 100.0, false);
                return 0;
            case IDC_SPEED:
                if (iPos < 108 && iPos > 92 && iPos != 100) cPlayback.SetSpeed(1.0, true);
                else cPlayback.SetSpeed(iPos / 100.0, false);
                return 0;
            case IDC_NSPEED:
                if (iPos < 108 && iPos > 92 && iPos != 100) cPlayback.SetNSpeed(1.0, true);
                else cPlayback.SetNSpeed((200 - iPos) / 100.0, false);
                return 0;
            }
        }
        break;
    }
    // This makes our static text have a transparent background
    case WM_CTLCOLORSTATIC:
    {
        HDC hDC = (HDC)wParam;
        SetBkMode(hDC, TRANSPARENT);
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    // This is for slider controls. Draw our own channel
    case WM_NOTIFY:
    {
        LPNMHDR lpnmhdr = (LPNMHDR)lParam;
        if (lpnmhdr->code == NM_CUSTOMDRAW &&
            (lpnmhdr->idFrom == IDC_VOLUME || lpnmhdr->idFrom == IDC_SPEED || lpnmhdr->idFrom == IDC_NSPEED))
        {
            LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
            switch (lpnmcd->dwDrawStage)
            {
            case CDDS_PREPAINT:
                return CDRF_NOTIFYITEMDRAW;
            case CDDS_ITEMPREPAINT:
                if (lpnmcd->dwItemSpec != TBCD_CHANNEL)
                    return CDRF_DODEFAULT;
                DrawSliderChannel(lpnmcd, hWnd);
                return CDRF_DODEFAULT; // Still want it to draw the channel as per normal
            }
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        short iXPos = LOWORD(lParam);
        short iYPos = HIWORD(lParam);
        POINT pt = { iXPos, iYPos };
        ClientToScreen(hWnd, &pt);
        CheckActivity(TRUE, &pt);
        break;
    }
    }

    return CallWindowProc(g_pPrevBarProc, hWnd, msg, wParam, lParam);
}

HWND CreateRebar(HWND hWndOwner)
{
    // Create the Rebar. Just houses the toolbar.
    HWND hWndRebar = CreateWindowEx(WS_EX_CONTROLPARENT, REBARCLASSNAME, NULL, WS_CHILD | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NODIVIDER | RBS_VARHEIGHT, 0, 0, 0, 0, hWndOwner, (HMENU)IDC_TOPREBAR, g_hInstance, NULL);
    if (!hWndRebar) return NULL;

    // Create the system font
    HDC hDC = GetDC(hWndOwner);
    HFONT hFont = CreateFont(
        12, //FontHeight
        0, //FontWidth
        0, //Escapement
        0, //Orientation
        0, //FontWeight
        0, //Italic
        0, //Underline
        0, //StrikeOut
        1, //CharSet
        0, //OutPrecision
        0, //ClipPrecision
        0, //Quality
        0, //PitchAndFamily
        L"Tahoma" //FaceName
    );
    ReleaseDC(hWndOwner, hDC);

    // Create and load the button icons
    HIMAGELIST hIml = ImageList_LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_MEDIAICONSSMALL), 1 << 4, (1 << 4) + (1 << 2), RGB(255, 255, 0), IMAGE_BITMAP, LR_CREATEDIBSECTION);

    // Create the toolbar. Houses custom controls too. Don't want multiple rebar brands because you lose too much control
    HWND hWndToolbar = CreateWindowEx(WS_EX_CONTROLPARENT, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_TABSTOP | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, hWndRebar, (HMENU)IDC_TOPTOOLBAR, g_hInstance, NULL);
    if (hWndToolbar == NULL)
        return NULL;

#define hWndVolumePlaceholder 999
#define hWndSpeedPlaceholder 998
#define hWndNSpeedPlaceholder 997

    TBBUTTON tbButtons[1 << 4] =
    {
        { MAKELONG(0, 0), ID_PLAY_PLAY, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Play") },
        { MAKELONG(1, 0), ID_PLAY_PAUSE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Pause") },
        { MAKELONG(2, 0), ID_PLAY_STOP, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Stop") },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, NULL },
        { MAKELONG(3, 0), ID_PLAY_SKIPBACK, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Skip Back") },
        { MAKELONG(4, 0), ID_PLAY_SKIPFWD, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Skip Fwd") },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, NULL },
        { MAKELONG(5, 0), ID_PLAY_MUTE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Mute") },
        { I_IMAGENONE, hWndVolumePlaceholder, 0, BTNS_BUTTON, {0}, 0, NULL },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, NULL },
        { MAKELONG(7, 0), ID_PLAY_RESETRATE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Reset Playback Speed") },
        { I_IMAGENONE, hWndSpeedPlaceholder, 0, BTNS_BUTTON, {0}, 0, NULL },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, NULL },
        { MAKELONG(8, 0), ID_PLAY_NRESET, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)TEXT("Reset Note Speed") },
        { I_IMAGENONE, hWndNSpeedPlaceholder, 0, BTNS_BUTTON, {0}, 0, NULL },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, NULL }
    };

    // First add the toolbar buttons
    g_pPrevBarProc = (WNDPROC)SetWindowLongPtr(hWndToolbar, GWLP_WNDPROC, (LONG_PTR)BarProc);
    SendMessage(hWndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hIml);
    SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, sizeof(tbButtons) / sizeof(TBBUTTON), (LPARAM)&tbButtons);
    SendMessage(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(1 << 5, 1 << 5));
    TBBUTTONINFO tbbi = { sizeof(TBBUTTONINFO) };
    tbbi.dwMask = TBIF_SIZE;
    tbbi.cx = 100;
    SendMessage(hWndToolbar, TB_SETBUTTONINFO, hWndVolumePlaceholder, (LPARAM)&tbbi);
    SendMessage(hWndToolbar, TB_SETBUTTONINFO, hWndSpeedPlaceholder, (LPARAM)&tbbi);
    SendMessage(hWndToolbar, TB_SETBUTTONINFO, hWndNSpeedPlaceholder, (LPARAM)&tbbi);

    RECT SliderPos;
    // Now add the other controls
    SendMessage(hWndToolbar, TB_GETITEMRECT, 8, (LPARAM)&SliderPos);
    HWND hWndVolume = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_BOTH | TBS_NOTICKS, SliderPos.left, SliderPos.top + 4, SliderPos.right - SliderPos.left, SliderPos.bottom - SliderPos.top - 4, hWndToolbar, (HMENU)IDC_VOLUME, g_hInstance, NULL);
    SendMessage(hWndVolume, TBM_SETRANGE, FALSE, MAKELONG(0, 100));
    SendMessage(hWndVolume, TBM_SETLINESIZE, 0, 5);

    SendMessage(hWndToolbar, TB_GETITEMRECT, 11, (LPARAM)&SliderPos);
    HWND hWndSpeed = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_BOTH | TBS_NOTICKS, SliderPos.left, SliderPos.top + 4, SliderPos.right - SliderPos.left, SliderPos.bottom - SliderPos.top - 4, hWndToolbar, (HMENU)IDC_SPEED, g_hInstance, NULL);
    SendMessage(hWndSpeed, TBM_SETRANGE, FALSE, MAKELONG(5, 195));
    SendMessage(hWndSpeed, TBM_SETLINESIZE, 0, 10);

    SendMessage(hWndToolbar, TB_GETITEMRECT, 14, (LPARAM)&SliderPos);
    HWND hWndNSpeed = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | TBS_BOTH | TBS_NOTICKS, SliderPos.left, SliderPos.top + 4, SliderPos.right - SliderPos.left, SliderPos.bottom - SliderPos.top - 4, hWndToolbar, (HMENU)IDC_NSPEED, g_hInstance, NULL);
    SendMessage(hWndNSpeed, TBM_SETRANGE, FALSE, MAKELONG(5, 195));
    SendMessage(hWndNSpeed, TBM_SETLINESIZE, 0, 10);

    HWND hWndPosn = CreateWindowEx(0, POSNCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWndRebar, (HMENU)IDC_POSNCTRL, g_hInstance, NULL);

    // Set the font to the dialog font
    SendMessage(hWndVolume, WM_SETFONT, (WPARAM)hFont, FALSE);
    SendMessage(hWndSpeed, WM_SETFONT, (WPARAM)hFont, FALSE);
    SendMessage(hWndNSpeed, WM_SETFONT, (WPARAM)hFont, FALSE);

    REBARBANDINFO rbbi;
    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_TEXT;

    rbbi.fStyle = RBBS_NOGRIPPER | RBBS_VARIABLEHEIGHT;
    rbbi.lpText = (LPWSTR)TEXT("");
    rbbi.hwndChild = hWndToolbar;
    rbbi.cxMinChild = rbbi.cyIntegral = 0;
    rbbi.cyMinChild = rbbi.cyChild = rbbi.cyMaxChild = 1 << 5;
    SendMessage(hWndRebar, RB_INSERTBAND, -1, (LPARAM)&rbbi);

    rbbi.fStyle = RBBS_NOGRIPPER | RBBS_VARIABLEHEIGHT | RBBS_BREAK;
    rbbi.lpText = (LPWSTR)TEXT("");
    rbbi.hwndChild = hWndPosn;
    rbbi.cyMinChild = rbbi.cyChild = rbbi.cyMaxChild = 1 << 4;
    SendMessage(hWndRebar, RB_INSERTBAND, -1, (LPARAM)&rbbi);

    // Now that the controls are created and set up, fill out the default values
    Config& config = Config::GetConfig();
    const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    g_hWndBar = hWndRebar; // SetMute needs it :/
    SetMute(cPlayback.GetMute());
    SendMessage(hWndSpeed, TBM_SETPOS, TRUE, (LONG)(100 * cPlayback.GetSpeed() + .5));
    SendMessage(hWndNSpeed, TBM_SETPOS, TRUE, (LONG)(100 * (2.0 - cPlayback.GetNSpeed()) + .5));
    SendMessage(hWndVolume, TBM_SETPOS, TRUE, (LONG)(100 * cPlayback.GetVolume() + .5));

    return hWndRebar;
}

// Transparent slider. Draw the parent's background into a mem DC, blit to screen.
VOID DrawSliderChannel(LPNMCUSTOMDRAW lpnmcd, HWND hWndOwner)
{
    static HDC hdcMem = NULL;
    static HBITMAP hBitmap = NULL;

    RECT rcCtrl, rcOwner;
    GetWindowRect(lpnmcd->hdr.hwndFrom, &rcCtrl);
    GetWindowRect(hWndOwner, &rcOwner);
    RECT rcCtrlClient = rcCtrl, rcOwnerClient = rcOwner;
    OffsetRect(&rcCtrlClient, -rcCtrl.left, -rcCtrl.top);
    OffsetRect(&rcOwnerClient, -rcOwner.left, -rcOwner.top);

    // Only make the copy of the parent once
    // ASSUMES THE SAME PARENT! Function will need to change if it ever gets called by more than one Proc
    if (!hdcMem && !hBitmap)
    {
        hdcMem = CreateCompatibleDC(lpnmcd->hdc);
        hBitmap = CreateCompatibleBitmap(lpnmcd->hdc, rcOwner.right - rcOwner.left, rcOwner.bottom - rcOwner.top);
        SelectObject(hdcMem, hBitmap);
        FillRect(hdcMem, &rcOwnerClient, (HBRUSH)(COLOR_BTNFACE + 1)); // In case we're on V5
        SendMessage(hWndOwner, WM_PRINTCLIENT, (WPARAM)hdcMem, PRF_CLIENT | PRF_NONCLIENT); // V6 Controls only
    }

    BitBlt(lpnmcd->hdc, 0, 0, rcCtrl.right - rcCtrl.left, rcCtrl.bottom - rcCtrl.top,
        hdcMem, rcCtrl.left - rcOwner.left, rcCtrl.top - rcOwner.top, SRCCOPY);
    if (GetFocus() == lpnmcd->hdr.hwndFrom)
        DrawFocusRect(lpnmcd->hdc, &rcCtrlClient);
}

LRESULT WINAPI PosnProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Bad design. Will need to be moved into a struct if we need more than 1 in a program
    static BOOL bEnabled = TRUE, bTracking = FALSE;
    static int iPosition = 0;
    static HIMAGELIST hIml = NULL;
    static HBITMAP hBackbuffer = NULL;
    static HBITMAP hBackground = NULL;

    switch (msg)
    {
    case WM_CREATE:
        hIml = ImageList_LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_MEDIAICONSSMALL), 1 << 4, (1 << 4) + (1 << 2), CLR_DEFAULT, IMAGE_BITMAP, LR_CREATEDIBSECTION);
        bEnabled = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_DISABLED) == 0);
        return 0;
    case WM_ENABLE:
    {
        bEnabled = (BOOL)wParam;
        if (!bEnabled)
        {
            iPosition = 0;
            if (bTracking)
            {
                bTracking = FALSE;
                ReleaseCapture();
            }
        }

        RECT rc;
        SendMessage(g_hWndBar, RB_GETRECT, 1, (LPARAM)&rc);
        RedrawWindow(g_hWndBar, &rc, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
        return 0;
    }
    // 3 Draws, but complicated in order to prevent flicker
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        HDC hDCMem = CreateCompatibleDC(hDC);
        HDC hDCBkg = CreateCompatibleDC(hDCMem);

        RECT rcCtrl, rcOwner;
        GetWindowRect(hWnd, &rcCtrl);
        GetWindowRect(g_hWndBar, &rcOwner);
        RECT rcCtrlClient = rcCtrl, rcOwnerClient = rcOwner;
        OffsetRect(&rcCtrlClient, -rcCtrl.left, -rcCtrl.top);
        OffsetRect(&rcOwnerClient, -rcOwner.left, -rcOwner.top);

        // Create bitmaps if needed. Get background.
        HGDIOBJ hbmpOld2 = NULL;
        if (!hBackbuffer)
        {
            hBackbuffer = CreateCompatibleBitmap(hDC, rcCtrlClient.right, rcCtrlClient.bottom);

            hBackground = CreateCompatibleBitmap(hDC, rcOwner.right - rcOwner.left, rcOwner.bottom - rcOwner.top);
            hbmpOld2 = SelectObject(hDCBkg, hBackground);
            FillRect(hDCBkg, &rcOwnerClient, (HBRUSH)(COLOR_BTNFACE + 1)); // In case we're on V5
            SendMessage(g_hWndBar, WM_PRINT, (WPARAM)hDCBkg, PRF_CLIENT | PRF_NONCLIENT | PRF_ERASEBKGND); // V6 Controls only
        }
        else
            hbmpOld2 = SelectObject(hDCBkg, hBackground);
        HGDIOBJ hbmpOld1 = SelectObject(hDCMem, hBackbuffer);

        // Channel and thumb rects
        RECT rcChannel, rcThumb;
        GetChannelRect(hWnd, &rcChannel);
        GetThumbRect(hWnd, iPosition, &rcChannel, &rcThumb);

        // Copy background and draw
        BitBlt(hDCMem, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
            hDCBkg, rcCtrl.left - rcOwner.left + ps.rcPaint.left, rcCtrl.top - rcOwner.top + ps.rcPaint.top, SRCCOPY);
        if (bEnabled)
        {
            SetDCBrushColor(hDCMem, RGB(255, 255, 255));
            HBRUSH hBrush = (HBRUSH)GetStockObject(DC_BRUSH);
            FillRect(hDCMem, &rcChannel, hBrush);
        }
        DrawEdge(hDCMem, &rcChannel, BDR_SUNKENOUTER, BF_RECT);
        ImageList_DrawEx(hIml, 9 + bEnabled, hDCMem, rcThumb.left, rcThumb.top, rcThumb.right - rcThumb.left, rcThumb.bottom - rcThumb.top,
            CLR_DEFAULT, CLR_DEFAULT, ILD_NORMAL);
        BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
            hDCMem, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

        SelectObject(hDCBkg, hbmpOld2);
        DeleteDC(hDCBkg);
        SelectObject(hDCMem, hbmpOld1);
        DeleteDC(hDCMem);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_SIZE:
        if (hBackbuffer != NULL) DeleteObject((HGDIOBJ)hBackbuffer);
        if (hBackground != NULL) DeleteObject((HGDIOBJ)hBackground);
        hBackbuffer = NULL;
        hBackground = NULL;
        return 0;
    case WM_LBUTTONDOWN:
    {
        if (!bEnabled) return 0;
        POINT pt = { (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) };

        RECT rcChannel, rcThumb;
        GetChannelRect(hWnd, &rcChannel);
        GetThumbRect(hWnd, iPosition, &rcChannel, &rcThumb);
        InflateRect(&rcChannel, 0, 4);

        if (PtInRect(&rcChannel, pt) || PtInRect(&rcThumb, pt))
        {
            int iPositionNew = GetThumbPosition((SHORT)pt.x, &rcChannel);
            MoveThumbPosition(iPositionNew, iPosition, hWnd, &rcChannel, &rcThumb);
            bTracking = TRUE;
            SetCapture(hWnd);
        }
        return 0;
    }
    case WM_CAPTURECHANGED:
        bTracking = false;
        return 0;
    case WM_LBUTTONUP:
        if (bTracking) ReleaseCapture();
        bTracking = FALSE;
        return 0;
    case WM_MOUSEMOVE:
    {
        if (!bTracking) return 0;

        short iXPos = LOWORD(lParam);
        RECT rcChannel, rcThumbOld;
        GetChannelRect(hWnd, &rcChannel);
        GetThumbRect(hWnd, iPosition, &rcChannel, &rcThumbOld);

        int iPositionNew = GetThumbPosition(iXPos, &rcChannel);
        MoveThumbPosition(iPositionNew, iPosition, hWnd, &rcChannel, &rcThumbOld);
        return 0;
    }
    case TBM_SETPOS:
    {
        RECT rcChannel, rcThumbOld;
        GetChannelRect(hWnd, &rcChannel);
        GetThumbRect(hWnd, iPosition, &rcChannel, &rcThumbOld);
        int iPositionNew = max(min((int)lParam, 1000), 0);
        MoveThumbPosition(iPositionNew, iPosition, hWnd, &rcChannel, &rcThumbOld, FALSE);
        return 0;
    }
    case WM_DESTROY:
        if (hBackbuffer != NULL) DeleteObject((HGDIOBJ)hBackbuffer);
        if (hBackground != NULL) DeleteObject((HGDIOBJ)hBackground);
        ImageList_Destroy(hIml);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID GetChannelRect(HWND hWnd, RECT* rcChannel)
{
    static const int iSize = 7;

    GetClientRect(hWnd, rcChannel);
    InflateRect(rcChannel, -iSize, -(rcChannel->bottom - rcChannel->top - iSize) / 2);
    rcChannel->right--;
    if (rcChannel->bottom - rcChannel->top == iSize + 1)
        rcChannel->bottom--;
}

VOID GetThumbRect(HWND, int iPosition, const RECT* rcChannel, RECT* rcThumb)
{
    int iSize = rcChannel->bottom - rcChannel->top;
    int iPixel = (2 * iPosition * (rcChannel->right - rcChannel->left - 1) + 1000) / (2 * 1000);
    rcThumb->left = rcChannel->left + iPixel - iSize / 2 - 3;
    rcThumb->top = rcChannel->top - 4;
    rcThumb->right = rcThumb->left + iSize + 6;
    rcThumb->bottom = rcThumb->top + iSize + 8;
}

INT GetThumbPosition(short iXPos, RECT* rcChannel)
{
    int iPositionNew = (2 * 1000 * (iXPos - rcChannel->left) + rcChannel->right - rcChannel->left) / (2 * (rcChannel->right - rcChannel->left));
    iPositionNew = max(min(iPositionNew, 1000), 0);
    return iPositionNew;
}

VOID MoveThumbPosition(int iPositionNew, int& iPosition, HWND hWnd, RECT* rcChannel, RECT* rcThumbOld, BOOL bUpdateGame)
{
    RECT rcThumbNew, rcInvalid;
    GetThumbRect(hWnd, iPositionNew, rcChannel, &rcThumbNew);
    UnionRect(&rcInvalid, rcThumbOld, &rcThumbNew);
    InflateRect(&rcInvalid, 10, 0);

    if (iPositionNew != iPosition)
    {
        MSG msg;
        iPosition = iPositionNew;
        RedrawWindow(hWnd, &rcInvalid, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        if (bUpdateGame)
        {
            HandOffMsg(TBM_SETPOS, 0, iPosition);
            while (PeekMessage(&msg, hWnd, TBM_SETPOS, TBM_SETPOS, PM_REMOVE));
        }
    }
}

INT_PTR WINAPI AboutProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        RECT rcPos, rcParent;
        HWND hWndParent = GetParent(hWnd);
        GetClientRect(hWnd, &rcPos);
        GetWindowRect(hWndParent, &rcParent);
        SetWindowPos(hWnd, NULL, rcParent.left + (rcParent.right - rcParent.left - rcPos.right) / 2,
            rcParent.top + (rcParent.bottom - rcParent.top - rcPos.bottom) / 2,
            0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

        return TRUE;
    }
    case WM_CTLCOLORSTATIC:
    {
        HDC hDC = (HDC)wParam;
        SetBkMode(hDC, TRANSPARENT);
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    }
    case WM_COMMAND:
    {
        int iId = LOWORD(wParam);
        switch (iId)
        {
        case IDOK: case IDCANCEL:
            EndDialog(hWnd, IDOK);
            return TRUE;
        }
        break;
    }
    }

    return FALSE;
}

// Helpers involved with user interaction and the GUI

VOID HandOffMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
    MSG msgGameThread = { g_hWndGfx, msg, wParam, lParam };
    g_MsgQueue.ForcePush(msgGameThread);
}

VOID ShowControls(BOOL bShow)
{
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    SizeWindows(0, 0);
    if (!cView.GetFullScreen() || cControls.bAlwaysShowControls)
        ShowWindow(g_hWndBar, bShow ? SW_SHOWNA : SW_HIDE);
    else if (cView.GetFullScreen())
        ShowWindow(g_hWndBar, SW_HIDE);
    HandOffMsg(WM_COMMAND, ID_VIEW_RESETDEVICE, 0);

    HMENU hMenu = GetMainMenu();
    CheckMenuItem(hMenu, ID_VIEW_CONTROLS, MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
}

VOID ShowKeyboard(BOOL bShow)
{
    HMENU hMenu = GetMainMenu();
    CheckMenuItem(hMenu, ID_VIEW_KEYBOARD, MF_BYCOMMAND | (bShow ? MF_CHECKED : MF_UNCHECKED));
}

VOID SetOnTop(BOOL bOnTop)
{
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    if (!cView.GetFullScreen())
        SetWindowPos(g_hWnd, bOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    HMENU hMenu = GetMainMenu();
    CheckMenuItem(hMenu, ID_VIEW_ALWAYSONTOP, MF_BYCOMMAND | (bOnTop ? MF_CHECKED : MF_UNCHECKED));
}

VOID SetFullScreen(BOOL bFullScreen)
{
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    static RECT rcOld = {};
    HMENU hMenu = GetMainMenu();

    if (bFullScreen)
    {
        RECT rcDesktop;
        GetWindowRect(g_hWnd, &rcOld);
        GetWindowRect(GetDesktopWindow(), &rcDesktop);

        SetMenu(g_hWnd, NULL);
        if (!cControls.bAlwaysShowControls) ShowWindow(g_hWndBar, SW_HIDE);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, GetWindowLongPtr(g_hWnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
        SetWindowPos(g_hWnd, HWND_TOPMOST, rcDesktop.left, rcDesktop.top,
            rcDesktop.right - rcDesktop.left, rcDesktop.bottom - rcDesktop.top,
            SWP_NOACTIVATE | SWP_FRAMECHANGED);
        HandOffMsg(WM_COMMAND, ID_VIEW_RESETDEVICE, 0);
        CheckMenuItem(hMenu, ID_VIEW_FULLSCREEN, MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        SetMenu(g_hWnd, hMenu);
        if (cView.GetControls()) ShowWindow(g_hWndBar, SW_SHOWNA);
        SetWindowLongPtr(g_hWnd, GWL_STYLE, GetWindowLongPtr(g_hWnd, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME);
        SetWindowPos(g_hWnd, cView.GetOnTop() ? HWND_TOPMOST : HWND_NOTOPMOST, rcOld.left, rcOld.top,
            rcOld.right - rcOld.left, rcOld.bottom - rcOld.top,
            SWP_NOACTIVATE | SWP_FRAMECHANGED);
        HandOffMsg(WM_COMMAND, ID_VIEW_RESETDEVICE, 0);
        CheckMenuItem(hMenu, ID_VIEW_FULLSCREEN, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

VOID SetZoomMove(BOOL bZoomMove)
{
    HMENU hMenu = GetMainMenu();
    CheckMenuItem(hMenu, ID_VIEW_MOVEANDZOOM, MF_BYCOMMAND | (bZoomMove ? MF_CHECKED : MF_UNCHECKED));
}

VOID SetMute(BOOL bMute)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    SendMessage(hWndToolbar, TB_CHANGEBITMAP, ID_PLAY_MUTE, bMute ? 6 : 5);
    HMENU hMenu = GetMainMenu();
    CheckMenuItem(hMenu, ID_PLAY_MUTE, MF_BYCOMMAND | (bMute ? MF_CHECKED : MF_UNCHECKED));
}

VOID SetSpeed(DOUBLE dSpeed)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    HWND hWndSpeed = GetDlgItem(hWndToolbar, IDC_SPEED);
    SendMessage(hWndSpeed, TBM_SETPOS, TRUE, (LONG)(100 * dSpeed + .5));
}

VOID SetNSpeed(DOUBLE dNSpeed)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    HWND hWndNSpeed = GetDlgItem(hWndToolbar, IDC_NSPEED);
    SendMessage(hWndNSpeed, TBM_SETPOS, TRUE, (LONG)(100 * (2.0 - dNSpeed) + .5));
}

VOID SetVolume(DOUBLE dVolume)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    HWND hWndVolume = GetDlgItem(hWndToolbar, IDC_VOLUME);
    SendMessage(hWndVolume, TBM_SETPOS, TRUE, (LONG)(100 * dVolume + .5));
}

VOID SetPosition(INT iPosition)
{
    HWND hWndPosn = GetDlgItem(g_hWndBar, IDC_POSNCTRL);
    PostMessage(hWndPosn, TBM_SETPOS, 0, iPosition);
}

VOID SetPlayable(BOOL bPlayable)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_PLAY_PLAY, bPlayable);
}

VOID SetPlayMode(INT ePlayMode)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    HMENU hMenu = GetMainMenu();
    BOOL bPractice = (ePlayMode == GameState::Practice);

    int iPlayButtons[] = { ID_PLAY_PLAY, ID_PLAY_PAUSE, ID_PLAY_STOP };
    for (size_t i = 0; i < sizeof(iPlayButtons) / sizeof(int); i++)
        SendMessage(hWndToolbar, TB_ENABLEBUTTON, iPlayButtons[i], bPractice);
    int iPracticeButtons[] = { ID_PLAY_SKIPFWD, ID_PLAY_SKIPBACK };
    for (size_t i = 0; i < sizeof(iPracticeButtons) / sizeof(int); i++)
        SendMessage(hWndToolbar, TB_ENABLEBUTTON, iPracticeButtons[i], bPractice);

    SendMessage(hWndToolbar, TB_PRESSBUTTON, ID_PLAY_PLAY, TRUE);
    SetZoomMove(FALSE);

    int iMenuItems[][5] = { { 1, ePlayMode, ID_FILE_CLOSEFILE },
                            { 3, bPractice, ID_PLAY_PLAYPAUSE, ID_PLAY_STOP, ID_VIEW_MOVEANDZOOM },
                            { 2, bPractice, ID_PLAY_SKIPFWD, ID_PLAY_SKIPBACK },
                            { 3, true, ID_PLAY_INCREASERATE, ID_PLAY_DECREASERATE, ID_PLAY_RESETRATE } };
    for (size_t i = 0; i < sizeof(iMenuItems) / sizeof(iMenuItems[0]); i++)
    {
        UINT uEnable = (iMenuItems[i][1] ? MF_ENABLED : MF_GRAYED);
        for (int j = 0; j < iMenuItems[i][0]; j++)
            EnableMenuItem(hMenu, iMenuItems[i][j + 2], MF_BYCOMMAND | uEnable);
    }

    HWND hWndPosn = GetDlgItem(g_hWndBar, IDC_POSNCTRL);
    EnableWindow(GetDlgItem(hWndToolbar, IDC_SPEED), true);
    EnableWindow(hWndPosn, bPractice);
}

VOID SetPlayPauseStop(BOOL bPlay, BOOL bPause, BOOL bStop)
{
    HWND hWndToolbar = GetDlgItem(g_hWndBar, IDC_TOPTOOLBAR);
    SendMessage(hWndToolbar, TB_PRESSBUTTON, ID_PLAY_PLAY, bPlay);
    SendMessage(hWndToolbar, TB_PRESSBUTTON, ID_PLAY_PAUSE, bPause);
    SendMessage(hWndToolbar, TB_PRESSBUTTON, ID_PLAY_STOP, bStop);
}

INT_PTR LoadingProc(HWND hwnd, UINT msg, WPARAM, LPARAM) {
    switch (msg) {
    case WM_INITDIALOG: {
        SetWindowTextW(hwnd, (LoadingText L" " + g_LoadingProgress.name).c_str());
        SetTimer(hwnd, 420691337, 1, NULL);
        EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        return true;
    }
    case WM_TIMER: {
        // lots of race conditions possible but hopefully nobody notices them
        wstring desc = LoadWindowTitle;
        switch (g_LoadingProgress.stage) {
        case MIDILoadingProgress::Stage::CopyToMem:
            desc = LoadingStage1;
            break;
        case MIDILoadingProgress::Stage::Decompress:
            desc = LoadingStage2;
            break;
        case MIDILoadingProgress::Stage::ParseTracks:
            desc = LoadingStage3;
            break;
        case MIDILoadingProgress::Stage::ConnectNotes:
            desc = LoadingStage4;
            break;
        case MIDILoadingProgress::Stage::SortEvents:
            desc = LoadingStage5;
            break;
        case MIDILoadingProgress::Stage::Done:
            EndDialog(hwnd, 0);
            return true;
        }

        wchar_t buf[1 << 10];
        auto prog = g_LoadingProgress.progress.load();
        auto bar = GetDlgItem(hwnd, IDC_LOADINGPROGRESS);
        SendMessage(bar, PBM_SETRANGE32, 0, g_LoadingProgress.max);
        SendMessage(bar, PBM_SETPOS, prog, 0);
        UpdateWindow(bar);

        SetWindowText(GetDlgItem(hwnd, IDC_LOADINGDESC), desc.c_str());

        swprintf(buf, sizeof(buf), L"%llu / %llu", prog, g_LoadingProgress.max);
        SetWindowText(GetDlgItem(hwnd, IDC_LOADINGNUM), buf);

        PROCESS_MEMORY_COUNTERS mem{};
        mem.cb = sizeof(mem);
        GetProcessMemoryInfo(GetCurrentProcess(), &mem, sizeof(mem));
        swprintf(buf, sizeof(buf), L"%llu MB", mem.PagefileUsage / 1048576);
        SetWindowText(GetDlgItem(hwnd, IDC_MEMUSAGE), buf);

        return true;
    }
    case WM_CLOSE: {
        EndDialog(hwnd, 0);
        return true;
    }
    }
    return false;
}

BOOL PlayFile(const wstring& sFile)
{
    Config& config = Config::GetConfig();
    PlaybackSettings& cPlayback = config.GetPlaybackSettings();
    ViewSettings& cView = config.GetViewSettings();
    const VideoSettings& cVideo = config.GetVideoSettings();

    const GameState::State ePlayMode = GameState::Practice;

    // Try loading the file
    MainScreen* pGameState = NULL;
    g_LoadingProgress.stage = MIDILoadingProgress::Stage::CopyToMem;
    g_LoadingProgress.name = sFile;
    g_LoadingProgress.progress = 0;
    g_LoadingProgress.max = 1;
    auto thread = std::thread([&]() {
        pGameState = new MainScreen(sFile, ePlayMode, NULL, NULL);
        g_LoadingProgress.stage = MIDILoadingProgress::Done;
        });
    DialogBox(NULL, MAKEINTRESOURCE(IDD_LOADING), g_hWnd, LoadingProc);
    thread.join();
    if (!pGameState->IsValid())
    {
        MessageBox(g_hWnd, (L"Was not able to load " + sFile).c_str(), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    // Set up track settings
    if (!GetCustomSettings(pGameState)) {
        delete pGameState;
        return FALSE;
    }

    // Success! Set up the GUI for playback
    if (!cPlayback.GetPlayable()) cPlayback.SetPlayable(true, true);
    if (cPlayback.GetPlayMode() != ePlayMode) cPlayback.SetPlayMode(ePlayMode, true);
    cPlayback.SetPaused(ePlayMode != GameState::Practice, true);
    cPlayback.SetPosition(0);
    cView.SetZoomMove(false);
    TCHAR sTitle[1 << 10];
    if (cVideo.bDumpFrames) {
        _stprintf_s(sTitle, MainWindowTitle1 L" v" LVersionString L" | " MainWindowTitle2 L" | " MainWindowTitle4 L"%ws" MainWindowTitle7, sFile.c_str() + (sFile.find_last_of(L'\\') + 1));
    }
    else {
        _stprintf_s(sTitle, MainWindowTitle1 L" v" LVersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 L"%ws" MainWindowTitle7, sFile.c_str() + (sFile.find_last_of(L'\\') + 1));
    }
    SetWindowText(g_hWnd, sTitle);


    // Switch game state
    HandOffMsg(WM_COMMAND, ID_CHANGESTATE, (LPARAM)pGameState);
    return TRUE;
}

VOID CheckActivity(BOOL bIsActive, POINT* ptNew, BOOL bToggleEnable)
{
    static const ViewSettings& cView = Config::GetConfig().GetViewSettings();
    static const ControlsSettings& cControls = Config::GetConfig().GetControlsSettings();
    static bool bEnabled = true;
    static bool bWasActive = true;
    static bool bMouseHidden = false;
    static POINT ptOld;

    if (!bEnabled && !bToggleEnable) return;
    if (bToggleEnable) bEnabled = !bEnabled;

    // Has the cursor position changed?
    bool bSamePt;
    if (ptNew)
    {
        bSamePt = (ptNew->x == ptOld.x && ptNew->y == ptOld.y);
        ptOld = *ptNew;
    }
    else
    {
        POINT pt;
        GetCursorPos(&pt);
        bSamePt = (pt.x == ptOld.x && pt.y == ptOld.y);
        ptOld = pt;
    }

    if ((bIsActive && !ptNew) || !bSamePt || !cView.GetFullScreen())
    {
        bWasActive = true;
        if (bMouseHidden) bMouseHidden = (ShowCursor(TRUE) < 0);
    }
    else if (!bIsActive && GetFocus() == g_hWndGfx && (!IsWindowVisible(g_hWndBar) || cControls.bAlwaysShowControls))
    {
        if (bWasActive)
            bWasActive = false;
        else if (!bMouseHidden)
            bMouseHidden = (ShowCursor(FALSE) < 0);
    }
}