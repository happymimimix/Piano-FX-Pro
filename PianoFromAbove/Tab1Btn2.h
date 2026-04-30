#include <Tab1.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>
#include <flpTools.h>

TouchEventListener* Tab1::SubViewBtn2::Close = nullptr;
ConsoleTextbox Tab1::SubViewBtn2::Path = {};
TouchEventListener* Tab1::SubViewBtn2::Browse = nullptr;
TouchEventListener* Tab1::SubViewBtn2::Install = nullptr;

UINT_PTR CALLBACK FolderPickHook(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    OutputDebugStringA("Hook called!\n");
    if (msg == WM_NOTIFY) {
        OFNOTIFY* ofn = (OFNOTIFY*)lParam;
        HWND hParent = GetParent(hDlg);
        switch (ofn->hdr.code) {
        case CDN_INITDONE:
            SetWindowTextW(GetDlgItem(hParent, cmb13), L"Folder");
            SendMessageW(GetDlgItem(hParent, cmb13), EM_SETREADONLY, TRUE, NULL);
            EnableWindow(GetDlgItem(hParent, cmb13), FALSE);
            SetWindowTextW(GetDlgItem(hParent, cmb1), L"Folder");
            SendMessageW(GetDlgItem(hParent, cmb1), EM_SETREADONLY, TRUE, NULL);
            EnableWindow(GetDlgItem(hParent, cmb1), FALSE);
        case CDN_SELCHANGE:
            SetWindowTextW(GetDlgItem(hParent, cmb13), L"Folder");
        }
    }
    return 0;
}

void Tab1::SubViewBtn2::Open() {
    TabSwitcher::DisableAll();
    Tab1::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    DoTransition(3, 8, 50, 9, 3, 16, 0x00001000);
    FillConsole(3, 8, 50, 9, ' ', 0x0F);
    Tab1Graphics::Btn3(3, 16, Normal);
    Tab1Graphics::Btn4(3, 19, Normal);
    Tab1Graphics::Btn5(3, 22, Normal);
    // Draw the UI
    Tab1Graphics::Btn2Graphics::Main(3, 7);
    Close = TouchEventManager::Create();
    Close->X = 65;
    Close->Y = 7;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        Path.Delete();
        TouchEventManager::Delete(Browse);
        TouchEventManager::Delete(Install);
        FillConsole(3, 7, 65, 9, ' ', 0x0F);
        Tab1Graphics::Btn2(3, 5, Normal);
        DoTransition(3, 16, 50, 10, 3, 8, 0x00001000);
        TabSwitcher::EnableAll();
        Tab1::EnableAll();
        FillConsole(3, 17, 50, 9, ' ', 0x0F);
    };
    Close->OnHover = []() {
        cout << "[8;66H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[8;66H[44m[91mx";
    };
    Path.Create(6, 10, 51, 2);
    Path.SetText(L"C:\\Program Files (x86)\\Image-Line\\FL Studio 12");
    Browse = TouchEventManager::Create();
    Browse->X = 57;
    Browse->Y = 10;
    Browse->W = 7;
    Browse->H = 0;
    Browse->OnTouch = []() {
        Tab1Graphics::Btn2Graphics::Browse(58, 10, Touched);
        OPENFILENAME ofn = {};
        TCHAR sFilename[LONG_MAX_PATH] = TEXT("Folder");
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = GetConsoleWindow();
        ofn.lpstrFile = sFilename;
        ofn.lpstrInitialDir = Path.GetText().c_str();
        ofn.nMaxFile = sizeof(sFilename) / sizeof(TCHAR);
        ofn.lpstrFilter = TEXT("Folder\0*\0");
        ofn.lpstrTitle = L"Select FL Studio Installation DIR: ";
        ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_ENABLEHOOK | OFN_NOVALIDATE | OFN_PATHMUSTEXIST;
        ofn.lpfnHook = FolderPickHook;
        if (GetOpenFileName(&ofn)) {
            *wcsrchr(sFilename, '\\') = 0;
            Path.SetText(sFilename);
        }
    };
    Browse->OnHover = []() {
        Tab1Graphics::Btn2Graphics::Browse(58, 10, Hovered);
    };
    Browse->OnLeave = []() {
        Tab1Graphics::Btn2Graphics::Browse(58, 10, Normal);
    };
    Install = TouchEventManager::Create();
    Install->X = 56;
    Install->Y = 13;
    Install->W = 8;
    Install->H = 0;
    Install->OnTouch = []() {
        Tab1Graphics::Btn2Graphics::Install(57, 13, Touched);
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
        Tab1Graphics::Btn2Graphics::Install(57, 13, Hovered);
    };
    Install->OnLeave = []() {
        Tab1Graphics::Btn2Graphics::Install(57, 13, Normal);
    };
}
