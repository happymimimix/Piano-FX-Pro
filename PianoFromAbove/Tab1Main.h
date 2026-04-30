#include <Tab1.h>
#include <Tab1Btn2.h>
#include <Tab1Btn4.h>
#include <Tab1Btn5.h>

TouchEventListener* Tab1::Btn1 = nullptr;
TouchEventListener* Tab1::Btn2 = nullptr;
TouchEventListener* Tab1::Btn3 = nullptr;
TouchEventListener* Tab1::Btn4 = nullptr;
TouchEventListener* Tab1::Btn5 = nullptr;

void Tab1::EnableAll() {
    if (AnyGoodPtr(Btn1,Btn2,Btn3,Btn4,Btn5)) {
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
        Btn1->W = 27;
        Btn1->H = 2;
        Btn1->OnTouch = []() {
            Tab1Graphics::Btn1(3, 2, Touched);
            Sleep(500);
            Btn1->OnLeave();
        };
        Btn1->OnHover = []() {
            Tab1Graphics::Btn1(3, 2, Hovered);
        };
        Btn1->OnLeave = []() {
            Tab1Graphics::Btn1(3, 2, Normal);
        };
        Btn2 = TouchEventManager::Create();
        Btn2->X = 3;
        Btn2->Y = 5;
        Btn2->W = 48;
        Btn2->H = 2;
        Btn2->OnTouch = []() {
            Tab1Graphics::Btn2(3, 5, Touched);
            Tab1::Btn2->OnLeave = []() {};
            Tab1::SubViewBtn2::Open();
        };
        Btn2->OnHover = []() {
            Tab1Graphics::Btn2(3, 5, Hovered);
        };
        Btn2->OnLeave = []() {
            Tab1Graphics::Btn2(3, 5, Normal);
        };
        Btn3 = TouchEventManager::Create();
        Btn3->X = 3;
        Btn3->Y = 8;
        Btn3->W = 41;
        Btn3->H = 2;
        Btn3->OnTouch = []() {
            Tab1Graphics::Btn3(3, 8, Touched);
            Sleep(500);
            char ProgramPath[LONG_MAX_PATH] = {};
            GetModuleFileNameA(NULL, ProgramPath, LONG_MAX_PATH);
            string Command = "start \"Standard MIDI Format Type 3 Introduction\" \"";
            Command += ProgramPath;
            Command += "\" ";
            Command += "OPEN SMF3Intro";
            system(Command.c_str());
            Btn3->OnLeave();
        };
        Btn3->OnHover = []() {
            Tab1Graphics::Btn3(3, 8, Hovered);
        };
        Btn3->OnLeave = []() {
            Tab1Graphics::Btn3(3, 8, Normal);
        };
        Btn4 = TouchEventManager::Create();
        Btn4->X = 3;
        Btn4->Y = 11;
        Btn4->W = 42;
        Btn4->H = 2;
        Btn4->OnTouch = []() {
            Tab1Graphics::Btn4(3, 11, Touched);
            Tab1::Btn4->OnLeave = []() {};
            Tab1::SubViewBtn4::Open();
        };
        Btn4->OnHover = []() {
            Tab1Graphics::Btn4(3, 11, Hovered);
        };
        Btn4->OnLeave = []() {
            Tab1Graphics::Btn4(3, 11, Normal);
        };
        Btn5 = TouchEventManager::Create();
        Btn5->X = 3;
        Btn5->Y = 14;
        Btn5->W = 48;
        Btn5->H = 2;
        Btn5->OnTouch = []() {
            Tab1Graphics::Btn5(3, 14, Touched);
            Tab1::Btn5->OnLeave = []() {};
            Tab1::SubViewBtn5::Open();
        };
        Btn5->OnHover = []() {
            Tab1Graphics::Btn5(3, 14, Hovered);
        };
        Btn5->OnLeave = []() {
            Tab1Graphics::Btn5(3, 14, Normal);
        };
        Tab1::Draw();
    }
}

void Tab1::DisableAll() {
    if (AnyNullPtr(Btn1, Btn2, Btn3, Btn4, Btn5)) {
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
        TouchEventManager::Delete(Btn5);
    }
}

void Tab1::Draw() {
    if (AnyNullPtr(Btn1, Btn2, Btn3, Btn4, Btn5)) {
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
        Btn5->OnLeave();
    }
}