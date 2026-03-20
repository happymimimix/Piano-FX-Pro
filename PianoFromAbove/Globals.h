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

inline bool IsWrapRenderer = false;
inline bool RendererActive = false;
#define TitleVersionInfo MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | "
#ifdef SOFTWARE_RENDER_ONLY
#define PF_SWR (IsWrapRenderer ? L"  " MainWindowTitle7 : L"") 
#else
#define PF_SWR L"  " MainWindowTitle8
#endif
#ifdef LIMIT_COLORS
#define PF_COL L"  " MainWindowTitle9
#else
#define PF_COL
#endif
#define TitlePostFix PF_SWR PF_COL
#define TitlePlay TitleVersionInfo MainWindowTitle3 L"%ws" TitlePostFix
#define TitleRender TitleVersionInfo MainWindowTitle4 L"%ws" TitlePostFix
#define TitleSplash TitleVersionInfo MainWindowTitle3 MainWindowTitle6 TitlePostFix
#define TitleIdle TitleVersionInfo MainWindowTitle3 MainWindowTitle5 TitlePostFix