#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <ConsoleTextbox.h>
#include <ConsoleDropdown.h>
#include <ConsoleNumberSpinner.h>
#include <NullPtrChecker.h>
#include <TabSwitcher.h>
#include <Tab1Graphics.h>

//Tab 1 forward declearations
struct Tab1 {
    static TouchEventListener* Btn1;
    static TouchEventListener* Btn2;
    static TouchEventListener* Btn3;
    static TouchEventListener* Btn4;
    static TouchEventListener* Btn5;

    static void EnableAll();
    static void DisableAll();
    static void Draw();

    struct SubViewBtn2 {
        static TouchEventListener* Close;
        static ConsoleTextbox Path;
        static TouchEventListener* Browse;
        static TouchEventListener* Install;

        static void Open();
    };

    struct SubViewBtn4 {
        static TouchEventListener* Close;
        static ConsoleTextbox Path;
        static TouchEventListener* Browse;
        static TouchEventListener* Convert;

        static void Open();
    };

    struct SubViewBtn5 {
        static TouchEventListener* Close;
        static ConsoleTextbox Path;
        static TouchEventListener* Browse;
        static TouchEventListener* Convert;

        static void Open();
    };
};