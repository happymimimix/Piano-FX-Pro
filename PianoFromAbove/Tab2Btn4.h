﻿#include <Tab2.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>

size_t Tab2::SubViewBtn4::ItemsCount = 0;
wstring Tab2::SubViewBtn4::Code = L"";
TouchEventListener* Tab2::SubViewBtn4::Close = nullptr;
ConsoleDropdown Tab2::SubViewBtn4::AnimationType1 = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationType2 = {};
ConsoleDropdown Tab2::SubViewBtn4::TimingUnit = {};
ConsoleTextbox Tab2::SubViewBtn4::StartTime = {};
ConsoleTextbox Tab2::SubViewBtn4::EndTime = {};
ConsoleTextbox Tab2::SubViewBtn4::StartValue = {};
ConsoleTextbox Tab2::SubViewBtn4::EndValue = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationTarget1 = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationTarget2 = {};
ConsoleTextbox Tab2::SubViewBtn4::SliceInterval = {};
TouchEventListener* Tab2::SubViewBtn4::Add = nullptr;
TouchEventListener* Tab2::SubViewBtn4::Generate = nullptr;

void Tab2::SubViewBtn4::Open() {
    TabSwitcher::DisableAll();
    Tab2::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    // Draw the UI
    Tab2Graphics::Btn4Graphics::Main(3, 13);
    cout << "";
    Close = TouchEventManager::Create();
    Close->X = 87;
    Close->Y = 13;
    Close->W = 0;
    Close->H = 0;
    Close->OnTouch = []() {
        Close->OnLeave = []() {};
        TouchEventManager::Delete(Close);
        AnimationType1.Delete();
        AnimationType2.Delete();
        TimingUnit.Delete();
        StartTime.Delete();
        EndTime.Delete();
        StartValue.Delete();
        EndValue.Delete();
        AnimationTarget1.Delete();
        AnimationTarget2.Delete();
        SliceInterval.Delete();
        TouchEventManager::Delete(Add);
        TouchEventManager::Delete(Generate);
        FillConsole(3, 11, 87, 19, ' ', 0x0F);
        Tab2Graphics::Btn4(3, 11, Normal);
        TabSwitcher::EnableAll();
        Tab2::EnableAll();
    };
    Close->OnHover = []() {
        cout << "[14;88H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[14;88H[44m[91mx";
    };
    AnimationType1.Create(22, 15, 21, 2);
    AnimationType1.AddItem(L"Instantaneous");
    AnimationType1.AddItem(L"Smooth");
    AnimationType1.AddItem(L"Target");
    AnimationType1.SetSelection(1);
    AnimationType2.Create(45, 15, 13, 2);
    AnimationType2.AddItem(L"Normal");
    AnimationType2.AddItem(L"Sliced");
    AnimationType2.SetSelection(0);
    AnimationTarget1.Create(24, 24, 21, 2);
    AnimationTarget1.AddItem(L"Microseconds");
    AnimationTarget1.AddItem(L"Volume");
    AnimationTarget1.AddItem(L"Mute");
    AnimationTarget1.AddItem(L"PlaybackSpeed");
    AnimationTarget1.AddItem(L"NoteSpeed");
    AnimationTarget1.AddItem(L"OffsetX");
    AnimationTarget1.AddItem(L"OffsetY");
    AnimationTarget1.AddItem(L"Zoom");
    AnimationTarget1.AddItem(L"SameWidth");
    AnimationTarget1.AddItem(L"StartKey");
    AnimationTarget1.AddItem(L"EndKey");
    AnimationTarget1.AddItem(L"KeyMode");
    AnimationTarget1.AddItem(L"Width");
    AnimationTarget1.AddItem(L"Height");
    AnimationTarget1.AddItem(L"Paused");
    AnimationTarget1.AddItem(L"Keyboard");
    AnimationTarget1.AddItem(L"VisualizePitchBends");
    AnimationTarget1.AddItem(L"PhigrosMode");
    AnimationTarget1.AddItem(L"ShowMarkers");
    AnimationTarget1.AddItem(L"TickBased");
    AnimationTarget1.AddItem(L"DisableUI");
    AnimationTarget1.AddItem(L"LimitFPS");
    AnimationTarget1.AddItem(L"Caption");
    AnimationTarget1.SetSelection(0);
    AnimationTarget1.Hide();
    AnimationTarget2.Create(24, 24, 21, 2);
    AnimationTarget2.AddItem(L"Microseconds");
    AnimationTarget2.AddItem(L"Volume");
    AnimationTarget2.AddItem(L"PlaybackSpeed");
    AnimationTarget2.AddItem(L"NoteSpeed");
    AnimationTarget2.AddItem(L"OffsetX");
    AnimationTarget2.AddItem(L"OffsetY");
    AnimationTarget2.AddItem(L"Zoom");
    AnimationTarget2.AddItem(L"StartKey");
    AnimationTarget2.AddItem(L"EndKey");
    AnimationTarget2.AddItem(L"Width");
    AnimationTarget2.AddItem(L"Height");
    AnimationTarget2.SetSelection(0);
}