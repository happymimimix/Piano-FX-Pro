#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <ConsoleTextbox.h>
#include <ConsoleDropdown.h>
#include <NullPtrChecker.h>
#include <TabSwitcher.h>
#include <Tab3Graphics.h>

//Tab 3 forward declearations
struct Tab3 {
    static TouchEventListener* Btn1;

    static void EnableAll();
    static void DisableAll();
    static void Draw();
};
