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
#include <Misc.h>
using namespace std;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;
extern HWND g_hWndBar;
extern HWND g_hWndGfx;
extern bool g_bGfxDestroyed;
extern TSQueue<MSG> g_MsgQueue;

inline bool* PtrToIsWrapRenderer = nullptr;
#define TitleGapLine wstring(L" | ")
#define TitleGapEmpty wstring(L"  ")
#define EmptyWstr wstring(L"")
#define TitleVersionInfo MainWindowTitle1+L" v"+VersionString+TitleGapLine+MainWindowTitle2+TitleGapLine
#ifdef SOFTWARE_RENDER_ONLY
#define PF_SWR TitleGapEmpty+MainWindowTitle8
#else
#define PF_SWR wstring(PtrToIsWrapRenderer != nullptr && (*PtrToIsWrapRenderer) ? wstring(TitleGapEmpty+MainWindowTitle7) : EmptyWstr)
#endif
#ifdef LIMIT_COLORS
#define PF_COL TitleGapEmpty+MainWindowTitle9
#else
#define PF_COL EmptyWstr
#endif
#define TitlePostFix PF_SWR+PF_COL
#define TitlePlay wstring(EmptyWstr+TitleVersionInfo+MainWindowTitle3+L"%ws"+TitlePostFix).c_str()
#define TitleRender wstring(EmptyWstr+TitleVersionInfo+MainWindowTitle4+L"%ws"+TitlePostFix).c_str()
#define TitleSplash wstring(EmptyWstr+TitleVersionInfo+MainWindowTitle3+MainWindowTitle6+TitlePostFix).c_str()
#define TitleIdle wstring(EmptyWstr+TitleVersionInfo+MainWindowTitle3+MainWindowTitle5+TitlePostFix).c_str()