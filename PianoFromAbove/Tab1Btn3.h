#include <Tab1.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>
#include <flpTools.h>

TouchEventListener* Tab1::SubViewBtn3::Close = nullptr;
ConsoleTextbox Tab1::SubViewBtn3::Path = {};
TouchEventListener* Tab1::SubViewBtn3::Browse = nullptr;
TouchEventListener* Tab1::SubViewBtn3::Install = nullptr;

void Tab1::SubViewBtn3::Open() {
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    // Move the last two buttons downwards
    DoTransition(3, 11, 46, 6, 3, 19, 0x00001000, true, 0x0F);
    // Don't let the last two buttons disappear on minimizing
    Tab1Graphics::Btn4(3, 19, Normal);
    Tab1Graphics::Btn5(3, 22, Normal);
    // Draw the UI
    Tab1Graphics::Btn3Graphics::Main(3, 10);
    Close = TouchEventManager::Create();
    Close->X = 65;
    Close->Y = 10;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        Path.Delete();
        TouchEventManager::Delete(Browse);
        TouchEventManager::Delete(Install);
        FillConsole(3, 8, 65, 11, ' ', 0x0F);
        Tab1Graphics::Btn3(3, 8, Normal);
        // Move the last two buttons back upwards
        DoTransition(3, 19, 46, 7, 3, 11, 0x00001000, true, 0x0F);
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
    Path.Create(6, 13, 51, 2);
    Path.SetText(L"C:\\Program Files (x86)\\Image-Line\\FL Studio 12");
    Browse = TouchEventManager::Create();
    Browse->X = 57;
    Browse->Y = 13;
    Browse->W = 7;
    Browse->H = 0;
    Browse->OnTouch = []() {
        Tab1Graphics::Btn3Graphics::Browse(58, 13, Touched);
        wchar_t DIR[MAX_PATH] = {};
        BROWSEINFOW BI = {};
        BI.lpszTitle = L"Select FL Studio Installation DIR: ";
        BI.ulFlags = NULL;

        LPITEMIDLIST pidl = SHBrowseForFolder(&BI);
        if (pidl && SHGetPathFromIDList(pidl, DIR)) {
            CoTaskMemFree(pidl);
            Path.SetText(DIR);
        }
    };
    Browse->OnHover = []() {
        Tab1Graphics::Btn3Graphics::Browse(58, 13, Hovered);
    };
    Browse->OnLeave = []() {
        Tab1Graphics::Btn3Graphics::Browse(58, 13, Normal);
    };
    Install = TouchEventManager::Create();
    Install->X = 56;
    Install->Y = 16;
    Install->W = 8;
    Install->H = 0;
    Install->OnTouch = []() {
        Tab1Graphics::Btn3Graphics::Install(57, 16, Touched);
        wstring DIR = Path.GetText();
        DIR += L"\\Data\\Projects\\Templates\\BlackMIDI";
        CreateDirectory(DIR.c_str(), NULL);
        uint16_t PPQ = 1 << 6;
        while (PPQ <= 1 << 12) {
            PPQ = PPQ << 1;
            wstring SubDIR = DIR;
            SubDIR += L"\\";
            SubDIR += to_wstring(PPQ);
            SubDIR += L"ppq";
            CreateDirectory(SubDIR.c_str(), NULL);
            SubDIR += L"\\";
            SubDIR += to_wstring(PPQ);
            SubDIR += L"ppq.nfo";
            fstream NFO(SubDIR, ios::out);
            NFO << "Description=New black midi project with a resolution of ";
            NFO << to_string(PPQ);
            NFO << "ppq. \n";
            NFO << "MenuIconIndex=-96";
            NFO.close();
            SubDIR = DIR;
            SubDIR += L"\\";
            SubDIR += to_wstring(PPQ);
            SubDIR += L"ppq";
            SubDIR += L"\\";
            SubDIR += to_wstring(PPQ);
            SubDIR += L"ppq.flp";
            CreateFLP(SubDIR, PPQ);
        }
        wstring NoticeText = L"";
        NoticeText += L"Black MIDI templates have been installed to: \n";
        NoticeText += DIR;
        MessageBox(GetConsoleWindow(), NoticeText.c_str(), L"Success", MB_OK | MB_ICONEXCLAMATION);
    };
    Install->OnHover = []() {
        Tab1Graphics::Btn3Graphics::Install(57, 16, Hovered);
    };
    Install->OnLeave = []() {
        Tab1Graphics::Btn3Graphics::Install(57, 16, Normal);
    };
}