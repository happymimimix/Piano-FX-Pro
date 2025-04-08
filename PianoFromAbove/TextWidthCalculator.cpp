#include <iostream>
#include <string>
#include <Windows.h>
#include "imgui/Fonts.h"

using namespace std;

void ReplaceAll(wstring& INPUT, wstring BEFORE, wstring AFTER) {
    size_t i = 0;
    while ((i = INPUT.find(BEFORE, i)) != wstring::npos) {
        INPUT.replace(i, BEFORE.size(), AFTER);
        i += AFTER.size();
    }
}

int main() {
    SetConsoleOutputCP(65001);
    wcin.imbue(locale("en_US.UTF-8"));
    wcout.imbue(locale("en_US.UTF-8"));
    cerr << "Calculating text widths..." << endl;
    HWND cmd = GetConsoleWindow();
    HDC cmdDC = GetDC(cmd);
    HFONT hFont = CreateFontW(
        -MulDiv(FontSize, GetDeviceCaps(cmdDC, LOGPIXELSY), 72), //FontHeight
        0, //FontWidth
        0, //Escapement
        0, //Orientation
        0, //FontWeight
        0, //Italic
        0, //Underline
        0, //StrikeOut
        1, //CharSet
        0, //OutPrecision
        0, //ClipPrecision
        0, //Quality
        0, //PitchAndFamily
        TEXT(FontName) //FaceName
    );
    TEXTMETRIC cmdTM;
    GetTextMetricsW(cmdDC, &cmdTM);
    SelectObject(cmdDC, hFont);
    SetBkMode(cmdDC, 1);
    SetTextColor(cmdDC, 0x0000FF);
    HBRUSH hBrush = CreateSolidBrush(0x0000FF);
    SelectObject(cmdDC, hBrush);
    wstring Line = L"";
    while (getline(wcin, Line)) {
        if (Line.length() < 8 || Line.substr(0, 8) != L"#define ") {
            if ((Line.length() < 9 || Line.substr(0, 9) != L"#include ") && (Line.length() < 7 || Line.substr(0, 7) != L"#undef ")) {
                wcout << Line << endl;
            }
            continue;
        }
        else {
            wstring LineNdef = Line.substr(8, Line.length() - 8);
            size_t Splitter = LineNdef.find_first_of(L" ");
            if (Splitter != wstring::npos) {
                wstring DefName = LineNdef.substr(0, Splitter);
                wstring DefString = LineNdef.substr(Splitter + 1, LineNdef.length() - Splitter - 1);
                if (DefString.substr(0, 2) == L"L\"" && DefString.substr(DefString.length() - 1, 1) == L"\"") {
                    DefString = DefString.substr(2, DefString.length() - 3);
                }
                else if (DefString.substr(0, 1) == L"\"" && DefString.substr(DefString.length() - 1, 1) == L"\"") {
                    DefString = DefString.substr(1, DefString.length() - 2);
                }
                else {
                    continue;
                }
                ReplaceAll(DefString, L"\\\"", L"\"");
                ReplaceAll(DefString, L"\\r", L"\r");
                ReplaceAll(DefString, L"\\n", L"\n");
                ReplaceAll(DefString, L"\\t", L"\t");
                ReplaceAll(DefString, L"\\v", L"\v");
                ReplaceAll(DefString, L"\\b", L"\b");
                ReplaceAll(DefString, L"\\\\", L"\\");
                SIZE TextSize;
                GetTextExtentPoint32W(cmdDC, DefString.c_str(), DefString.length(), &TextSize);
                wcout << L"#define " << DefName << L"W " << MulDiv(TextSize.cx, 4, cmdTM.tmAveCharWidth*(0x1.fffffffffffffp-1)) << L" //Text: " << LineNdef.substr(Splitter + 1, LineNdef.length() - Splitter - 1) << endl;
                RECT cmdRECT;
                GetClientRect(cmd, &cmdRECT);
                LONG W = cmdRECT.right - cmdRECT.left;
                LONG H = cmdRECT.bottom - cmdRECT.top;
                BitBlt(cmdDC, 0, 0, W, H - cmdTM.tmHeight, cmdDC, 0, cmdTM.tmHeight, SRCCOPY);
                PatBlt(cmdDC, 0, H - cmdTM.tmHeight, W, cmdTM.tmHeight, BLACKNESS);
                TextOutW(cmdDC, 0, H - cmdTM.tmHeight, DefString.c_str(), DefString.length());
                PatBlt(cmdDC, TextSize.cx, H - cmdTM.tmHeight, 1, cmdTM.tmHeight, PATCOPY);
            }
        }
    }
    DeleteObject(hFont);
    DeleteObject(hBrush);
    ReleaseDC(cmd, cmdDC);
    Sleep(1 << 10);
    return 0;
}