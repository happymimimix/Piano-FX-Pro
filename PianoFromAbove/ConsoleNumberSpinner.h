#pragma once
#include <PFXSTUDIO_Global_Imports.h>

struct ConsoleNumberSpinner {
    HWND hEdit = NULL;
    HWND hSpin = NULL;

    void Create(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
        if (this->hEdit != NULL || this->hSpin != NULL) {
            cout << "[1;1H[40m[91mERROR: Trying to create a spinner that already exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            this->hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
                static_cast<int>(x) * ChW - ChW / 2, static_cast<int>(y) * ChH - ChH / 2, static_cast<int>(w) * ChW, static_cast<int>(h) * ChH,
                GetConsoleWindow(),
                NULL,
                NULL,
                NULL
            );
            this->hSpin = CreateWindowEx(
                0,
                UPDOWN_CLASS,
                NULL,
                WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ARROWKEYS,
                (static_cast<int>(x) * ChW - ChW / 2) + (static_cast<int>(w) * ChW), static_cast<int>(y) * ChH - ChH / 2, ChW, static_cast<int>(h) * ChH,
                GetConsoleWindow(),
                NULL,
                NULL,
                NULL
            );
            SendMessage(this->hSpin, UDM_SETBUDDY, (WPARAM)hEdit, 0);
            💬();
        }
    }

    void SetLimits(int MinLimit, int MaxLimit) {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            SendMessage(this->hSpin, UDM_SETRANGE32, (WPARAM)MinLimit, (LPARAM)MaxLimit);
        }
        else {
        cout << "[1;1H[40m[91mERROR: Trying to set limit to a spinner that doesn't exist! \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
        }
        💬();
    }

    int GetValue() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            int Content = SendMessage(this->hSpin, UDM_GETPOS32, 0, 0);
            💬();
            return Content;
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to grab number from a spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void SetValue(int Content) {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            SendMessage(this->hSpin, UDM_SETPOS32, 0, Content);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to set number to a spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Delete() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            DestroyWindow(this->hEdit);
            DestroyWindow(this->hSpin);
            this->hEdit = NULL;
            this->hSpin = NULL;
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to delete a number spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Hide() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            ShowWindow(this->hEdit, SW_HIDE);
            ShowWindow(this->hSpin, SW_HIDE);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to hide a number spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Show() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            ShowWindow(this->hEdit, SW_SHOW);
            ShowWindow(this->hSpin, SW_SHOW);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to show a number spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Disable() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            EnableWindow(this->hEdit, false);
            EnableWindow(this->hSpin, false);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to disable a number spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }

    void Enable() {
        if (this->hEdit != NULL && this->hSpin != NULL) {
            EnableWindow(this->hEdit, true);
            EnableWindow(this->hSpin, true);
            💬();
        }
        else {
            cout << "[1;1H[40m[91mERROR: Trying to enable a number spinner that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
    }
};