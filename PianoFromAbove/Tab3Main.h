#include <Tab3.h>

TouchEventListener* Tab3::Btn1 = nullptr;

void Tab3::EnableAll() {
    if (Btn1 != nullptr) {
        cout << "[1;1H[40m[91mERROR: Trying to enable a TouchEventListener that is already enabled. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        Btn1 = TouchEventManager::Create();
        Draw();
    }
}

void Tab3::DisableAll() {
    TouchEventManager::Delete(Tab3::Btn1);
}

void Tab3::Draw() {
    if (Btn1 == nullptr) {
        cout << "[1;1H[40m[91mERROR: Trying to render a button that doesn't exist. \n";
        cout << "\nThe program will now stop. \n";
        while (true) {
            // Make the program hang instead of closing! 
            // This way the user can clearly see the error message. 
        }
    }
    else {
        Btn1->OnLeave();
    }
}
