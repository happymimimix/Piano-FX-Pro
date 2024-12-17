#pragma once
#include <PFXSTUDIO_Global_Imports.h>

struct TouchEventListener {
    TouchEventListener* Prev = nullptr;
    TouchEventListener* Next = nullptr;
    uint8_t X = 0;
    uint8_t Y = 0;
    uint8_t W = 0;
    uint8_t H = 0;
    function<void()> OnHover = []() {};
    function<void()> OnLeave = []() {};
    function<void()> OnTouch = []() {};

    bool IsTouching(POINT MousePos) {
        if (X <= MousePos.x && MousePos.x <= (X + W) && Y <= MousePos.y && MousePos.y <= (Y + H)) {
            return true;
        }
        else {
            return false;
        }
    }
};

struct TouchEventManager {
    // A chain list that keep track of all instances of TouchEventListeners
    static TouchEventListener* First;
    static TouchEventListener* LastUsed;
    static TouchEventListener* Create() {
        TouchEventListener* NewListener = new TouchEventListener();
        if (First == nullptr) {
            // This is the first item
            First = NewListener;
        }
        else {
            NewListener->Next = First;
            First->Prev = NewListener;
            First = NewListener;
        }
        return NewListener;
    }
    static void Delete(TouchEventListener* &ListenerPointer) {
        if (ListenerPointer == nullptr) {
            cout << "[1;1H[40m[91mERROR: Trying to remove a touch event listener that doesn't exist! \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
        }
        else {
            if (ListenerPointer->Prev) {
                ListenerPointer->Prev->Next = ListenerPointer->Next;
            }
            else {
                First = ListenerPointer->Next;
            }
            if (ListenerPointer->Next) {
                ListenerPointer->Next->Prev = ListenerPointer->Prev;
            }
            ListenerPointer = nullptr;
        }
    }
    static void CheckTouches(POINT MousePos, bool MouseClicked) {
        TouchEventListener* CurrentListener = First;
        TouchEventListener* Touched = nullptr;
        while (CurrentListener != nullptr) {
            if (CurrentListener->IsTouching(MousePos)) {
                if (Touched == nullptr) {
                    Touched = CurrentListener;
                }
                else {
                    // Oh no! We're triggering more than one TouchEventListeners at the same time! 
                    cout << "[1;1H[40m[91mERROR: Touch event listeners cannot overlap! \n\n";
                    cout << "The TouchEventListener at (";
                    cout << (int)Touched->X;
                    cout << ", ";
                    cout << (int)Touched->Y;
                    cout << ") with size ";
                    cout << (int)Touched->W;
                    cout << " * ";
                    cout << (int)Touched->H;
                    cout << " is overlapping with another TouchEventListener at (";
                    cout << (int)CurrentListener->X;
                    cout << ", ";
                    cout << (int)CurrentListener->Y;
                    cout << ") with size ";
                    cout << (int)CurrentListener->W;
                    cout << " * ";
                    cout << (int)CurrentListener->H;
                    cout << "! \n";
                    cout << "\nThe program will now stop. \n";
                    while (true) {
                        // Make the program hang instead of closing! 
                        // This way the user can clearly see the error message. 
                    }
                }
            }
            CurrentListener = CurrentListener->Next;
        }
        if (Touched != nullptr) {
            // The mouse has touched something! 
            if (MouseClicked && LastUsed == Touched) {
                Touched->OnTouch();
            }
            else if (LastUsed != Touched) {
                Touched->OnHover();
            }
        }
        if (LastUsed != nullptr && LastUsed != Touched) {
            LastUsed->OnLeave();
        }
        LastUsed = Touched;
        CurrentListener = nullptr;
        Touched = nullptr;
    }
};