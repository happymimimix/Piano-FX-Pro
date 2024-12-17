#include <Tab1.h>
#include <Tab1Btn3.h>

TouchEventListener* Tab1::Btn1 = nullptr;
TouchEventListener* Tab1::Btn2 = nullptr;
TouchEventListener* Tab1::Btn3 = nullptr;
TouchEventListener* Tab1::Btn4 = nullptr;
TouchEventListener* Tab1::Btn5 = nullptr;

void Tab1::EnableAll() {
    if (Btn1 != nullptr | Btn2 != nullptr | Btn3 != nullptr | Btn4 != nullptr | Btn5 != nullptr) {
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
        Btn1->W = 35;
        Btn1->H = 2;
        Btn1->OnTouch = []() {
            cout << "[3;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[4;4H[44m[97m Download English translated Domino [30m█";
            cout << "[5;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            Sleep(500);
            ShellExecuteA(0, "open", "http://github.com/happymimimix/Domino-Customized/releases/download/000/DominoCustomized.MSI", 0, 0, SW_SHOWNORMAL);
            Btn1->OnLeave();
        };
        Btn1->OnHover = []() {
            cout << "[3;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[4;4H[103m[30m Download English translated Domino [90m█";
            cout << "[5;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn1->OnLeave = []() {
            cout << "[3;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[4;4H[107m[30m Download English translated Domino [90m█";
            cout << "[5;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn2 = TouchEventManager::Create();
        Btn2->X = 3;
        Btn2->Y = 5;
        Btn2->W = 33;
        Btn2->H = 2;
        Btn2->OnTouch = []() {
            cout << "[6;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[7;4H[44m[97m Download Fl Studio v12.3 archive [30m█";
            cout << "[8;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            Sleep(500);
            ShellExecuteA(0, "open", "http://github.com/happymimimix/Fl-Studio-12.3-Archive/releases/tag/12.3", 0, 0, SW_SHOWNORMAL);
            Btn2->OnLeave();
        };
        Btn2->OnHover = []() {
            cout << "[6;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[7;4H[103m[30m Download Fl Studio v12.3 archive [90m█";
            cout << "[8;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn2->OnLeave = []() {
            cout << "[6;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[7;4H[107m[30m Download Fl Studio v12.3 archive [90m█";
            cout << "[8;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn3 = TouchEventManager::Create();
        Btn3->X = 3;
        Btn3->Y = 8;
        Btn3->W = 48;
        Btn3->H = 2;
        Btn3->OnTouch = []() {
            cout << "[9;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[10;4H[44m[97m Install Black MIDI templates for FL Studio 12.3 [30m█";
            cout << "[11;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            Tab1::Btn3::Open();
        };
        Btn3->OnHover = []() {
            cout << "[9;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[10;4H[103m[30m Install Black MIDI templates for FL Studio 12.3 [90m█";
            cout << "[11;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn3->OnLeave = []() {
            cout << "[9;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[10;4H[107m[30m Install Black MIDI templates for FL Studio 12.3 [90m█";
            cout << "[11;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn4 = TouchEventManager::Create();
        Btn4->X = 3;
        Btn4->Y = 11;
        Btn4->W = 44;
        Btn4->H = 2;
        Btn4->OnTouch = []() {
            cout << "[12;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[13;4H[44m[97m Download Cheat Engine from official website [30m█";
            cout << "[14;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            Sleep(500);
            ShellExecuteA(0, "open", "http://www.cheatengine.org", 0, 0, SW_SHOWNORMAL);
            Btn4->OnLeave();
        };
        Btn4->OnHover = []() {
            cout << "[12;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[13;4H[103m[30m Download Cheat Engine from official website [90m█";
            cout << "[14;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn4->OnLeave = []() {
            cout << "[12;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[13;4H[107m[30m Download Cheat Engine from official website [90m█";
            cout << "[14;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn5 = TouchEventManager::Create();
        Btn5->X = 3;
        Btn5->Y = 14;
        Btn5->W = 36;
        Btn5->H = 2;
        Btn5->OnTouch = []() {
            cout << "[15;4H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[16;4H[44m[97m Improve audio quality with OmniMIDI [30m█";
            cout << "[17;4H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            Sleep(500);
            ShellExecuteA(0, "open", "https://github.com/KeppySoftware/OmniMIDI/releases", 0, 0, SW_SHOWNORMAL);
            Btn5->OnLeave();
        };
        Btn5->OnHover = []() {
            cout << "[15;4H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[16;4H[103m[30m Improve audio quality with OmniMIDI [90m█";
            cout << "[17;4H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Btn5->OnLeave = []() {
            cout << "[15;4H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀";
            cout << "[16;4H[107m[30m Improve audio quality with OmniMIDI [90m█";
            cout << "[17;4H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
        };
        Tab1::Draw();
    }
}

void Tab1::DisableAll() {
    if (Btn1 == nullptr | Btn2 == nullptr | Btn3 == nullptr | Btn4 == nullptr | Btn5 == nullptr) {
        cout << "[1;1H[40m[91mERROR: Trying to disable a tab that isn't enabled. \n";
        cout << "\nThe program will now stop. \n";
    }
    TouchEventManager::Delete(Btn1);
    TouchEventManager::Delete(Btn2);
    TouchEventManager::Delete(Btn3);
    TouchEventManager::Delete(Btn4);
    TouchEventManager::Delete(Btn5);
}

void Tab1::Draw() {
    if (Btn1 == nullptr | Btn2 == nullptr | Btn3 == nullptr | Btn4 == nullptr | Btn5 == nullptr) {
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