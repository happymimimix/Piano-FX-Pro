#pragma once
#include <PFXSTUDIO_Global_Imports.h>

void Copy(const string& Content) {
    OpenClipboard(nullptr);
    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, Content.size() + 1);
    memcpy(GlobalLock(hMem), Content.c_str(), Content.size() + 1);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}