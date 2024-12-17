#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <TabSwitcher.h>
#include <ConsoleTextbox.h>

struct Tab1 {
    static TouchEventListener* Btn1;
    static TouchEventListener* Btn2;
    static TouchEventListener* Btn3;
    static TouchEventListener* Btn4;
    static TouchEventListener* Btn5;

    static void EnableAll();
    static void DisableAll();
    static void Draw();

    struct Btn3 {
        static TouchEventListener* Close;
        static ConsoleTextbox Path;

        static void Open();
    };
};