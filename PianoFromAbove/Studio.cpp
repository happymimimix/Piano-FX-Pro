#include <Studio.h>
#include <PFXSTUDIO_Global_Imports.h>
#include <EmbededFilesExtractor.h>
#include <ConsoleSetupScript.h>
#include <TabSwitcher.h>
#include <CloseBtn.h>

void StudioMain() {
    Setup();
    CloseBtn::Create();
    TabSwitcher::EnableTab(1);
    TabSwitcher::EnableAll();

    size_t LastCommandLength = 0;
    cout << "[1;2H[40m[31m(0,0)";
    while (!CloseBtn::Terminated) {
        INPUT_RECORD inrec;
        DWORD nEvents;
        if (ReadConsoleInputA(GetStdHandle(STD_INPUT_HANDLE), &inrec, 1, &nEvents)) {
            if (inrec.EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mouseEvent = inrec.Event.MouseEvent;
                // Print mouse debug information
                string CommandText = "(" + to_string(mouseEvent.dwMousePosition.X) + ", " + to_string(mouseEvent.dwMousePosition.Y) + ")";
                size_t ThisCommandLength = CommandText.length();
                bool Clicked = false;
                if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                    // Button down - green text
                    cout << "[1;2H[40m[32m";
                    Clicked = true;
                }
                else {
                    // Button up - red text
                    cout << "[1;2H[40m[31m";
                }
                cout << CommandText;
                while (ThisCommandLength < LastCommandLength) {
                    cout << "[96m═";
                    ThisCommandLength++;
                }
                LastCommandLength = CommandText.length();
                TouchEventManager::CheckTouches(mouseEvent.dwMousePosition, Clicked);
            }
        }
    }
}