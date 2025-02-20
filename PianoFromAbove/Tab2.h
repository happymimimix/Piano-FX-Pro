#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <ConsoleTextbox.h>
#include <ConsoleDropdown.h>
#include <ConsoleNumberSpinner.h>
#include <NullPtrChecker.h>
#include <TabSwitcher.h>
#include <Tab2Graphics.h>

struct Tab2 {
    static TouchEventListener* Btn1;
    static TouchEventListener* Btn2;
    static TouchEventListener* Btn3;
    static TouchEventListener* Btn4;

    static void EnableAll();
    static void DisableAll();
    static void Draw();

    struct SubViewBtn4 {
        static size_t ItemsCount;
        static wstring Code;
        static TouchEventListener* Close;
        static ConsoleDropdown AnimationType1;
        static ConsoleDropdown AnimationType2;
        static ConsoleDropdown AnimationTarget1;
        static ConsoleDropdown AnimationTarget2;
        static ConsoleDropdown TimingUnit;
        static ConsoleTextbox StartTime;
        static ConsoleTextbox EndTime;
        static ConsoleTextbox StartValue;
        static ConsoleTextbox EndValue;
        static ConsoleTextbox SliceInterval;
        static TouchEventListener* UseThreads;
        static TouchEventListener* Generate;
        static bool Thread;

        static void Open();
    };
};