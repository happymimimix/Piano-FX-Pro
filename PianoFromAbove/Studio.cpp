#include <iostream>
#include <fstream>
#include <Windows.h>
#include <Studio.h>
#include <Colorizer.exe.h>
#include <PFXGDI.exe.h>
#include <filesystem>
#include <string>
#include <vector>
#include <functional>

using namespace std;
bool Terminated = false;
uint8_t CurrentTab = 1;
uint8_t SubTab = 0;

string ProgramDIR() {
    char szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

struct TouchEventListener {
    TouchEventListener* Prev = nullptr;
    TouchEventListener* Next = nullptr;
    uint8_t X = 0;
    uint8_t Y = 0;
    uint8_t W = 0;
    uint8_t H = 0;
    function<void()> OnHover = []() {};
    function<void()> OnLeave = []() {};
    function<void()> OnTouch = []() {};

    bool IsTouching(COORD MousePos) {
        if (this->X <= MousePos.X && MousePos.X <= (this->X + this->W) && this->Y <= MousePos.Y && MousePos.Y <= (this->Y + this->H)) {
            return true;
        }
        else {
            return false;
        }
    }
};

struct TouchEventManager {
    // A chain list that keep track of all instances of TouchEventListeners
    TouchEventListener* First = nullptr;
    TouchEventListener* LastUsed = nullptr;
    TouchEventListener* Create() {
        TouchEventListener* NewListener = new TouchEventListener();
        if (this->First == nullptr) {
            // This is the first item
            this->First = NewListener;
        }
        else {
            NewListener->Next = this->First;
            this->First->Prev = NewListener;
            this->First = NewListener;
        }
        return NewListener;
    }
    void Delete(TouchEventListener* ListenerPointer) {
        if (ListenerPointer == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to remove a touch event listener that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            if (ListenerPointer->Prev == nullptr) {
                // Trying to delete the first item
                this->First = ListenerPointer->Next;
                if (ListenerPointer->Next != nullptr) {
                    ListenerPointer->Next->Prev = nullptr;
                }
            }
            else if (ListenerPointer->Next == nullptr) {
                // Trying to delete the last item
                if (ListenerPointer->Prev == nullptr) {
                    cout << "[1;1H[40m[91mERROR: The touch event listener chain list is broken. \n";
                    cout << "\nThe program will now stop. \n";
                    while (true) {
                        // Make the program hang instead of closing! 
                        // This way the user can clearly see the error message. 
                    }
                }
                else {
                    ListenerPointer->Prev->Next = nullptr;
                }
            }
            else {
                // Trying to delete an item in the middle
                if (ListenerPointer->Next == nullptr || ListenerPointer->Prev == nullptr) {
                    cout << "[1;1H[40m[91mERROR: The touch event listener chain list is broken. \n";
                    cout << "\nThe program will now stop. \n";
                    while (true) {
                        // Make the program hang instead of closing! 
                        // This way the user can clearly see the error message. 
                    }
                }
                else {
                    ListenerPointer->Prev->Next = ListenerPointer->Next;
                    ListenerPointer->Next->Prev = ListenerPointer->Prev;
                }
            }
        }
        ListenerPointer = nullptr;
    }
    void CheckTouches(COORD MousePos, bool MouseClicked) {
        TouchEventListener* CurrentListener = this->First;
        TouchEventListener* Touched = nullptr;
        while (CurrentListener != nullptr) {
            if (CurrentListener->IsTouching(MousePos)) {
                if (Touched == nullptr) {
                    Touched = CurrentListener;
                }
                else {
                    // Oh no! We're triggering more than one TouchEventListeners at the same time! 
                    cout << "[1;1H[40m[91mERROR: Touch event listeners cannot overlap! \n\n";
                    cout << "The TouchEventListener at (";
                    cout << (int)Touched->X;
                    cout << ", ";
                    cout << (int)Touched->Y;
                    cout << ") with size ";
                    cout << (int)Touched->W;
                    cout << " * ";
                    cout << (int)Touched->H;
                    cout << " is overlapping with another TouchEventListener at (";
                    cout << (int)CurrentListener->X;
                    cout << ", ";
                    cout << (int)CurrentListener->Y;
                    cout << ") with size ";
                    cout << (int)CurrentListener->W;
                    cout << " * ";
                    cout << (int)CurrentListener->H;
                    cout << "! \n";
                    cout << "\nThe program will now stop. \n";
                    while (true) {
                        // Make the program hang instead of closing! 
                        // This way the user can clearly see the error message. 
                    }
                }
            }
            CurrentListener = CurrentListener->Next;
        }
        if (Touched != nullptr) {
            // The mouse has touched something! 
            if (MouseClicked && LastUsed == Touched) {
                Touched->OnTouch();
            }
            else if (LastUsed != Touched) {
                Touched->OnHover();
            }
        }
        if (LastUsed != nullptr && LastUsed != Touched) {
            LastUsed->OnLeave();
        }
        LastUsed = Touched;
        CurrentListener = nullptr;
        Touched = nullptr;
    }
};

TouchEventManager temgr;

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
}

void DrawBG() {
    cout << "[1;1H[40m[96m╔══════════════════════════════════════════════════════════════════════════════════════════════╤═══╗\n";
    cout << "[40m[96m║                                                                                              │ [93mx[96m ║\n";
    cout << "[40m[96m║                                                                                              └───╢\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m║                                                                                                  ║\n";
    cout << "[40m[96m╚══════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    cout << "[40m[96m                                                                                                    \n";
    cout << "[40m[96m                                                                                                    ";
}

void DrawTabs(uint8_t Selected) {
    DrawBG();
    switch (Selected) {
    case 1:
        cout << "[33;2H[40m[96m╕       ╒╤═════════╤╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [92mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab1TE::EnableAll();
        break;
    case 2:
        cout << "[33;2H[40m[96m╤═══════╤╕         ╒╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [92mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab2TE::EnableAll();
        break;
    case 3:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╕          ╒╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [92mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab3TE::EnableAll();
        break;
    case 4:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╤══════════╤╕     ╒";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [92mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab4TE::EnableAll();
        break;
    default:
        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
        cout << (int)Selected;
        cout << "\n\nAllowed tab IDs: \n";
        cout << "1 (Setup)\n";
        cout << "2 (Animate)\n";
        cout << "3 (Colorize)\n";
        cout << "4 (GDI)\n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
}

struct TabsTE {
    static TouchEventListener* Tab1;
    static TouchEventListener* Tab2;
    static TouchEventListener* Tab3;
    static TouchEventListener* Tab4;

    static void EnableAll() {
        if (Tab1 != nullptr || Tab2 != nullptr || Tab3 != nullptr || Tab4 != nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Tab1 = temgr.Create();
            Tab1->X = 2;
            Tab1->Y = 33;
            Tab1->W = 6;
            Tab1->H = 0;
            Tab1->OnTouch = []() {
                if (CurrentTab != 1) {
                    switch (CurrentTab)
                    {
                    case 2:
                        Tab2TE::DisableAll();
                    case 3:
                        Tab3TE::DisableAll();
                    case 4:
                        Tab4TE::DisableAll();
                    default:
                        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
                        cout << (int)CurrentTab;
                        cout << "\n\nAllowed tab IDs: \n";
                        cout << "1 (Setup)\n";
                        cout << "2 (Animate)\n";
                        cout << "3 (Colorize)\n";
                        cout << "4 (GDI)\n";
                        cout << "\nThe program will now stop. \n";
                        while (true) {
                            // Make the program hang instead of closing! 
                            // This way the user can clearly see the error message. 
                        }
                    }
                    CurrentTab = 1;
                    DrawTabs(CurrentTab);
                }
            };
            Tab1->OnHover = []() {
                if (CurrentTab != 1) {
                    cout << "[34;4H[93mSetup";
                }
            };
            Tab1->OnLeave = []() {
                if (CurrentTab != 1) {
                    cout << "[34;4H[91mSetup";
                }
            };
            Tab2 = temgr.Create();
            Tab2->X = 11;
            Tab2->Y = 33;
            Tab2->W = 8;
            Tab2->H = 0;
            Tab2->OnTouch = []() {
                if (CurrentTab != 2) {
                    switch (CurrentTab)
                    {
                    case 1:
                        Tab1TE::DisableAll();
                    case 3:
                        Tab3TE::DisableAll();
                    case 4:
                        Tab4TE::DisableAll();
                    default:
                        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
                        cout << (int)CurrentTab;
                        cout << "\n\nAllowed tab IDs: \n";
                        cout << "1 (Setup)\n";
                        cout << "2 (Animate)\n";
                        cout << "3 (Colorize)\n";
                        cout << "4 (GDI)\n";
                        cout << "\nThe program will now stop. \n";
                        while (true) {
                            // Make the program hang instead of closing! 
                            // This way the user can clearly see the error message. 
                        }
                    }
                    CurrentTab = 2;
                    DrawTabs(CurrentTab);
                }
            };
            Tab2->OnHover = []() {
                if (CurrentTab != 2) {
                    cout << "[34;13H[93mAnimate";
                }
            };
            Tab2->OnLeave = []() {
                if (CurrentTab != 2) {
                    cout << "[34;13H[91mAnimate";
                }
            };
            Tab3 = temgr.Create();
            Tab3->X = 22;
            Tab3->Y = 33;
            Tab3->W = 9;
            Tab3->H = 0;
            Tab3->OnTouch = []() {
                if (CurrentTab != 3) {
                    switch (CurrentTab)
                    {
                    case 1:
                        Tab1TE::DisableAll();
                    case 2:
                        Tab2TE::DisableAll();
                    case 4:
                        Tab4TE::DisableAll();
                    default:
                        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
                        cout << (int)CurrentTab;
                        cout << "\n\nAllowed tab IDs: \n";
                        cout << "1 (Setup)\n";
                        cout << "2 (Animate)\n";
                        cout << "3 (Colorize)\n";
                        cout << "4 (GDI)\n";
                        cout << "\nThe program will now stop. \n";
                        while (true) {
                            // Make the program hang instead of closing! 
                            // This way the user can clearly see the error message. 
                        }
                    }
                    CurrentTab = 3;
                    DrawTabs(CurrentTab);
                }
            };
            Tab3->OnHover = []() {
                if (CurrentTab != 3) {
                    cout << "[34;24H[93mColorize";
                }
            };
            Tab3->OnLeave = []() {
                if (CurrentTab != 3) {
                    cout << "[34;24H[91mColorize";
                }
            };
            Tab4 = temgr.Create();
            Tab4->X = 34;
            Tab4->Y = 33;
            Tab4->W = 4;
            Tab4->H = 0;
            Tab4->OnTouch = []() {
                if (CurrentTab != 4) {
                    switch (CurrentTab)
                    {
                    case 1:
                        Tab1TE::DisableAll();
                    case 2:
                        Tab2TE::DisableAll();
                    case 3:
                        Tab3TE::DisableAll();
                    default:
                        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
                        cout << (int)CurrentTab;
                        cout << "\n\nAllowed tab IDs: \n";
                        cout << "1 (Setup)\n";
                        cout << "2 (Animate)\n";
                        cout << "3 (Colorize)\n";
                        cout << "4 (GDI)\n";
                        cout << "\nThe program will now stop. \n";
                        while (true) {
                            // Make the program hang instead of closing! 
                            // This way the user can clearly see the error message. 
                        }
                    }
                    CurrentTab = 4;
                    DrawTabs(CurrentTab);
                }
            };
            Tab4->OnHover = []() {
                if (CurrentTab != 4) {
                    cout << "[34;36H[93mGDI";
                }
            };
            Tab4->OnLeave = []() {
                if (CurrentTab != 4) {
                    cout << "[34;36H[91mGDI";
                }
            };
        }
    }

    static void DisableAll() {
        temgr.Delete(Tab1);
        temgr.Delete(Tab2);
        temgr.Delete(Tab3);
        temgr.Delete(Tab4);
    }
};
TouchEventListener* TabsTE::Tab1 = nullptr;
TouchEventListener* TabsTE::Tab2 = nullptr;
TouchEventListener* TabsTE::Tab3 = nullptr;
TouchEventListener* TabsTE::Tab4 = nullptr;

struct Tab1TE {
    static TouchEventListener* Btn1;
    static TouchEventListener* Btn2;
    static TouchEventListener* Btn3;
    static TouchEventListener* Btn4;
    static TouchEventListener* Btn5;

    static void EnableAll() {
        if (Btn1 != nullptr | Btn2 != nullptr | Btn3 != nullptr | Btn4 != nullptr | Btn5 != nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1 = temgr.Create();
            Btn1->X = 3;
            Btn1->Y = 2;
            Btn1->W = 35;
            Btn1->H = 2;
            Btn1->OnTouch = []() {
                cout << "[3;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[4;4H[44m[97m Download English translated Domino [30m█";
                cout << "[5;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
                Sleep(500);
                ShellExecuteA(0, "open", "http://github.com/happymimimix/Domino-Customized/releases/download/000/DominoCustomized.MSI", 0, 0, SW_SHOWNORMAL);
                Btn1->OnLeave();
            };
            Btn1->OnHover = []() {
                cout << "[3;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[4;4H[103m[30m Download English translated Domino [90m█";
                cout << "[5;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn1->OnLeave = []() {
                cout << "[3;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[4;4H[107m[30m Download English translated Domino [90m█";
                cout << "[5;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn2 = temgr.Create();
            Btn2->X = 3;
            Btn2->Y = 5;
            Btn2->W = 33;
            Btn2->H = 2;
            Btn2->OnTouch = []() {
                cout << "[6;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[7;4H[44m[97m Download Fl Studio v12.3 archive [30m█";
                cout << "[8;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
                Sleep(500);
                ShellExecuteA(0, "open", "http://github.com/happymimimix/Fl-Studio-12.3-Archive/releases/tag/12.3", 0, 0, SW_SHOWNORMAL);
                Btn2->OnLeave();
            };
            Btn2->OnHover = []() {
                cout << "[6;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[7;4H[103m[30m Download Fl Studio v12.3 archive [90m█";
                cout << "[8;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn2->OnLeave = []() {
                cout << "[6;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[7;4H[107m[30m Download Fl Studio v12.3 archive [90m█";
                cout << "[8;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn3 = temgr.Create();
            Btn3->X = 3;
            Btn3->Y = 8;
            Btn3->W = 48;
            Btn3->H = 2;
            Btn3->OnTouch = []() {
                cout << "[9;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[10;4H[44m[97m Install Black MIDI templates for FL Studio 12.3 [30m█";
                cout << "[11;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
                DisableAll();
                TabsTE::DisableAll();
                Btn3->OnLeave = []() {};
                // Move the last two buttons downwards
                HWND ConsoleWindow = GetConsoleWindow();
                HDC ConsoleDC = GetDC(ConsoleWindow);
                uint16_t CP_Y = 11 * 18;
                while (CP_Y < 19 * 18) {
                    BitBlt(ConsoleDC, 3 * 10, CP_Y+(1<<2), 46 * 10, 6 * 18, ConsoleDC, 3 * 10, CP_Y, SRCCOPY);
                    CP_Y += 1<<2;
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
            };
            Btn3->OnHover = []() {
                cout << "[9;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[10;4H[103m[30m Install Black MIDI templates for FL Studio 12.3 [90m█";
                cout << "[11;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn3->OnLeave = []() {
                cout << "[9;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[10;4H[107m[30m Install Black MIDI templates for FL Studio 12.3 [90m█";
                cout << "[11;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn4 = temgr.Create();
            Btn4->X = 3;
            Btn4->Y = 11;
            Btn4->W = 44;
            Btn4->H = 2;
            Btn4->OnTouch = []() {
                cout << "[12;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[13;4H[44m[97m Download Cheat Engine from official website [30m█";
                cout << "[14;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
                Sleep(500);
                ShellExecuteA(0, "open", "http://www.cheatengine.org", 0, 0, SW_SHOWNORMAL);
                Btn4->OnLeave();
            };
            Btn4->OnHover = []() {
                cout << "[12;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[13;4H[103m[30m Download Cheat Engine from official website [90m█";
                cout << "[14;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn4->OnLeave = []() {
                cout << "[12;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[13;4H[107m[30m Download Cheat Engine from official website [90m█";
                cout << "[14;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn5 = temgr.Create();
            Btn5->X = 3;
            Btn5->Y = 14;
            Btn5->W = 36;
            Btn5->H = 2;
            Btn5->OnTouch = []() {
                cout << "[15;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[16;4H[44m[97m Improve audio quality with OmniMIDI [30m█";
                cout << "[17;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
                Sleep(500);
                ShellExecuteA(0, "open", "http://www.cheatengine.org", 0, 0, SW_SHOWNORMAL);
                Btn5->OnLeave();
            };
            Btn5->OnHover = []() {
                cout << "[15;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[16;4H[103m[30m Improve audio quality with OmniMIDI [90m█";
                cout << "[17;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Btn5->OnLeave = []() {
                cout << "[15;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
                cout << "[16;4H[107m[30m Improve audio quality with OmniMIDI [90m█";
                cout << "[17;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            };
            Draw();
        }
    }

    static void DisableAll() {
        temgr.Delete(Btn1);
        temgr.Delete(Btn2);
        temgr.Delete(Btn3);
        temgr.Delete(Btn4);
        temgr.Delete(Btn5);
    }

    static void Draw() {
        if (Btn1 == nullptr | Btn2 == nullptr | Btn3 == nullptr | Btn4 == nullptr | Btn5 == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1->OnLeave();
            Btn2->OnLeave();
            Btn3->OnLeave();
            Btn4->OnLeave();
            Btn5->OnLeave();
        }
    }
};
TouchEventListener* Tab1TE::Btn1 = nullptr;
TouchEventListener* Tab1TE::Btn2 = nullptr;
TouchEventListener* Tab1TE::Btn3 = nullptr;
TouchEventListener* Tab1TE::Btn4 = nullptr;
TouchEventListener* Tab1TE::Btn5 = nullptr;

struct Tab2TE {
    static TouchEventListener* Btn1;

    static void EnableAll() {
        if (Btn1 != nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1 = temgr.Create();
            Draw();
        }
    }

    static void DisableAll() {
        temgr.Delete(Btn1);
    }

    static void Draw() {
        if (Btn1 == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1->OnLeave();
        }
    }
};
TouchEventListener* Tab2TE::Btn1 = nullptr;

struct Tab3TE {
    static TouchEventListener* Btn1;

    static void EnableAll() {
        if (Btn1 != nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1 = temgr.Create();
            Draw();
        }
    }

    static void DisableAll() {
        temgr.Delete(Btn1);
    }

    static void Draw() {
        if (Btn1 == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1->OnLeave();
        }
    }
};
TouchEventListener* Tab3TE::Btn1 = nullptr;

struct Tab4TE {
    static TouchEventListener* Btn1;

    static void EnableAll() {
        if (Btn1 != nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1 = temgr.Create();
            Draw();
        }
    }

    static void DisableAll() {
        temgr.Delete(Btn1);
    }

    static void Draw() {
        if (Btn1 == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            Btn1->OnLeave();
        }
    }
};
TouchEventListener* Tab4TE::Btn1 = nullptr;

void StudioMain() {
    Setup();

    // Clear screen
    cout << "[2J[H[?25l[0m";

    string ColorizerPath = ProgramDIR() + "\\Colorizer.exe";
    if (!filesystem::exists(ColorizerPath)) {
        ofstream ColorizerFile(ColorizerPath, ios::binary);
        ColorizerFile.write(reinterpret_cast<const char*>(Colorizer_exe), Colorizer_exe_len);
        ColorizerFile.close();
    }
    string PFXGDIPath = ProgramDIR() + "\\PFXGDI.exe";
    if (!filesystem::exists(PFXGDIPath)) {
        ofstream PFXGDIFile(PFXGDIPath, ios::binary);
        PFXGDIFile.write(reinterpret_cast<const char*>(PFXGDI_exe), PFXGDI_exe_len);
        PFXGDIFile.close();
    }

    TouchEventListener* CloseBtn = temgr.Create();
    CloseBtn->X = 96;
    CloseBtn->Y = 1;
    CloseBtn->W = 2;
    CloseBtn->H = 0;
    CloseBtn->OnTouch = []() {
        // End the program!
        Terminated = true;
    };
    CloseBtn->OnHover = []() {
        cout << "[2;98H[91mx";
    };
    CloseBtn->OnLeave = []() {
        cout << "[2;98H[93mx";
    };

    DrawTabs(CurrentTab);

    TabsTE::EnableAll();
    Tab1TE::EnableAll();

    size_t LastCommandLength = 0;
    cout << "[1;2H[40m[31m(0,0)";
    while (!Terminated) {
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
                temgr.CheckTouches(mouseEvent.dwMousePosition, Clicked);
            }
        }
    }
}