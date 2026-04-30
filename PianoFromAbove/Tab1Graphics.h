#pragma once
#include <PFXSTUDIO_Global_Imports.h>
#include <ButtonStateDeclarations.h>

struct Tab1Graphics {
    static void Btn1(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Install Cheat Engine 7.5.1 [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Install Cheat Engine 7.5.1 [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Install Cheat Engine 7.5.1 [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
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
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Install Black MIDI templates for FL Studio 12.3 [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Install Black MIDI templates for FL Studio 12.3 [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Install Black MIDI templates for FL Studio 12.3 [30m█"; y++;
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
    static void Btn3(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Standard MIDI Format Type 3 introduction [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Standard MIDI Format Type 3 introduction [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Standard MIDI Format Type 3 introduction [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
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
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Convert SMF 1 to SMF 3 for faster loading [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Convert SMF 1 to SMF 3 for faster loading [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Convert SMF 1 to SMF 3 for faster loading [30m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
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
    static void Btn5(uint8_t x, uint8_t y, BtnState state) {
        // 0-based -> 1-based
        x++;
        y++;
        switch (state) {
        case Normal:
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m Convert SMF 3 to SMF 1 for better compatibility [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[107m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Hovered:
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m Convert SMF 3 to SMF 1 for better compatibility [90m█"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[103m[30m▄[90m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█";
            break;
        case Touched:
            cout << "[" << (short)y << ';' << (short)x << "H[44m[30m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[97m Convert SMF 3 to SMF 1 for better compatibility [30m█"; y++;
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
    struct Btn2Graphics {
        static void Main(uint8_t x, uint8_t y) {
            // 0-based -> 1-based
            x++;
            y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m┌────────────────────────────────────────────────────────────[[91mx[36m]┐"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Fl Studio installation directory:                             [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m ┌──────────────────────────────────────────────────┬────────┐ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m │                                                  │ [91mBrowse[95m │ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m └──────────────────────────────────────────────────┴────────┘ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m┌─────────┐ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m│ [92mInstall[91m │ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m└─────────┘ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m└───────────────────────────────────────────────────────────────┘";
        }
        static void Browse(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[91mBrowse";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mBrowse";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[31mBrowse";
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
        static void Install(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[92mInstall";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mInstall";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[32mInstall";
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
    struct Btn4Graphics {
        static void Main(uint8_t x, uint8_t y) {
            // 0-based -> 1-based
            x++;
            y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m┌────────────────────────────────────────────────────────────[[91mx[36m]┐"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m Path to MIDI file:                                            [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m ┌──────────────────────────────────────────────────┬────────┐ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m │                                                  │ [91mBrowse[95m │ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m └──────────────────────────────────────────────────┴────────┘ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m┌─────────┐ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m│ [92mConvert[91m │ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m│[95m                                                   [91m└─────────┘ [36m│"; y++;
            cout << "[" << (short)y << ';' << (short)x << "H[44m[36m└───────────────────────────────────────────────────────────────┘";
        }
        static void Browse(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[91mBrowse";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mBrowse";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[31mBrowse";
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
        static void Convert(uint8_t x, uint8_t y, BtnState state) {
            // 0-based -> 1-based
            x++;
            y++;
            switch (state) {
            case Normal:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[92mConvert";
                break;
            case Hovered:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[93mConvert";
                break;
            case Touched:
                cout << "[" << (short)y << ';' << (short)x << "H[44m[32mConvert";
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