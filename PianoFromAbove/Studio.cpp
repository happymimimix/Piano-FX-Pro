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
    TouchEventListener* Add() {
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
        if (ListenerPointer->Prev == nullptr) {
            // Trying to delete the first item
            this->First = ListenerPointer->Next;
            if (ListenerPointer->Next != nullptr) {
                ListenerPointer->Next->Prev = nullptr;
            }
        }
        else if (ListenerPointer->Next == nullptr) {
            // Trying to delete the last item
            ListenerPointer->Prev->Next = nullptr;
        }
        else {
            // Trying to delete an item in the middle
            ListenerPointer->Prev->Next = ListenerPointer->Next;
            ListenerPointer->Next->Prev = ListenerPointer->Prev;
        }
        delete ListenerPointer;
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
    switch (Selected) {
    case 1:
        cout << "[33;2H[40m[96m╕       ╒╤═════════╤╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [92mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        break;
    case 2:
        cout << "[33;2H[40m[96m╤═══════╤╕         ╒╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [92mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        break;
    case 3:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╕          ╒╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [92mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        break;
    case 4:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╤══════════╤╕     ╒";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [92mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
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

void StudioMain() {
    Setup();
    TouchEventManager TEmanager;

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

    DrawBG();
    cout << "[1;2H[31m(0,0)";

    TouchEventListener* CloseBtn = TEmanager.Add();
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

    TouchEventListener* Tab1 = TEmanager.Add();
    Tab1->X = 2;
    Tab1->Y = 33;
    Tab1->W = 6;
    Tab1->H = 0;
    Tab1->OnTouch = []() {
        if (CurrentTab != 1) {
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
    TouchEventListener* Tab2 = TEmanager.Add();
    Tab2->X = 11;
    Tab2->Y = 33;
    Tab2->W = 8;
    Tab2->H = 0;
    Tab2->OnTouch = []() {
        if (CurrentTab != 2) {
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
    TouchEventListener* Tab3 = TEmanager.Add();
    Tab3->X = 22;
    Tab3->Y = 33;
    Tab3->W = 9;
    Tab3->H = 0;
    Tab3->OnTouch = []() {
        if (CurrentTab != 3) {
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
    TouchEventListener* Tab4 = TEmanager.Add();
    Tab4->X = 34;
    Tab4->Y = 33;
    Tab4->W = 4;
    Tab4->H = 0;
    Tab4->OnTouch = []() {
        if (CurrentTab != 4) {
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
    
    size_t LastCommandLength = 0;
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
                    cout << "[1;2H[32m";
                    Clicked = true;
                }
                else {
                    // Button up - red text
                    cout << "[1;2H[31m";
                }
                cout << CommandText;
                while (ThisCommandLength < LastCommandLength) {
                    cout << "[96m═";
                    ThisCommandLength++;
                }
                LastCommandLength = CommandText.length();
                TEmanager.CheckTouches(mouseEvent.dwMousePosition, Clicked);
            }
        }
    }
}