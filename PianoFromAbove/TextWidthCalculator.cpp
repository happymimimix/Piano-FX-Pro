#define UNICODE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include "matrices.h"
#include <locale>
#include <codecvt>

using namespace std;

size_t CalculateTextWidth(wstring text) {
    HDC DC = GetDC(NULL);
    HFONT hFont = CreateFont(
        FontSize, //FontHeight
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
    HFONT oldFont = (HFONT)SelectObject(DC, hFont);
    SIZE size;
    GetTextExtentPoint32(DC, text.c_str(), text.length(), &size);
    SelectObject(DC, oldFont);
    DeleteObject(hFont);
    ReleaseDC(NULL, DC);
    return size.cx;
}

void ReplaceAll(wstring& INPUT, wstring BEFORE, wstring AFTER) {
    size_t i = 0;
    while ((i = INPUT.find(BEFORE, i)) != wstring::npos) {
        INPUT.replace(i, BEFORE.size(), AFTER);
        i += AFTER.size();
    }
}

int main() {
    wifstream INPUT("Language.h");
    INPUT.imbue(locale("en_US.UTF-8"));
    wofstream OUTPUT("Widths.h");
    OUTPUT.imbue(locale("en_US.UTF-8"));
    wstring Line = L"";
    while (getline(INPUT, Line)) {
        if (Line.length() < 8 || Line.substr(0, 8) != L"#define ") {
            if ((Line.length() < 9 || Line.substr(0, 9) != L"#include ") && (Line.length() < 7 || Line.substr(0, 7) != L"#undef ")) {
                OUTPUT << Line << endl;
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
                if (
                    DefString[DefString.length() - 1] == L':' ||
                    DefString[DefString.length() - 1] == L';' ||
                    DefString[DefString.length() - 1] == L',' ||
                    DefString[DefString.length() - 1] == L'.' ||
                    DefString[DefString.length() - 1] == L'?' ||
                    DefString[DefString.length() - 1] == L'!' ||
                    DefString[DefString.length() - 1] == L'"' ||
                    DefString[DefString.length() - 1] == L'\''
                    ) {
                    DefString += ' ';
                }
                size_t Width = CalculateTextWidth(DefString);
                OUTPUT << L"#define " << DefName << L"W " << Width+2 << endl;
            }
        }
    }
    INPUT.close();
    OUTPUT.close();
    return 0;
}