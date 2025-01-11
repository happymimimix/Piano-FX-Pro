#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <ButtonStateDeclarations.h>

struct Tab4Graphics {
    static void Btn1(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Visualize pitch bend tutorial [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Visualize pitch bend tutorial [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Visualize pitch bend tutorial [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        default:
            cout << "[1;1H[40m[91mERROR: Unknown button state ";
            cout << (int)state;
            cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
            break;
        }
    }
    static void Btn2(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Cheat engine Lua scripting tutorial [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Cheat engine Lua scripting tutorial [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Cheat engine Lua scripting tutorial [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        default:
            cout << "[1;1H[40m[91mERROR: Unknown button state ";
            cout << (int)state;
            cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
            break;
        }
    }
    static void Btn3(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Copy Lua animation script template to clipboard [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Copy Lua animation script template to clipboard [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Copy Lua animation script template to clipboard [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        default:
            cout << "[1;1H[40m[91mERROR: Unknown button state ";
            cout << (int)state;
            cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
            break;
        }
    }
    static void Btn4(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Generate animation code with a form [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Generate animation code with a form [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Generate animation code with a form [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        default:
            cout << "[1;1H[40m[91mERROR: Unknown button state ";
            cout << (int)state;
            cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
            cout << "\nThe program will now stop. \n";
            while (true) {
                // Make the program hang instead of closing! 
                // This way the user can clearly see the error message. 
            }
            break;
        }
    }
    struct Btn4Graphics {
        static void Main(uint8_t x, uint8_t y) {
            // 0-based -> 1-based
            x++;
            y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m┌──────────────────────────────────────────────────────────────────────────────────[[91mx[36m]┐"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                 ┌────────────────────┐ ┌────────────┐              ┌────────────┐   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Animation type: │                    │ │            │ Timing unit: │            │   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                 └────────────────────┘ └────────────┘              └────────────┘   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m             ┌──────────────────┐           ┌──────────────────┐                     [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Start time: │                  │ End time: │                  │                     [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m             └──────────────────┘           └──────────────────┘                     [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m              ┌──────────────────┐            ┌──────────────────┐                   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Start value: │                  │ End value: │                  │                   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m              └──────────────────┘            └──────────────────┘                   [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                   ┌────────────────────┐                                            [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Animation target: │                    │                                            [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                   └────────────────────┘                                            [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                 ┌──────────────────┐                   [91m┌─────────────┐ [91m┌──────────┐ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Slice interval: │                  │                   [91m│ [93mAdd to list[91m │ [91m│ [92mGenerate[91m │ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                 └──────────────────┘                   [91m└─────────────┘ [91m└──────────┘ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m└─────────────────────────────────────────────────────────────────────────────────────┘";
        }
        static void Add(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[91mAdd to list";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mAdd to list";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[31mAdd to list";
                break;
            default:
                cout << "[1;1H[40m[91mERROR: Unknown button state ";
                cout << (int)state;
                cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
                cout << "\nThe program will now stop. \n";
                while (true) {
                    // Make the program hang instead of closing! 
                    // This way the user can clearly see the error message. 
                }
                break;
            }
        }
        static void Generate(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[92mGenerate";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mGenerate";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[32mGenerate";
                break;
            default:
                cout << "[1;1H[40m[91mERROR: Unknown button state ";
                cout << (int)state;
                cout << ", should only be 1(normal), 2(hovered), or 3(touched). \n";
                cout << "\nThe program will now stop. \n";
                while (true) {
                    // Make the program hang instead of closing! 
                    // This way the user can clearly see the error message. 
                }
                break;
            }
        }
    };
};