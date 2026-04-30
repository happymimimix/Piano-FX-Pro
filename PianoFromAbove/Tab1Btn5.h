#include <Tab1.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>
#include <flpTools.h>

TouchEventListener* Tab1::SubViewBtn5::Close = nullptr;
ConsoleTextbox Tab1::SubViewBtn5::Path = {};
TouchEventListener* Tab1::SubViewBtn5::Browse = nullptr;
TouchEventListener* Tab1::SubViewBtn5::Convert = nullptr;

void Tab1::SubViewBtn5::Open() {
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    // Draw the UI
    Tab1Graphics::Btn4Graphics::Main(3, 16);
    Close = TouchEventManager::Create();
    Close->X = 65;
    Close->Y = 16;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        Path.Delete();
        TouchEventManager::Delete(Browse);
        TouchEventManager::Delete(Convert);
        FillConsole(3, 16, 65, 9, ' ', 0x0F);
        Tab1Graphics::Btn5(3, 14, Normal);
        TabSwitcher::EnableAll();
        Tab1::EnableAll();
    };
    Close->OnHover = []() {
        cout << "[17;66H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[17;66H[44m[91mx";
    };
    Path.Create(6, 19, 51, 2);
    Browse = TouchEventManager::Create();
    Browse->X = 57;
    Browse->Y = 19;
    Browse->W = 7;
    Browse->H = 0;
    Browse->OnTouch = []() {
        Tab1Graphics::Btn4Graphics::Browse(58, 19, Touched);
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
        Tab1Graphics::Btn4Graphics::Browse(58, 19, Hovered);
    };
    Browse->OnLeave = []() {
        Tab1Graphics::Btn4Graphics::Browse(58, 19, Normal);
    };
    Convert = TouchEventManager::Create();
    Convert->X = 56;
    Convert->Y = 22;
    Convert->W = 8;
    Convert->H = 0;
    Convert->OnTouch = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 22, Touched);
        wstring DIR = Path.GetText();
        wstring NoticeText = L"";
        NoticeText += L"Black MIDI templates have been installed to: \n";
        NoticeText += DIR;
        MessageBox(GetConsoleWindow(), NoticeText.c_str(), L"Success", MB_OK | MB_ICONEXCLAMATION);
    };
    Convert->OnHover = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 22, Hovered);
    };
    Convert->OnLeave = []() {
        Tab1Graphics::Btn4Graphics::Convert(57, 22, Normal);
    };
}