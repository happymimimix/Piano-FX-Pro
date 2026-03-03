/*************************************************************************************************
*
* File: MainProcs.h
*
* Description: Defines the main GUI functions. Not C++ :/
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include "Misc.h"
using namespace std;

// Message handlers for the main windows
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HMENU GetMainMenu();
VOID SizeWindows(winword_t iMainWidth, winword_t iMainHeight);

LRESULT WINAPI GfxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
VOID CopyMenuState(HMENU hMenuSrc, HMENU hMenuDest);

LRESULT WINAPI BarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND CreateRebar(HWND hWndOwner);
VOID DrawSliderChannel(LPNMCUSTOMDRAW lpnmcd, HWND hWndOwner);

LRESULT WINAPI PosnProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
VOID GetChannelRect(HWND hWnd, RECT* rcChannel);
VOID GetThumbRect(HWND hWnd, winword_t iPosition, const RECT* rcChannel, RECT* rcThumb);
winword_t GetThumbPosition(winword_t iXPos, RECT* rcChannel);
VOID MoveThumbPosition(winword_t iPositionNew, winword_t& iPosition, HWND hWnd, RECT* rcChannel, RECT* rcThumbOld, BOOL bUpdateGame = TRUE);

INT_PTR WINAPI AboutProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

VOID HandOffMsg(UINT msg, WPARAM wParam, LPARAM lParam);
VOID ShowControls(BOOL bShow);
VOID ShowKeyboard(BOOL bShow);
VOID SetOnTop(BOOL bOnTop);
VOID SetFullScreen(BOOL bFullScreen);
VOID SetZoomMove(BOOL bZoomMove);
VOID SetMute(BOOL bMute);
VOID SetSpeed(DOUBLE dSpeed);
VOID SetNSpeed(DOUBLE dSpeed);
VOID SetVolume(DOUBLE dVolume);
VOID SetPosition(INT iPosition);
VOID SetPlayable(BOOL bPlayable);
VOID SetPlayMode(INT ePlayMode);
VOID SetPlayPauseStop(BOOL bPlay, BOOL bPause, BOOL bStop);
BOOL PlayFile(const wstring& sFile);
VOID CheckActivity(BOOL bIsActive, POINT* ptNew = NULL, BOOL bToggleEnable = false);