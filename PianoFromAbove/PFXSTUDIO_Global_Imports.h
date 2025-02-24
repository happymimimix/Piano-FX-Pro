#pragma once
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <functional>
#include <shlobj.h>

using namespace std;

#define ChW 10
#define ChH 18

function<void()> inline LoopingTask = []() {};

void inline 💬() {
    MSG msg = {};
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void inline 🖥() {
    HWND hComponents = NULL;
    while ((hComponents = FindWindowEx(GetConsoleWindow(), hComponents, NULL, NULL)) != NULL)
    {
        InvalidateRect(hComponents, NULL, true);
    }
}