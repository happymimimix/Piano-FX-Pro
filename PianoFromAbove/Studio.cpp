/*
The entry point for Piano-FX Studio
*/

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
    wcout << L"[1;2H[40m[91m(0,0)";
    while (!CloseBtn::Terminated) {
        POINT CursorPos = {};
        //Get mouse cursor position and button states
        if (GetCursorPos(&CursorPos) && GetForegroundWindow() == GetConsoleWindow()) {
            ScreenToClient(GetConsoleWindow(), &CursorPos);
            CursorPos.x /= ChW;
            CursorPos.y /= ChH;
            wstring CommandText = L"(" + to_wstring(CursorPos.x) + L", " + to_wstring(CursorPos.y) + L")";
            size_t ThisCommandLength = CommandText.length();
            bool Clicked = false;
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                // Button down - green text
                wcout << L"[1;2H[40m[92m";
                Clicked = true;
            }
            else {
                // Button up - red text
                wcout << L"[1;2H[40m[91m";
            }
            wcout << CommandText;
            while (ThisCommandLength < LastCommandLength) {
                wcout << L"[96m═";
                ThisCommandLength++;
            }
            LastCommandLength = CommandText.length();
            //Process touch event listeners
            TouchEventManager::CheckTouches(CursorPos, Clicked);
        }
        LoopingTask(); //Pseudo multithreading
        🖥(); //Graphics update
        💬(); //Process messages
    }
}