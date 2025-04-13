/*
⚠️ Disguised header file notice: 

This file will be treated as a .cpp source code file when compiling. 
Please do not be deceived by the .h file extension! 
See file properties for more details. 
*/

#include <TabSwitcher.h>
#include <Tab1.h>
#include <Tab2.h>
#include <Tab3.h>
#include <Tab4.h>
#include <Background.h>

TouchEventListener* TabSwitcher::Tab1 = nullptr;
TouchEventListener* TabSwitcher::Tab2 = nullptr;
TouchEventListener* TabSwitcher::Tab3 = nullptr;
TouchEventListener* TabSwitcher::Tab4 = nullptr;
uint8_t TabSwitcher::CurrentTab = 1;

void TabSwitcher::EnableAll() {
    if (Tab1 != nullptr || Tab2 != nullptr || Tab3 != nullptr || Tab4 != nullptr) {
        cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        Tab1 = TouchEventManager::Create();
        Tab1->X = 2;
        Tab1->Y = 33;
        Tab1->W = 6;
        Tab1->H = 0;
        Tab1->OnTouch = []() {
            if (TabSwitcher::CurrentTab != 1) {
                DisableTab(TabSwitcher::CurrentTab);
                TabSwitcher::CurrentTab = 1;
                EnableTab(TabSwitcher::CurrentTab);
            }
        };
        Tab1->OnHover = []() {
            if (TabSwitcher::CurrentTab != 1) {
                cout << "[34;4H[93mSetup";
            }
        };
        Tab1->OnLeave = []() {
            if (TabSwitcher::CurrentTab != 1) {
                cout << "[34;4H[91mSetup";
            }
        };
        Tab2 = TouchEventManager::Create();
        Tab2->X = 11;
        Tab2->Y = 33;
        Tab2->W = 8;
        Tab2->H = 0;
        Tab2->OnTouch = []() {
            if (TabSwitcher::CurrentTab != 2) {
                DisableTab(TabSwitcher::CurrentTab);
                TabSwitcher::CurrentTab = 2;
                EnableTab(TabSwitcher::CurrentTab);
            }
        };
        Tab2->OnHover = []() {
            if (TabSwitcher::CurrentTab != 2) {
                cout << "[34;13H[93mAnimate";
            }
        };
        Tab2->OnLeave = []() {
            if (TabSwitcher::CurrentTab != 2) {
                cout << "[34;13H[91mAnimate";
            }
        };
        Tab3 = TouchEventManager::Create();
        Tab3->X = 22;
        Tab3->Y = 33;
        Tab3->W = 9;
        Tab3->H = 0;
        Tab3->OnTouch = []() {
            if (TabSwitcher::CurrentTab != 3) {
                DisableTab(TabSwitcher::CurrentTab);
                TabSwitcher::CurrentTab = 3;
                EnableTab(TabSwitcher::CurrentTab);
            }
        };
        Tab3->OnHover = []() {
            if (TabSwitcher::CurrentTab != 3) {
                cout << "[34;24H[93mColorize";
            }
        };
        Tab3->OnLeave = []() {
            if (TabSwitcher::CurrentTab != 3) {
                cout << "[34;24H[91mColorize";
            }
        };
        Tab4 = TouchEventManager::Create();
        Tab4->X = 34;
        Tab4->Y = 33;
        Tab4->W = 4;
        Tab4->H = 0;
        Tab4->OnTouch = []() {
            if (TabSwitcher::CurrentTab != 4) {
                DisableTab(TabSwitcher::CurrentTab);
                TabSwitcher::CurrentTab = 4;
                EnableTab(TabSwitcher::CurrentTab);
            }
        };
        Tab4->OnHover = []() {
            if (TabSwitcher::CurrentTab != 4) {
                cout << "[34;36H[93mGDI";
            }
        };
        Tab4->OnLeave = []() {
            if (TabSwitcher::CurrentTab != 4) {
                cout << "[34;36H[91mGDI";
            }
        };
    }
}

void TabSwitcher::DisableAll() {
    TouchEventManager::Delete(Tab1);
    TouchEventManager::Delete(Tab2);
    TouchEventManager::Delete(Tab3);
    TouchEventManager::Delete(Tab4);
}

void TabSwitcher::EnableTab(uint8_t Tab) {
    DrawBG();
    switch (Tab) {
    case 1:
        cout << "[33;2H[40m[96m╕       ╒╤═════════╤╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [92mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab1::EnableAll();
        break;
    case 2:
        cout << "[33;2H[40m[96m╤═══════╤╕         ╒╤══════════╤╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [92mAnimate[96m ││ [91mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab2::EnableAll();
        break;
    case 3:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╕          ╒╤═════╤";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [92mColorize[96m ││ [91mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab3::EnableAll();
        break;
    case 4:
        cout << "[33;2H[40m[96m╤═══════╤╤═════════╤╤══════════╤╕     ╒";
        cout << "[34;2H[40m[96m│ [91mSetup[96m ││ [91mAnimate[96m ││ [91mColorize[96m ││ [92mGDI[96m │";
        cout << "[35;2H[40m[96m└───────┘└─────────┘└──────────┘└─────┘";
        Tab4::EnableAll();
        break;
    default:
        cout << "[1;1H[40m[91mERROR: Switching to an unknown tab: ";
        cout << (int)Tab;
        cout << "\n\nAllowed tab IDs: \n";
        cout << "1 (Setup)\n";
        cout << "2 (Animate)\n";
        cout << "3 (Colorize)\n";
        cout << "4 (GDI)\n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
}

void TabSwitcher::DisableTab(uint8_t Tab) {
    switch (Tab)
    {
    case 1:
        Tab1::DisableAll();
        break;
    case 2:
        Tab2::DisableAll();
        break;
    case 3:
        Tab3::DisableAll();
        break;
    case 4:
        Tab4::DisableAll();
        break;
    default:
        cout << "[1;1H[40m[91mERROR: Switching from an unknown tab: ";
        cout << (int)Tab;
        cout << "\n\nAllowed tab IDs: \n";
        cout << "1 (Setup)\n";
        cout << "2 (Animate)\n";
        cout << "3 (Colorize)\n";
        cout << "4 (GDI)\n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
}