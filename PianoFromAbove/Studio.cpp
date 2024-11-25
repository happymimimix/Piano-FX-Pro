#include <iostream>
#include <fstream>
#include <Windows.h>
#include <Studio.h>
#include <Colorizer.exe.h>
#include <PFXGDI.exe.h>
#include <ConGui.h>
#include <Shlwapi.h>

using namespace std;

string ProgramDIR() {
    char szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

void DrawBG() {
    cout << "[40m[96m╔══════════════════════════════════════════════════════════════════════════════════════════════╤═══╗\n";
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
    cout << "[40m[96m╚══════════════════════════════════════════════════════════════════════════════════════════════════╝";
}

void StudioMain() {
    // Set console window size
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = {};
    fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(hConsole, false, &fontInfo);
    fontInfo.dwFontSize.X = 10;
    fontInfo.dwFontSize.Y = 18;
    wcscpy_s(fontInfo.FaceName, L"Terminal");
    SetCurrentConsoleFontEx(hConsole, false, &fontInfo);
    COORD bufferSize = {100,35};
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SMALL_RECT windowSize = {0,0,100,35};
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
    ConGui::Init();
    // Clear screen
    cout << "[2J[H[?25l";
    string ColorizerPath = ProgramDIR() + "\\Colorizer.exe";
    if (PathFileExistsA(ColorizerPath.c_str()) == 0) {
        ofstream ColorizerFile(ColorizerPath, ios::binary);
        ColorizerFile.write(reinterpret_cast<const char*>(Colorizer_exe), Colorizer_exe_len);
        ColorizerFile.close();
    }
    string PFXGDIPath = ProgramDIR() + "\\PFXGDI.exe";
    if (PathFileExistsA(PFXGDIPath.c_str()) == 0) {
        ofstream PFXGDIFile(PFXGDIPath, ios::binary);
        PFXGDIFile.write(reinterpret_cast<const char*>(PFXGDI_exe), PFXGDI_exe_len);
        PFXGDIFile.close();
    }

    DrawBG();
    
    while (true) {

    }
}