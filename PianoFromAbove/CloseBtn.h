#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>

//The close button on the right top corner
struct CloseBtn {
    static bool Terminated;
    static void Create() {
        TouchEventListener* CloseBtn = TouchEventManager::Create();
        CloseBtn->X = 96;
        CloseBtn->Y = 1;
        CloseBtn->W = 2;
        CloseBtn->H = 0;
        CloseBtn->OnTouch = []() {
            Terminated = true;
        };
        CloseBtn->OnHover = []() {
            cout << "[2;98H[91mx";
        };
        CloseBtn->OnLeave = []() {
            cout << "[2;98H[93mx";
        };
    }
};
bool CloseBtn::Terminated = false;