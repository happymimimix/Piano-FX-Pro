#include <Tab2.h>
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
        //AnimationType1.Delete();
        //AnimationType2.Delete();
        //TimingUnit.Delete();
        //StartTime.Delete();
        //EndTime.Delete();
        //StartValue.Delete();
        //EndValue.Delete();
        //AnimationTarget1.Delete();
        //AnimationTarget2.Delete();
        //SliceInterval.Delete();
        //TouchEventManager::Delete(Add);
        //TouchEventManager::Delete(Generate);
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

}