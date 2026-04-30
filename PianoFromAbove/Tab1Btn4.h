#include <Tab1.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>
#include <flpTools.h>

TouchEventListener* Tab1::SubViewBtn4::Close = nullptr;
ConsoleTextbox Tab1::SubViewBtn4::Path = {};
TouchEventListener* Tab1::SubViewBtn4::Browse = nullptr;
TouchEventListener* Tab1::SubViewBtn4::Convert = nullptr;

void Tab1::SubViewBtn4::Open() {
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    DoTransition(3, 14, 50, 3, 3, 22, 0x00001000);
    FillConsole(3, 14, 50, 3, ' ', 0x0F);
    Tab1Graphics::Btn5(3, 22, Normal);
    // Draw the UI
    Tab1Graphics::Btn4Graphics::Main(3, 13);
    Close = TouchEventManager::Create();
    Close->X = 65;
    Close->Y = 13;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        Path.Delete();
        TouchEventManager::Delete(Browse);
        TouchEventManager::Delete(Convert);
        FillConsole(3, 13, 65, 9, ' ', 0x0F);
        Tab1Graphics::Btn4(3, 11, Normal);
        DoTransition(3, 22, 50, 4, 3, 14, 0x00001000);
        TabSwitcher::EnableAll();
        Tab1::EnableAll();
        FillConsole(3, 22, 50, 3, ' ', 0x0F);
    };
    Close->OnHover = []() {
        cout << "[14;66H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[14;66H[44m[91mx";
    };
    Path.Create(6, 16, 51, 2);
    Browse = TouchEventManager::Create();
    Browse->X = 57;
    Browse->Y = 16;
    Browse->W = 7;
    Browse->H = 0;
    Browse->OnTouch = []() {
        Tab1Graphics::Btn4Graphics::Browse(58, 16, Touched);
        OPENFILENAME ofn = {};
        TCHAR sFilename[LONG_MAX_PATH] = {};
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = GetConsoleWindow();
        ofn.lpstrFile = sFilename;
        ofn.lpstrInitialDir = Path.GetText().c_str();
        ofn.nMaxFile = sizeof(sFilename) / sizeof(TCHAR);
        ofn.lpstrFilter = TEXT("MIDI Files (*.mid)\0*.mid\0");
        ofn.lpstrTitle = L"Please select the file you want to convert: ";
        ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        if (GetOpenFileName(&ofn)) {
            Path.SetText(sFilename);
        }
    };
    Browse->OnHover = []() {
        Tab1Graphics::Btn4Graphics::Browse(58, 16, Hovered);
    };
    Browse->OnLeave = []() {
        Tab1Graphics::Btn4Graphics::Browse(58, 16, Normal);
    };
    Convert = TouchEventManager::Create();
    Convert->X = 56;
    Convert->Y = 19;
    Convert->W = 8;
    Convert->H = 0;
    Convert->OnTouch = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 19, Touched);
        wstring DIR = Path.GetText();
        wstring NoticeText = L"";
        NoticeText += L"Black MIDI templates have been installed to: \n";
        NoticeText += DIR;
        MessageBox(GetConsoleWindow(), NoticeText.c_str(), L"Success", MB_OK | MB_ICONEXCLAMATION);
    };
    Convert->OnHover = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 19, Hovered);
    };
    Convert->OnLeave = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 19, Normal);
    };
}