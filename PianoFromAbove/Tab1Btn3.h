#include <Tab1.h>
#include <ConsoleTextbox.h>
#include <ConsoleFillingTool.h>

TouchEventListener* Tab1::Btn3::Close = nullptr;
ConsoleTextbox Tab1::Btn3::Path;

void Tab1::Btn3::Open() {
    Tab1::Btn3->OnLeave = []() {};
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    // Move the last two buttons downwards
    HWND ConsoleWindow = GetConsoleWindow();
    HDC ConsoleDC = GetDC(ConsoleWindow);
    HDC MemDC = CreateCompatibleDC(ConsoleDC);
    HBITMAP BMP = CreateCompatibleBitmap(ConsoleDC, 46 * ChW, 6 * ChH);
    SelectObject(MemDC, BMP);
    BitBlt(MemDC, 0, 0, 46 * ChW, 6 * ChH, ConsoleDC, 3 * ChW, 11 * ChH, SRCCOPY);\
    FillConsole(3, 11, 46, 6, ' ', 0x0F);
    uint16_t CP_Y = 11 * ChH;
    while (CP_Y <= 19 * ChH) {
        BitBlt(ConsoleDC, 3 * ChW, CP_Y, 46 * ChW, 6 * ChH, MemDC, 0, 0, SRCCOPY);
        CP_Y += 1 << 2;
        Sleep(1);
    }
    DeleteDC(MemDC);
    DeleteObject(BMP);
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
    Close = TouchEventManager::Create();
    Close->X = 65;
    Close->Y = 10;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        Path.Delete();
        FillConsole(3, 8, 65, 11, ' ', 0x0F);
        cout << "[9;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
        cout << "[10;4H[107m[30m Install Black MIDI templates for FL Studio 12.3 [90m█";
        cout << "[11;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        // Move the last two buttons downwards
        HWND ConsoleWindow = GetConsoleWindow();
        HDC ConsoleDC = GetDC(ConsoleWindow);
        HDC MemDC = CreateCompatibleDC(ConsoleDC);
        HBITMAP BMP = CreateCompatibleBitmap(ConsoleDC, 46 * ChW, 7 * ChH);
        SelectObject(MemDC, BMP);
        BitBlt(MemDC, 0, 0, 46 * ChW, 7 * ChH, ConsoleDC, 3 * ChW, 19 * ChH, SRCCOPY);
        uint16_t CP_Y = 19 * ChH;
        while (CP_Y >= 11 * ChH) {
            BitBlt(ConsoleDC, 3 * ChW, CP_Y, 46 * ChW, 7 * ChH, MemDC, 0, 0, SRCCOPY);
            CP_Y -= 1 << 2;
            Sleep(1);
        }
        DeleteDC(MemDC);
        DeleteObject(BMP);
        TabSwitcher::EnableAll();
        Tab1::EnableAll();
        FillConsole(3, 19, 46, 6, ' ', 0x0F);
    };
    Close->OnHover = []() {
        cout << "[11;66H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[11;66H[44m[91mx";
    };
    Path.Create(6*ChW-ChW/2, 13*ChH-ChH/2, 51*ChW, 2*ChH);
    Path.SetText(L"C:\\Program Files (x86)\\Image-Line\\FL Studio 12");
}