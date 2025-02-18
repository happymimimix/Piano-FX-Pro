#pragma once
#include <PFXSTUDIO_Global_Imports.h>

struct ConsoleTextbox {
    HWND hTextbox = NULL;

    void Create(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
        if (this->hTextbox != NULL) {
            cout << "[1;1H[40m[91mERROR: Trying to create a text box that already exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            MSG msg = {};
            if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            this->hTextbox = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                static_cast<int>(x) * ChW - ChW / 2, static_cast<int>(y) * ChH - ChH / 2, static_cast<int>(w) * ChW, static_cast<int>(h) * ChH,
                GetConsoleWindow(),
                NULL,
                NULL,
                NULL
            );
        }
    }

    wstring GetText() {
        if (this->hTextbox != NULL) {
            wstring Content(GetWindowTextLengthW(this->hTextbox), L'\0');
            GetWindowTextW(this->hTextbox, &Content[0], GetWindowTextLengthW(this->hTextbox) + 1);
            return Content;
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to grab text from a text box that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void SetText(const wstring& Content) {
        if (this->hTextbox != NULL) {
            SendMessage(this->hTextbox, WM_SETTEXT, 0, (LPARAM)Content.c_str());
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to set text to a text box that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Delete() {
        if (this->hTextbox != NULL) {
            DestroyWindow(this->hTextbox);
            this->hTextbox = NULL;
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to delete a text box that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Hide() {
        if (this->hTextbox != NULL) {
            ShowWindow(this->hTextbox, SW_HIDE);
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to hide a text box that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Show() {
        if (this->hTextbox != NULL) {
            ShowWindow(this->hTextbox, SW_SHOW);
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to show a text box that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }
};