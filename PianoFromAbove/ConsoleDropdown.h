#pragma once
#include <PFXSTUDIO_Global_Imports.h>

struct ConsoleDropdown {
    HWND hDropdown = NULL;

    void Create(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
        if (this->hDropdown != NULL) {
            cout << "[1;1H[40m[91mERROR: Trying to create a dropdown menu that already exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            this->hDropdown = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"COMBOBOX",
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST | WS_VSCROLL,
                static_cast<int>(x) * ChW - ChW / 2, static_cast<int>(y) * ChH - ChH / 4, static_cast<int>(w) * ChW, static_cast<int>(h) * ChH - ChH / 2,
                GetConsoleWindow(),
                NULL,
                NULL,
                NULL
            );
            💬();
        }
    }

    void AddItem(const wchar_t* item) {
        if (this->hDropdown != NULL) {
            SendMessageW(this->hDropdown, CB_ADDSTRING, 0, (LPARAM)item);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to add items to a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    int GetSelection() {
        if (this->hDropdown != NULL) {
            int Choice = SendMessageW(this->hDropdown, CB_GETCURSEL, 0, 0);
            💬();
            return Choice;
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to grab selection from a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    wstring GetSelectionText() {
        if (this->hDropdown != NULL) {
            int IDX = SendMessageW(this->hDropdown, CB_GETCURSEL, 0, 0);
            wstring Content(SendMessageW(this->hDropdown, CB_GETLBTEXTLEN, IDX, 0), L'\0');
            SendMessageW(this->hDropdown, CB_GETLBTEXT, IDX, (LPARAM)&Content[0]);
            💬();
            return Content;
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to grab selection from a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void SetSelection(int IDX) {
        if (this->hDropdown != NULL) {
            if (IDX >= 0 && IDX < SendMessageW(this->hDropdown, CB_GETCOUNT, 0, 0)) {
                SendMessageW(this->hDropdown, CB_SETCURSEL, IDX, 0);
                💬();
            }
            else {
                cout << "[1;1H[40m[91mERROR: Cannot set selection index ";
                cout << IDX;
                cout << " to a dropdown menu that has only ";
                cout << SendMessageW(this->hDropdown, CB_GETCOUNT, 0, 0);
                cout << " items! \n";
                cout << "\nThe program will now stop. \n";
                while (true) {
                    // Make the program hang instead of closing! 
                    // This way the user can clearly see the error message. 
                }
            }
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to set selection to a dropdown menu that doesn't exist! \n";
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
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to delete a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Hide() {
        if (this->hDropdown != NULL) {
            ShowWindow(this->hDropdown, SW_HIDE);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to hide a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Show() {
        if (this->hDropdown != NULL) {
            ShowWindow(this->hDropdown, SW_SHOW);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to show a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Disable() {
        if (this->hDropdown != NULL) {
            EnableWindow(this->hDropdown, false);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to disable a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Enable() {
        if (this->hDropdown != NULL) {
            EnableWindow(this->hDropdown, true);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to enable a dropdown menu that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }
};