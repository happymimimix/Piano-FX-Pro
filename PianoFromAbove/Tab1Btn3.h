#include <Tab1.h>

void Tab1::OpenSubView_Btn3() {
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    Btn3->OnLeave = []() {};
    // Move the last two buttons downwards
    HWND ConsoleWindow = GetConsoleWindow();
    HDC ConsoleDC = GetDC(ConsoleWindow);
    uint16_t CP_Y = 11 * 18;
    while (CP_Y < 19 * 18) {
        BitBlt(ConsoleDC, 3 * 10, CP_Y + (1 << 2), 46 * 10, 6 * 18, ConsoleDC, 3 * 10, CP_Y, SRCCOPY);
        CP_Y += 1 << 2;
        Sleep(1);
    }
    // Draw the UI
    cout << "[9;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
    cout << "[10;4H[44m[97m Install Black MIDI templates for FL Studio 12.3 [30m█";
    cout << "[11;4H[44m[36m┌────────────────────────────────────────────────────────────[[91mx[36m]┐";
    cout << "[12;4H[44m[36m│ [95mFl Studio installation directory:                             [36m│";
    cout << "[13;4H[44m[36m│ [95m┌──────────────────────────────────────────────────┬────────┐ [36m│";
    cout << "[14;4H[44m[36m│ [95m│                                                  │ [91mBrowse [95m│ [36m│";
    cout << "[15;4H[44m[36m│ [95m└──────────────────────────────────────────────────┴────────┘ [36m│";
    cout << "[16;4H[44m[36m│                                                   [91m┌─────────┐ [36m│";
    cout << "[17;4H[44m[36m│                                                   [91m│ [92mInstall [91m│ [36m│";
    cout << "[18;4H[44m[36m│                                                   [91m└─────────┘ [36m│";
    cout << "[19;4H[44m[36m└───────────────────────────────────────────────────────────────┘";
    // Don't let the last two buttons disappear on minimizing
    cout << "[20;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
    cout << "[21;4H[107m[30m Download Cheat Engine from official website [90m█";
    cout << "[22;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
    cout << "[23;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
    cout << "[24;4H[107m[30m Improve audio quality with OmniMIDI [90m█";
    cout << "[25;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
    // TODO: Add text boxes and buttons. 
}