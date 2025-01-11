#pragma once
#include <PFXSTUDIO_Global_Imports.h>

void inline FillConsole(SHORT X, SHORT Y, SHORT W, SHORT H, char CH, WORD Color) {
    COORD Pos = { X, Y };
    DWORD Erased;
    DWORD Wiped;
    for (SHORT Line = 0; Line < H; Line++) {
        FillConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), CH, W, Pos, &Erased);
        FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color, W, Pos, &Wiped);
        Pos.Y++;
    }
}