#pragma once
#include <PFXSTUDIO_Global_Imports.h>

void Copy(const wstring& Content) {
    OpenClipboard(nullptr);
    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (Content.size() + 1) * sizeof(wchar_t));
    memcpy(GlobalLock(hMem), Content.c_str(), (Content.size() + 1) * sizeof(wchar_t));
    GlobalUnlock(hMem);
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();
}