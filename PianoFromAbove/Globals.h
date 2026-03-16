/*************************************************************************************************
*
* File: Globals.h
*
* Description: Global variables. Mostly window handlers.
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>
#include "Misc.h"
#include "resource.h"

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;
extern HWND g_hWndBar;
extern HWND g_hWndGfx;
extern bool g_bGfxDestroyed;
extern TSQueue< MSG > g_MsgQueue; // Producer/consumer to hold events for our game thread

#define ERRORANDRETURN(hwnd, msg, retval) {MessageBox((hwnd), (msg), TEXT("Error"), MB_OK | MB_ICONERROR); return (retval);}
#define NOMINMAX

#define TitlePlay MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 L"%ws" MainWindowTitle7 MainWindowTitle8
#define TitleRender MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle4 L"%ws" MainWindowTitle7 MainWindowTitle8
#define TitleSplash MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle6 MainWindowTitle7 MainWindowTitle8
#define TitleIdle MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle5 MainWindowTitle7 MainWindowTitle8