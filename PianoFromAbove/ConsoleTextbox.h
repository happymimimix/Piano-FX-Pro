#pragma once
#include <PFXSTUDIO_Global_Imports.h>

struct ConsoleTextbox {
    HWND hDropdown = NULL;

    void Create(int x, int y, int w, int h) {
        this->hDropdown = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            x, y, w, h,
            GetConsoleWindow(),
            NULL,
            NULL,
            NULL
        );
    }

    wstring GetText() {
        if (this->hDropdown != NULL) {
            wstring Content(GetWindowTextLengthW(this->hDropdown), L'\0');
            GetWindowTextW(this->hDropdown, &Content[0], GetWindowTextLengthW(this->hDropdown) + 1);
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
        if (this->hDropdown != NULL) {
            SendMessage(this->hDropdown, WM_SETTEXT, 0, (LPARAM)Content.c_str());
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
        if (this->hDropdown != NULL) {
            DestroyWindow(this->hDropdown);
            this->hDropdown = NULL;
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
};