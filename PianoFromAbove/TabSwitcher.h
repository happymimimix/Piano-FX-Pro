#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>

struct TabSwitcher {
    static TouchEventListener* Tab1;
    static TouchEventListener* Tab2;
    static TouchEventListener* Tab3;
    static TouchEventListener* Tab4;
    static uint8_t CurrentTab;

    static void EnableAll();
    static void DisableAll();
    static void EnableTab(uint8_t Tab);
    static void DisableTab(uint8_t Tab);
};