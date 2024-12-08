#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <TouchEventListener.h>
#include <TabSwitcher.h>

struct Tab2 {
    static TouchEventListener* Btn1;

    static void EnableAll();
    static void DisableAll();
    static void Draw();
};