#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <ConsoleFillingTool.h>

uint32_t inline ProgressCalc(uint32_t Begin, uint32_t End, uint32_t Progress, uint32_t ProgressMAX) {
    float fBegin = static_cast<float>(Begin);
    float fEnd = static_cast<float>(End);
    float fProgress = static_cast<float>(Progress) / static_cast<float>(ProgressMAX);
    float Result = (fEnd - fBegin) * fProgress + fBegin;
    return static_cast<uint32_t>(round(Result));
}

void inline DoTransition(uint8_t ogX, uint8_t ogY, uint8_t W, uint8_t H, uint8_t destX, uint8_t destY, uint32_t Speed) {
    HDC ConsoleDC = GetDC(GetConsoleWindow());
    HDC MemDC = CreateCompatibleDC(ConsoleDC);
    HBITMAP BMP = CreateCompatibleBitmap(ConsoleDC, W * ChW, H * ChH);
    SelectObject(MemDC, BMP);
    BitBlt(MemDC, 0, 0, W * ChW, H * ChH, ConsoleDC, ogX * ChW, ogY * ChH, SRCCOPY);
    for (uint32_t Progress = 0; Progress < Speed; Progress++) {
        BitBlt(ConsoleDC, ProgressCalc(ogX * ChW, destX * ChW, Progress, Speed), ProgressCalc(ogY * ChH, destY * ChH, Progress, Speed), W * ChW, H * ChH, MemDC, 0, 0, SRCCOPY);
        _mm_pause();
    }
    BitBlt(ConsoleDC,destX * ChW, destY * ChH, W * ChW, H * ChH, MemDC, 0, 0, SRCCOPY);
    DeleteDC(MemDC);
    DeleteObject(BMP);
    ReleaseDC(GetConsoleWindow(), ConsoleDC);
}