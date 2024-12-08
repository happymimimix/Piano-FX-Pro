#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <TabSwitcher.h>

struct Tab1 {
    static TouchEventListener* Btn1;
    static TouchEventListener* Btn2;
    static TouchEventListener* Btn3;
    static TouchEventListener* Btn4;
    static TouchEventListener* Btn5;

    static void EnableAll();
    static void DisableAll();
    static void Draw();
    static void OpenSubView_Btn3();
};