#include <windows.h>

const wchar_t CLASS_NAME[] = L"PFXGDI";

// Global variable to hold the handle of the overlay window
HWND overlayWindow;
UINT_PTR timerId;

// Function to create the overlay window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Create a close button
        CreateWindowA("BUTTON", "Close",WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,0, 0, 60, 25, hwnd, (HMENU)999, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        // Close button clicked
        if (LOWORD(wParam) == 999) {
            DestroyWindow(hwnd);
        }
        break;
    }
    case WM_CLOSE: {
        DestroyWindow(hwnd);
        break;
    }
    case WM_DESTROY: {
        KillTimer(hwnd, 998);
        PostQuitMessage(0);
        break;
    }
    case WM_TIMER: {
        if (LOWORD(wParam) == 998) {
            HWND targetWindow = FindWindowW(L"PFX", NULL);
            HWND targetGFXWindow = FindWindowExW(targetWindow, NULL, L"PFXGFX", NULL);
            if (targetWindow && targetGFXWindow) {
                RECT rect;
                GetWindowRect(targetGFXWindow, &rect);
                SetWindowPos(overlayWindow, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);
                if (GetForegroundWindow() == targetWindow) {
                    SetWindowPos(overlayWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
                else {
                    SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(overlayWindow, &cursorPos);
                if (GetForegroundWindow() == overlayWindow && !(cursorPos.x >= 0 && cursorPos.x <= 60 && cursorPos.y >= 0 && cursorPos.y <= 25)) {
                    SetForegroundWindow(targetWindow);
                }
            }
            else {
                DestroyWindow(overlayWindow);
            }
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(overlayWindow, &cursorPos);
            if (cursorPos.x >= 0 && cursorPos.x <= 60 && cursorPos.y >= 0 && cursorPos.y <= 25) {
                InvalidateRect(GetDlgItem(overlayWindow, 999), NULL, TRUE); //Redraw the button
            }
        }
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void CreateOverlayWindow() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    HWND targetWindow = FindWindowW(L"PFX", NULL);
    if (targetWindow) {
        HWND targetGFXWindow = FindWindowExW(targetWindow, NULL, L"PFXGFX", NULL);
        if (targetGFXWindow) {
            RECT rect;
            GetWindowRect(targetGFXWindow, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            overlayWindow = CreateWindowEx(
                NULL,
                CLASS_NAME,
                L"Piano-FX GDI Overlay",
                WS_POPUP | WS_VISIBLE,
                rect.left, rect.top, width, height,
                NULL, NULL, GetModuleHandle(NULL), NULL
            );
            SetWindowPos(overlayWindow, HWND_TOP, rect.left, rect.top, width, height, SWP_NOMOVE);
            SetTimer(overlayWindow, 998, NULL, NULL);
        }
        else {
            MessageBoxA(overlayWindow, "Please run Piano-FX-Pro.exe first. ", "Error! ", MB_OK);
            KillTimer(overlayWindow, 998);
            PostQuitMessage(0);
        }
    }
    else {
        MessageBoxA(overlayWindow, "Please run Piano-FX-Pro.exe first. ", "Error! ", MB_OK);
        KillTimer(overlayWindow, 998);
        PostQuitMessage(0);
    }
}

// Main function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    CreateOverlayWindow();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
