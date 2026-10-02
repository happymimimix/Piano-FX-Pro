/*
This tool is responsible for calculating text width for automatic layout generation. 
The results will be stored to Widths.h
*/

#include <iostream>
#include <string>
#include <Windows.h>
#include <Fonts.h>

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
    wcerr << L"Calculating text widths..." << endl;
    HWND cmd = GetConsoleWindow();
    HDC cmdDC = GetDC(cmd);
    HFONT hFont = CreateFontW(
        -MulDiv(FontSize, 96, 72), //FontHeight
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
    HGDIOBJ oldfont = SelectObject(cmdDC, hFont);
    SetBkMode(cmdDC, 1);
    SetTextColor(cmdDC, 0x0000FF);
    HBRUSH hBrush = CreateSolidBrush(0x00FFFF);
    HGDIOBJ oldbrush = SelectObject(cmdDC, hBrush);
    TEXTMETRIC cmdTM;
    GetTextMetricsW(cmdDC, &cmdTM);
    // I have no idea what this shit is but this is what OpenNT 4.5 is doing.
    static WCHAR wszAvgChars[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    SIZE AlphabetSize;
    GetTextExtentPointW(cmdDC, wszAvgChars, (sizeof(wszAvgChars)/sizeof(WCHAR)) - 1, &AlphabetSize);
    LONG baseX = ((AlphabetSize.cx / 26) + 1) / 2;
    LONG baseY = cmdTM.tmHeight;
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
                GetTextExtentPointW(cmdDC, DefString.c_str(), DefString.length(), &TextSize);
                wcout << L"#define " << DefName << L"W " << MulDiv(TextSize.cx, 4, baseX) << L" //Text: " << LineNdef.substr(Splitter + 1, LineNdef.length() - Splitter - 1) << endl;
                RECT cmdRECT;
                GetClientRect(cmd, &cmdRECT);
                LONG W = cmdRECT.right - cmdRECT.left;
                LONG H = cmdRECT.bottom - cmdRECT.top;
                BitBlt(cmdDC, 0, 0, W, H - baseY, cmdDC, 0, baseY, SRCCOPY);
                PatBlt(cmdDC, 0, H - baseY, W, baseY, BLACKNESS);
                TextOutW(cmdDC, 0, H - baseY, DefString.c_str(), DefString.length());
                PatBlt(cmdDC, TextSize.cx, H - baseY, 1, baseY, PATCOPY);
                wstring WidthText = L" <- " + to_wstring(MulDiv(TextSize.cx, 4, baseX));
                TextOutW(cmdDC, TextSize.cx, H - baseY, WidthText.c_str(), WidthText.length());
                HDC NULLDC = GetDC(NULL);
                PatBlt(NULLDC, 0, 0, 1, 1, PATINVERT);
                GdiFlush();
                ReleaseDC(NULL, NULLDC);
            }
        }
    }
    SelectObject(cmdDC, oldfont);
    DeleteObject(hFont);
    SelectObject(cmdDC, oldbrush);
    DeleteObject(hBrush);
    ReleaseDC(cmd, cmdDC);
    return 0;
}