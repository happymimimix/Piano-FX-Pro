#include <Tab2.h>
#include <Tab2Btn4.h>
#include <ClipboardTool.h>
#include <LuaCode.h>

TouchEventListener* Tab2::Btn1 = nullptr;
TouchEventListener* Tab2::Btn2 = nullptr;
TouchEventListener* Tab2::Btn3 = nullptr;
TouchEventListener* Tab2::Btn4 = nullptr;

void Tab2::EnableAll() {
    if (AnyGoodPtr(Btn1,Btn2,Btn3,Btn4)) {
        cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        Btn1 = TouchEventManager::Create();
        Btn1->X = 3;
        Btn1->Y = 2;
        Btn1->W = 30;
        Btn1->H = 2;
        Btn1->OnTouch = []() {
            Tab2Graphics::Btn1(3, 2, Touched);
            Sleep(500);
            char ProgramPath[MAX_PATH + 1] = {};
            GetModuleFileNameA(NULL, ProgramPath, MAX_PATH);
            string Command = "start \"Visualize pitch bend tutorial\" \"";
            Command += ProgramPath;
            Command += "\" ";
            Command += "OPEN VisualizePitchBendTutorial";
            system(Command.c_str());
            Btn1->OnLeave();
        };
        Btn1->OnHover = []() {
            Tab2Graphics::Btn1(3, 2, Hovered);
        };
        Btn1->OnLeave = []() {
            Tab2Graphics::Btn1(3, 2, Normal);
        };
        Btn2 = TouchEventManager::Create();
        Btn2->X = 3;
        Btn2->Y = 5;
        Btn2->W = 36;
        Btn2->H = 2;
        Btn2->OnTouch = []() {
            Tab2Graphics::Btn2(3, 5, Touched);
            Sleep(500);
            char ProgramPath[MAX_PATH + 1] = {};
            GetModuleFileNameA(NULL, ProgramPath, MAX_PATH);
            string Command = "start \"Cheat engine Lua scripting tutorial\" \"";
            Command += ProgramPath;
            Command += "\" ";
            Command += "OPEN CheatEngineTutorial";
            system(Command.c_str());
            Btn2->OnLeave();
        };
        Btn2->OnHover = []() {
            Tab2Graphics::Btn2(3, 5, Hovered);
        };
        Btn2->OnLeave = []() {
            Tab2Graphics::Btn2(3, 5, Normal);
        };
        Btn3 = TouchEventManager::Create();
        Btn3->X = 3;
        Btn3->Y = 8;
        Btn3->W = 48;
        Btn3->H = 2;
        Btn3->OnTouch = []() {
            Tab2Graphics::Btn3(3, 8, Touched);
            Sleep(500);
            Copy(LuaCode);
            MessageBoxA(GetConsoleWindow(), "Cheat Engine lua code template has been copied to clipboard. \nOpen Cheat Engine and paste them into Table -> Show Cheat Table Lua Script. ", "Success", NULL);
            Btn3->OnLeave();
        };
        Btn3->OnHover = []() {
            Tab2Graphics::Btn3(3, 8, Hovered);
        };
        Btn3->OnLeave = []() {
            Tab2Graphics::Btn3(3, 8, Normal);
        };
        Btn4 = TouchEventManager::Create();
        Btn4->X = 3;
        Btn4->Y = 11;
        Btn4->W = 36;
        Btn4->H = 2;
        Btn4->OnTouch = []() {
            Tab2Graphics::Btn4(3, 11, Touched);
            Tab2::Btn4->OnLeave = []() {};
            Tab2::SubViewBtn4::Open();
        };
        Btn4->OnHover = []() {
            Tab2Graphics::Btn4(3, 11, Hovered);
        };
        Btn4->OnLeave = []() {
            Tab2Graphics::Btn4(3, 11, Normal);
        };
        Draw();
    }
}

void Tab2::DisableAll() {
    if (AnyNullPtr(Btn1, Btn2, Btn3, Btn4)) {
        cout << "[1;1H[40m[91mERROR: Trying to disable a tab that isn't enabled. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        TouchEventManager::Delete(Btn1);
        TouchEventManager::Delete(Btn2);
        TouchEventManager::Delete(Btn3);
        TouchEventManager::Delete(Btn4);
    }
}

void Tab2::Draw() {
    if (AnyNullPtr(Btn1, Btn2, Btn3, Btn4)) {
        cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        Btn1->OnLeave();
        Btn2->OnLeave();
        Btn3->OnLeave();
        Btn4->OnLeave();
    }
}