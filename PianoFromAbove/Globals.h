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

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;
extern HWND g_hWndBar;
extern HWND g_hWndGfx;
extern bool g_bGfxDestroyed;
extern TSQueue< MSG > g_MsgQueue; // Producer/consumer to hold events for our game thread

#define ERRORANDRETURN(hwnd, msg, retval) {MessageBox((hwnd), (msg), TEXT("Error"), MB_OK | MB_ICONERROR); return (retval);}
#define NOMINMAX

typedef signed long tick_t;
typedef signed long long mms_t;
typedef unsigned char key_t;
typedef uint16_t track_t;
typedef uint8_t chan_t;
typedef uint32_t TnC_t;
typedef uint8_t msg_t;
typedef uint32_t msgln_t;
#ifdef LONG_INTEGER
static_assert(sizeof(void*) == 8, "Extended addressing is not supported in 32bit! ");
typedef size_t idx_t;
typedef intptr_t sidx_t;
#else
typedef uint32_t idx_t;
typedef int32_t sidx_t;
#endif
typedef uint32_t color_t;
typedef uint32_t bpm_t;
typedef INT win32_t;
