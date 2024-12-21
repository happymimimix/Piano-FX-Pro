#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <ConsoleSetupScript.h>
#include <resource.h>

int8_t RedrawTimer = 0;

struct Diagram {
    WORD BMPid = NULL;
    uint16_t X = 0;
    uint16_t Y = 0;
    uint16_t W = 0;
    uint16_t H = 0;
};

struct HighlightedArea {
    COLORREF Color = 0x0000FF; //BGR
    uint16_t X = 0;
    uint16_t Y = 0;
    uint16_t W = 0;
    uint16_t H = 0;
    uint8_t Thickness = 0;
};

struct TutorialDocument {
    uint16_t DocumentLength = 0;
    string TextContent = R"MultilineText(
)MultilineText";
    vector<Diagram> DiagramsList = {};
};

bool IsVisible(uint16_t ImageTop, uint16_t ImageHeight, uint16_t ViewTop, uint16_t ViewBottom) {
    uint16_t ImageBottom = ImageTop + ImageHeight;
    return (ImageTop <= ViewBottom || ImageBottom >= ViewTop);
}

void SetDocumentLength(uint16_t Length) {
    COORD bufferSize;
    bufferSize.X = 102;
    bufferSize.Y = max(Length,35);
    if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize)) {
        cout << "[1;1H[40m[91mERROR: Unable to set console buffer size. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
}

void ShowText(string Content) {
    COORD pos = {};
    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << Content;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void DrawBMP(Diagram BMP) {
    HWND ConsoleHWND = GetConsoleWindow();
    HDC ConsoleDC = GetDC(ConsoleHWND);
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo)) {
        cout << "[1;1H[40m[91mERROR: Cannot obtain console buffer info. \n\nReason: \n";
        cout << GetLastError();
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    uint16_t ViewTop = BufInfo.srWindow.Top;
    uint16_t ViewBottom = BufInfo.srWindow.Bottom;
    if (IsVisible(BMP.Y, BMP.H, ViewTop, ViewBottom)) {
        HDC MemDC = CreateCompatibleDC(ConsoleDC);
        HBITMAP LoadedBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(BMP.BMPid), IMAGE_BITMAP, NULL, NULL, NULL);
        SelectObject(MemDC, LoadedBMP);
        BITMAP BMPinfo;
        GetObject(LoadedBMP, sizeof(BITMAP), &BMPinfo);
        BLENDFUNCTION BlFunc = {};
        BlFunc.AlphaFormat = NULL;
        BlFunc.BlendFlags = NULL;
        BlFunc.BlendOp = NULL;
        BlFunc.SourceConstantAlpha = 0xFF;
        AlphaBlend(ConsoleDC, BMP.X * ChW, (BMP.Y - ViewTop) * ChH, BMP.W * ChW, BMP.H * ChH, MemDC, NULL, NULL, BMPinfo.bmWidth, BMPinfo.bmHeight, BlFunc);
        DeleteDC(MemDC);
        DeleteObject(LoadedBMP);
        ReleaseDC(GetConsoleWindow(), ConsoleDC);
    }
}

void OpenTutorialDocument(TutorialDocument Document) {
    ConsoleInitNoMouse();
    SetDocumentLength(Document.DocumentLength);
    ShowText(Document.TextContent);
    while (true) {
        for (Diagram BMP : Document.DiagramsList) {
            DrawBMP(BMP);
        }
        if (RedrawTimer < 0) {
            RedrawTimer = 0;
            InvalidateRect(GetConsoleWindow(), NULL, NULL);
        }
        RedrawTimer++;
    }
}