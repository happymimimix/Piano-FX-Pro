/*************************************************************************************************
*
* File: ConfigProcs.h
*
* Description: Defines configuration GUI functions. Not C++ :/
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>

#include "GameState.h"
#include "Config.h"
#include "MIDI.h"

// Message handlers for the configuration property sheets
VOID DoPreferences( HWND hWndOwner );
VOID Changed( HWND hWnd );

INT_PTR WINAPI VisualProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
INT_PTR WINAPI AudioProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
INT_PTR WINAPI VideoProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
INT_PTR WINAPI ControlsProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

BOOL GetCustomSettings( MainScreen *pGameState );
INT_PTR WINAPI TracksProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
