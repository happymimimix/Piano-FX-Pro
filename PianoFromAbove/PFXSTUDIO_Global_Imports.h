#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <functional>
#include <shlobj.h>

using namespace std;

inline int32_t ChW = 10;
inline int32_t ChH = 18;

function<void()> inline LoopingTask = []() {};

void inline 💬() {
    //Process messages
    MSG msg = {};
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void inline 🖥() {
    //Graphics update
    HWND hComponents = NULL;
    while ((hComponents = FindWindowEx(GetConsoleWindow(), hComponents, NULL, NULL)) != NULL)
    {
        InvalidateRect(hComponents, NULL, true);
    }

    //Handle console font change
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if (!GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi)) {
        cout << "[1;1H[40m[91mERROR: Unable to get console font size. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    ChW = cfi.dwFontSize.X;
    ChH = cfi.dwFontSize.Y;
}