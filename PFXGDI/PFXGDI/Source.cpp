#include <windows.h>

const wchar_t CLASS_NAME[] = L"PFXGDI";

// Global variable to hold the handle of the overlay window
HWND overlayWindow;
UINT_PTR timerId;
RECT PrevRect;

bool IsAbove(HWND hwndTop, HWND hwndBottom) {
    HWND hwnd = hwndTop;
    while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL) {
        if (hwnd == hwndBottom)
            return true;  // found hwndBottom below hwndTop
    }
    return false;
}

bool IsDirectlyAbove(HWND hwndTop, HWND hwndBottom) {
    HWND hwnd = hwndTop;
    while ((hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) != NULL) {
        if (hwnd == hwndBottom)
            return true;
        if (IsWindowVisible(hwnd) && !IsIconic(hwnd))
            return false;
    }
    return false;
}

// Function to create the overlay window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Create a close button
        CreateWindowA("BUTTON", "Close", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 0, 0, 60, 25, hwnd, (HMENU)999, NULL, NULL);
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
            SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_SENDCHANGE); // Fuck you Microsoft! 
            HWND targetWindow = FindWindowW(L"PFX", NULL);
            HWND targetGFXWindow = FindWindowExW(targetWindow, NULL, L"PFXGFX", NULL);
            if (targetWindow && targetGFXWindow) {
                RECT WindowRect;
                GetWindowRect(targetGFXWindow, &WindowRect);
                SetWindowPos(overlayWindow, HWND_NOTOPMOST, WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, SWP_NOREDRAW | SWP_NOACTIVATE);
                if (WindowRect.left != PrevRect.left || WindowRect.top != PrevRect.top || WindowRect.right != PrevRect.right || WindowRect.bottom != PrevRect.bottom) {
                    InvalidateRect(GetDlgItem(overlayWindow, 999), NULL, TRUE); //Redraw the button
                    PrevRect = WindowRect;
                }
                if (GetForegroundWindow() == targetWindow && IsWindowEnabled(targetWindow)) {
                    if (!IsAbove(overlayWindow, targetWindow)) {
                        SetWindowPos(overlayWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        SetWindowPos(targetWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    }
                    else {
                        if (!IsDirectlyAbove(overlayWindow, targetWindow)) {
                            SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                            SetForegroundWindow(targetWindow);
                        }
                        else {
                            SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        }
                    }
                }
                else {
                    if (!IsDirectlyAbove(overlayWindow, targetWindow) && IsWindowEnabled(targetWindow)) {
                        SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        HDWP hdwp = BeginDeferWindowPos(2);
                        hdwp = DeferWindowPos(hdwp, overlayWindow, targetWindow, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        hdwp = DeferWindowPos(hdwp, targetWindow, overlayWindow, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                        EndDeferWindowPos(hdwp);
                    }
                    else {
                        SetWindowPos(overlayWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    }
                }
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(overlayWindow, &cursorPos);
                if (GetForegroundWindow() == overlayWindow && !(cursorPos.x >= 0 && cursorPos.x <= 60 && cursorPos.y >= 0 && cursorPos.y <= 25)) {
                    SetForegroundWindow(targetWindow);
                }
                if (cursorPos.x >= 0 && cursorPos.x <= 60 && cursorPos.y >= 0 && cursorPos.y <= 25) {
                    InvalidateRect(GetDlgItem(overlayWindow, 999), NULL, TRUE); //Redraw the button
                }
            }
            else {
                DestroyWindow(overlayWindow);
            }
        }
        break;
    }
    default: {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
    }
    return 0;
}

void CreateOverlayWindow() {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_SAVEBITS;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = NULL;
    wcex.cbWndExtra = NULL;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wcex);
    HWND targetWindow = FindWindowW(L"PFX", NULL);
    if (targetWindow) {
        HWND targetGFXWindow = FindWindowExW(targetWindow, NULL, L"PFXGFX", NULL);
        if (targetGFXWindow) {
            RECT WindowRect;
            GetWindowRect(targetGFXWindow, &WindowRect);
            overlayWindow = CreateWindowEx(
                NULL,
                CLASS_NAME,
                L"Piano-FX GDI Overlay",
                WS_POPUP | WS_VISIBLE,
                WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
                NULL, NULL, GetModuleHandle(NULL), NULL
            );
            SetWindowPos(overlayWindow, HWND_TOP, WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, SWP_NOREDRAW);
            PrevRect = WindowRect;
            SetForegroundWindow(overlayWindow);
            SetTimer(overlayWindow, 998, 1, NULL);
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
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd) {
    CreateOverlayWindow();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterClassW(CLASS_NAME, GetModuleHandle(NULL));
    return 0;
}
