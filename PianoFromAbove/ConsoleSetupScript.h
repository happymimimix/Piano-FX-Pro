#include <PFXSTUDIO_Global_Imports.h>

void Setup() {
    // Enable ANSI escape code and mouse interaction
    DWORD dwMode;
    if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwMode)) {
        cout << "[1;1H[40m[91mERROR: Cannot obtain console output mode. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwMode)) {
        cout << "[1;1H[40m[91mERROR: Cannot change console output mode. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    if (!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &dwMode)) {
        cout << "[1;1H[40m[91mERROR: Cannot obtain console input mode. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    dwMode &= ~ENABLE_QUICK_EDIT_MODE;
    dwMode |= ENABLE_MOUSE_INPUT;
    dwMode |= ENABLE_AUTO_POSITION;
    if (!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), dwMode)) {
        cout << "[1;1H[40m[91mERROR: Cannot change console input mode. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    // UTF-8
    if (!SetConsoleOutputCP(65001)) {
        cout << "[1;1H[40m[91mERROR: Unable to set console code page. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    // Set console font
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.dwFontSize.X = 10;
    cfi.dwFontSize.Y = 18;
    cfi.FontWeight = 400;
    wcscpy(cfi.FaceName, L"Terminal");
    if (!SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi)) {
        cout << "[1;1H[40m[91mERROR: Unable to set console font. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    // Set console window size
    COORD bufferSize;
    bufferSize.X = 100;
    bufferSize.Y = 35;
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = 99;
    windowSize.Bottom = 34;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
        cout << "[1;1H[40m[91mERROR: Unable to set console buffer size. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    if (!SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize)) {
        cout << "[1;1H[40m[91mERROR: Unable to set console window size. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    // Prevent window resize
    LONG style = GetWindowLong(GetConsoleWindow(), GWL_STYLE);
    style &= ~WS_THICKFRAME;
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(GetConsoleWindow(), GWL_STYLE, style);
    // Clear screen
    cout << "[2J[H[?25l[0m";
}