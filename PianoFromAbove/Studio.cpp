#include <iostream>
#include <Windows.h>
#include <Misc.h>

using namespace std;

void StudioMain() {
    // Enable escape sequence
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    // Clear screen
    cout << "\033[2J\033[H";

}