#include <Tab2.h>
#include <ConsoleFillingTool.h>
#include <ConsoleTransitionTool.h>

size_t Tab2::SubViewBtn4::ItemsCount = 0;
wstring Tab2::SubViewBtn4::Code = L"";
TouchEventListener* Tab2::SubViewBtn4::Close = nullptr;
ConsoleDropdown Tab2::SubViewBtn4::AnimationType1 = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationType2 = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationTarget1 = {};
ConsoleDropdown Tab2::SubViewBtn4::AnimationTarget2 = {};
ConsoleDropdown Tab2::SubViewBtn4::TimingUnit = {};
ConsoleTextbox Tab2::SubViewBtn4::StartTime = {};
ConsoleTextbox Tab2::SubViewBtn4::EndTime = {};
ConsoleTextbox Tab2::SubViewBtn4::StartValue = {};
ConsoleTextbox Tab2::SubViewBtn4::EndValue = {};
ConsoleTextbox Tab2::SubViewBtn4::SliceInterval = {};
TouchEventListener* Tab2::SubViewBtn4::UseThreads = nullptr;
TouchEventListener* Tab2::SubViewBtn4::Generate = nullptr;
bool Tab2::SubViewBtn4::Thread = true;

void Tab2::SubViewBtn4::Open() {
    TabSwitcher::DisableAll();
    Tab2::DisableAll();
    InvalidateRect(GetConsoleWindow(), NULL, TRUE);
    // Draw the UI
    Tab2Graphics::Btn4Graphics::Main(3, 13);
    Close = TouchEventManager::Create();
    Close->X = 75;
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
        TouchEventManager::Delete(UseThreads);
        TouchEventManager::Delete(Generate);
        FillConsole(3, 11, 87, 19, ' ', 0x0F);
        Tab2Graphics::Btn4(3, 11, Normal);
        TabSwitcher::EnableAll();
        Tab2::EnableAll();
        LoopingTask = []() {};
    };
    Close->OnHover = []() {
        cout << "[14;76H[44m[93mx";
    };
    Close->OnLeave = []() {
        cout << "[14;76H[44m[91mx";
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
    AnimationTarget1.Create(24, 18, 21, 2);
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
    AnimationTarget1.AddItem(L"HideStatistics");
    AnimationTarget1.AddItem(L"RemoveOverlaps");
    AnimationTarget1.AddItem(L"LimitFPS");
    AnimationTarget1.AddItem(L"VelocityThreshold");
    AnimationTarget1.AddItem(L"Caption");
    AnimationTarget1.AddItem(L"DifficultyText");
    AnimationTarget1.SetSelection(0);
    AnimationTarget2.Create(24, 18, 21, 2);
    AnimationTarget2.AddItem(L"Volume");
    AnimationTarget2.AddItem(L"PlaybackSpeed");
    AnimationTarget2.AddItem(L"NoteSpeed");
    AnimationTarget2.AddItem(L"OffsetX");
    AnimationTarget2.AddItem(L"OffsetY");
    AnimationTarget2.AddItem(L"Zoom");
    AnimationTarget2.AddItem(L"StartKey");
    AnimationTarget2.AddItem(L"EndKey");
    AnimationTarget2.AddItem(L"VelocityThreshold");
    AnimationTarget2.SetSelection(0);
    TimingUnit.Create(60, 18, 13, 2);
    TimingUnit.AddItem(L"Microseconds");
    TimingUnit.AddItem(L"Ticks");
    TimingUnit.SetSelection(0);
    StartTime.Create(18, 21, 19, 2);
    StartTime.SetText(L"0");
    EndTime.Create(49, 21, 19, 2);
    EndTime.SetText(L"0");
    StartValue.Create(19, 24, 19, 2);
    StartValue.SetText(L"0");
    EndValue.Create(51, 24, 19, 2);
    EndValue.SetText(L"0");
    SliceInterval.Create(22, 27, 19, 2);
    SliceInterval.SetText(L"100");
    UseThreads = TouchEventManager::Create();
    UseThreads->X = 49;
    UseThreads->Y = 27;
    UseThreads->W = 0;
    UseThreads->H = 0;
    UseThreads->OnTouch = []() {
        Thread = !Thread;
        UseThreads->OnLeave();
        while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            
        }
    };
    UseThreads->OnHover = []() {
        Tab2Graphics::Btn4Graphics::UseThreads(49, 27, Hovered);
    };
    UseThreads->OnLeave = []() {
        if (Thread) {
            Tab2Graphics::Btn4Graphics::UseThreads(49, 27, Touched);
        }
        else {
            Tab2Graphics::Btn4Graphics::UseThreads(49, 27, Normal);
        }
    };
    Generate = TouchEventManager::Create();
    Generate->X = 65;
    Generate->Y = 27;
    Generate->W = 9;
    Generate->H = 0;
    Generate->OnTouch = []() {
        Tab2Graphics::Btn4Graphics::Generate(66, 27, Touched);
        wstring Code = L"";
        if (AnimationType2.GetSelectionText() == L"Sliced") {
            Code += L"Sliced";
        }
        Code += AnimationType1.GetSelectionText();
        Code += L"AnimationBy";
        if (TimingUnit.GetSelectionText() == L"Ticks") {
            Code += L"Tick";
        }
        else {
            Code += L"Time";
        }
        if (Thread == false) {
            Code += L"AndWait";
        }
        Code += L"(";
        if (AnimationType1.GetSelectionText() == L"Target") {
            Code += L"Get";
            Code += AnimationTarget1.GetSelectionText();
        }
        else {
            Code += StartValue.GetText();
        }
    };
    Generate->OnHover = []() {
        Tab2Graphics::Btn4Graphics::Generate(66, 27, Hovered);
    };
    Generate->OnLeave = []() {
        Tab2Graphics::Btn4Graphics::Generate(66, 27, Normal);
    };
    LoopingTask = []() {
        if (AnimationType1.GetSelection() == 0) {
            AnimationTarget1.Show();
            AnimationTarget2.Hide();
            EndTime.Disable();
            EndValue.Disable();
        }
        else {
            AnimationTarget1.Hide();
            AnimationTarget2.Show();
            EndTime.Enable();
            EndValue.Enable();
        }
        if (AnimationType1.GetSelection() == 2) {
            StartValue.Disable();
        }
        else {
            StartValue.Enable();
        }
        if (AnimationType2.GetSelection() == 0) {
            SliceInterval.Disable();
        }
        else {
            SliceInterval.Enable();
        }
    };
}