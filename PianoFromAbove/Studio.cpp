#include <Studio.h>
#include <PFXSTUDIO_Global_Imports.h>
#include <EmbededFilesExtractor.h>
#include <ConsoleSetupScript.h>
#include <TabSwitcher.h>
#include <CloseBtn.h>

void StudioMain() {
    ConsoleInit();
    ExtractEmbededFiles();
    CloseBtn::Create();
    TabSwitcher::EnableTab(1);
    TabSwitcher::EnableAll();

    size_t LastCommandLength = 0;
    cout << "[1;2H[40m[91m(0,0)";
    while (!CloseBtn::Terminated) {
        POINT CursorPos = {};
        if (GetCursorPos(&CursorPos) && GetForegroundWindow() == GetConsoleWindow()) {
            ScreenToClient(GetConsoleWindow(), &CursorPos);
            CursorPos.x /= ChW;
            CursorPos.y /= ChH;
            string CommandText = "(" + to_string(CursorPos.x) + ", " + to_string(CursorPos.y) + ")";
            size_t ThisCommandLength = CommandText.length();
            bool Clicked = false;
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                // Button down - green text
                cout << "[1;2H[40m[92m";
                Clicked = true;
            }
            else {
                // Button up - red text
                cout << "[1;2H[40m[91m";
            }
            cout << CommandText;
            while (ThisCommandLength < LastCommandLength) {
                cout << "[96m═";
                ThisCommandLength++;
            }
            LastCommandLength = CommandText.length();
            TouchEventManager::CheckTouches(CursorPos, Clicked);
        }
        LoopingTask(); //Pseudo multithreading
        🖥();
        💬();
    }
}