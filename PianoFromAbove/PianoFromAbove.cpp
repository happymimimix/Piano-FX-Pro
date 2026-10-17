/*************************************************************************************************
*
* File: PianoFromAbove.cpp
*
* Description: Main entry point for Piano From Above.
*              Creates windows and enters the GUI and game loops
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <Windows.h>
#include <uxtheme.h>
#include <lzma.h>
#include <fcntl.h>
#include <io.h>
#ifdef INCLUDE_FFMPEG
// Do not include these files in Debug configuration as these files are too large and would cause significant lag in intellisense! 
#include <ffmpeg1.h>
#include <ffmpeg2.h>
#include <ffmpeg3.h>
#include <ffmpeg4.h>
#endif
#include <MainProcs.h>
#include <Config.h>
#include <GameState.h>
#include <Renderer.h>
#include <Misc.h>
#include <Studio.h>
#include <Tutorials.h>
#include <LuaCode.h>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpszCmdLine, INT nCmdShow);
DWORD WINAPI GameThread(LPVOID lpParameter);

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;
HWND g_hWndBar = NULL;
HWND g_hWndGfx = NULL;
bool g_bGfxDestroyed = false;
TSQueue<MSG> g_MsgQueue; // Producer/consumer to hold events for our game thread

wstring GetProcessName() {
    wchar_t buffer[LONG_MAX_PATH] = {};
    GetModuleFileNameW(NULL, buffer, LONG_MAX_PATH);
    wstring fullPath(buffer);
    size_t pos = fullPath.find_last_of(L"\\");
    return (pos != wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
}

wstring ProgramPath() {
    wchar_t szFilePath[LONG_MAX_PATH] = {};
    GetModuleFileNameW(NULL, szFilePath, LONG_MAX_PATH);
    (wcsrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

wstring MultiInstanceGuardInit() {
    wstring Code = L"";
    Code += L"if _G.Working then\n";
    Code += L"showMessage(\"Do not run multiple instances at once!\")\n";
    Code += L"return\n";
    Code += L"end\n";
    Code += L"_G.Working=true\n";
    return Code;
}

wstring MultiInstanceGuardRelease() {
    return L"_G.Working=false\n";
}

wstring FakeThreadingEngine() {
    wstring Code = L"";
    Code += L"Threads={}\n";
    Code += L"function Spawn(Func)\n";
    Code += L"local Thread=coroutine.create(Func)\n";
    Code += L"table.insert(Threads,Thread)\n";
    Code += L"coroutine.resume(Thread)\n";
    Code += L"end\n";
    Code += L"function Tick()\n";
    Code += L"if readShortInteger(CE_VideoOutput)~=0 then\n";
    Code += L"while Incomplete() and readShortInteger(CE_DoNextTick)==0 do\n";
    Code += L"processMessages()\n";
    Code += L"end\n";
    Code += L"writeShortInteger(CE_DoNextTick,0)\n";
    Code += L"end\n";
    Code += L"local ThreadIdx=1\n";
    Code += L"while ThreadIdx<=#Threads do\n";
    Code += L"if coroutine.status(Threads[ThreadIdx])==\"dead\" then\n";
    Code += L"table.remove(Threads,ThreadIdx)\n";
    Code += L"else\n";
    Code += L"local Status,Details=coroutine.resume(Threads[ThreadIdx])\n";
    Code += L"if not Status then error(debug.traceback(Threads[ThreadIdx],Details)) end\n";
    Code += L"ThreadIdx=ThreadIdx+1\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"processMessages()\n";
    Code += L"collectgarbage(\"collect\")\n";
    Code += L"if readShortInteger(CE_VideoOutput)~=0 then\n";
    Code += L"writeShortInteger(CE_Responded,1)\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"function Next()\n";
    Code += L"coroutine.yield()\n";
    Code += L"end\n";
    return Code;
}

wstring InitializeVariables() {
    wstring Code = L"";
    Code += L"MS=1e+3\n";
    Code += L"S=1e+6\n";
    Code += L"EXE=executeCodeLocalEx\n";
    Code += L"hPFX=EXE(QM(\"FindWindowA\"),\"PFX\",0)\n";
    Code += L"hGFX=EXE(QM(\"FindWindowExA\"),hPFX,0,\"PFXGFX\",0)\n";
    Code += L"hGDI=EXE(QM(\"FindWindowA\"),\"PFXGDI\",0)\n";
    Code += L"PFXdc=EXE(QM(\"GetDC\"),hGFX)\n";
    Code += L"GDIdc=EXE(QM(\"GetDC\"),hGDI)\n";
    Code += L"openProcess(getWindowProcessID(hPFX))\n";
    Code += L"SRCCOPY=0x00CC0020\n";
    Code += L"SRCPAINT=0x00EE0086\n";
    Code += L"SRCAND=0x008800C6\n";
    Code += L"SRCINVERT=0x00660046\n";
    Code += L"SRCERASE=0x00440328\n";
    Code += L"NOTSRCCOPY=0x00330008\n";
    Code += L"NOTSRCERASE=0x001100A6\n";
    Code += L"MERGECOPY=0x00C000CA\n";
    Code += L"MERGEPAINT=0x00BB0226\n";
    Code += L"PATCOPY=0x00F00021\n";
    Code += L"PATPAINT=0x00FB0A09\n";
    Code += L"PATINVERT=0x005A0049\n";
    Code += L"DSTINVERT=0x00550009\n";
    Code += L"BLACKNESS=0x00000042\n";
    Code += L"WHITENESS=0x00FF0062\n";
    Code += L"BS_SOLID=0\n";
    Code += L"BS_NULL=1\n";
    Code += L"BS_HOLLOW=BS_NULL\n";
    Code += L"BS_HATCHED=2\n";
    Code += L"BS_PATTERN=3\n";
    Code += L"BS_INDEXED=4\n";
    Code += L"BS_DIBPATTERN=5\n";
    Code += L"BS_DIBPATTERNPT=6\n";
    Code += L"BS_PATTERN8X8=7\n";
    Code += L"BS_DIBPATTERN8X8=8\n";
    Code += L"BS_MONOPATTERN=9\n";
    Code += L"HS_HORIZONTAL=0\n";
    Code += L"HS_VERTICAL=1\n";
    Code += L"HS_FDIAGONAL=2\n";
    Code += L"HS_BDIAGONAL=3\n";
    Code += L"HS_CROSS=4\n";
    Code += L"HS_DIAGCROSS=5\n";
    Code += L"HS_API_MAX=12\n";
    Code += L"PS_SOLID=0\n";
    Code += L"PS_DASH=1\n";
    Code += L"PS_DOT=2\n";
    Code += L"PS_DASHDOT=3\n";
    Code += L"PS_DASHDOTDOT=4\n";
    Code += L"PS_NULL=5\n";
    Code += L"PS_INSIDEFRAME=6\n";
    Code += L"PS_USERSTYLE=7\n";
    Code += L"PS_ALTERNATE=8\n";
    Code += L"PS_STYLE_MASK=0x0000000F\n";
    Code += L"PS_ENDCAP_ROUND=0x00000000\n";
    Code += L"PS_ENDCAP_SQUARE=0x00000100\n";
    Code += L"PS_ENDCAP_FLAT=0x00000200\n";
    Code += L"PS_ENDCAP_MASK=0x00000F00\n";
    Code += L"PS_JOIN_ROUND=0x00000000\n";
    Code += L"PS_JOIN_BEVEL=0x00001000\n";
    Code += L"PS_JOIN_MITER=0x00002000\n";
    Code += L"PS_JOIN_MASK=0x0000F000\n";
    Code += L"PS_COSMETIC=0x00000000\n";
    Code += L"PS_GEOMETRIC=0x00010000\n";
    Code += L"PS_TYPE_MASK=0x000F0000\n";
    Code += L"ALIGN_LEFT=0x0100\n";
    Code += L"ALIGN_CENTER=0x0200\n";
    Code += L"ALIGN_RIGHT=0x0300\n";
    Code += L"ALIGN_TOP=0x0001\n";
    Code += L"ALIGN_MIDDLE=0x0002\n";
    Code += L"ALIGN_BOTTOM=0x0003\n";
    Code += L"WHITE_BRUSH=0\n";
    Code += L"LTGRAY_BRUSH=1\n";
    Code += L"GRAY_BRUSH=2\n";
    Code += L"DKGRAY_BRUSH=3\n";
    Code += L"BLACK_BRUSH=4\n";
    Code += L"NULL_BRUSH=5\n";
    Code += L"HOLLOW_BRUSH=NULL_BRUSH\n";
    Code += L"WHITE_PEN=6\n";
    Code += L"BLACK_PEN=7\n";
    Code += L"NULL_PEN=8\n";
    return Code;
}

wstring DefineMemoryAddress(wstring VariableName, wstring Address, bool Pointer) {
    wstring Code = L"";
    Code += VariableName + L"=" + wstring(Pointer ? L"readPointer" : L"getAddress") + L"(\"" + GetProcessName() + L"+" + Address + L"\")\n";
    return Code;
}

wstring GettersAndSetters(wstring VariableName, wstring Type, bool IsReadOnly, wstring DebugList, bool IsSigned = false, bool NotePosUpdateRequired = false) {
    wstring Code = L"";
    Code += L"function Get" + VariableName + L"()\n";
    Code += L"return read" + Type + L"(" + VariableName + wstring(IsSigned ? L",true" : L"") + L")\n";
    Code += L"end\n";
    if (!IsReadOnly) {
        Code += L"function Set" + VariableName + L"(VAL)\n";
        Code += DebugList + L"Add(\"" + VariableName + L" -> \"..VAL)\n";
        Code += L"write" + Type + L"(" + VariableName + L",VAL)\n";
        if (NotePosUpdateRequired) Code += L"writeShortInteger(UpdateNotePos,1)\n";
        Code += L"end\n";
    }
    return Code;
}

wstring StringTypeGettersAndSetters(wstring VariableName, size_t Size, bool IsReadOnly, wstring DebugList, bool IsCaption = false) {
    wstring Code = L"";
    Code += L"function Get" + VariableName + L"()\n";
    Code += L"return readString(" + VariableName + L")\n";
    Code += L"end\n";
    if (!IsReadOnly) {
        Code += L"function Set" + VariableName + L"(VAL)\n";
        Code += DebugList + L"Add(\"" + VariableName + L" -> \\\"\"..VAL..\"\\\"\")\n";
        Code += L"if #VAL<" + to_wstring(Size) + L" then\n";
        Code += L"writeShortInteger(getAddress(" + VariableName + L")+#VAL,0)\n";
        Code += L"writeString(" + VariableName + L",VAL)\n";
        Code += L"end\n";
        Code += L"end\n";
    }
    if (IsCaption) { //CE Caption need a special alignment setting
        Code += L"function Set" + VariableName + L"Alignment(VAL)\n";
        Code += L"if VAL==ALIGN_LEFT then\n";
        Code += DebugList + L"Add(\"" + VariableName + L"Alignment -> \\\"L\\\"\")\n";
        Code += L"writeString(" + VariableName + L"Alignment,\"L\")\n";
        Code += L"end\n";
        Code += L"if VAL==ALIGN_CENTER then\n";
        Code += DebugList + L"Add(\"" + VariableName + L"Alignment -> \\\"C\\\"\")\n";
        Code += L"writeString(" + VariableName + L"Alignment,\"C\")\n";
        Code += L"end\n";
        Code += L"if VAL==ALIGN_RIGHT then\n";
        Code += DebugList + L"Add(\"" + VariableName + L"Alignment -> \\\"R\\\"\")\n";
        Code += L"writeString(" + VariableName + L"Alignment,\"R\")\n";
        Code += L"end\n";
        Code += L"end\n";
    }
    return Code;
}

wstring SetResolutionFunction() {
    wstring Code = L"";
    Code += L"function SetResolution(W,H)\n";
    Code += L"HistoryAdd(\"Resolution -> \"..W..\"*\"..H)\n";
    Code += L"local RectData=createMemoryStream()\n";
    Code += L"RectData.size=16\n";
    Code += L"RectData.Position=0\n";
    Code += L"EXE(QM(\"GetWindowRect\"),hPFX,RectData.Memory)\n";
    Code += L"RectData.Position=0\n";
    Code += L"local Px=RectData.readDword()\n";
    Code += L"local Py=RectData.readDword()\n";
    Code += L"local Pw=RectData.readDword()-Px\n";
    Code += L"local Ph=RectData.readDword()-Py\n";
    Code += L"RectData.Position=0\n";
    Code += L"EXE(QM(\"GetWindowRect\"),hGFX,RectData.Memory)\n";
    Code += L"RectData.Position=0\n";
    Code += L"local Gx=RectData.readDword()\n";
    Code += L"local Gy=RectData.readDword()\n";
    Code += L"local Gw=RectData.readDword()-Gx\n";
    Code += L"local Gh=RectData.readDword()-Gy\n";
    Code += L"local Mw=Pw-Gw\n";
    Code += L"local Mh=Ph-Gh\n";
    Code += L"RectData.destroy()\n";
    Code += L"RectData=nil\n";
    Code += L"EXE(QM(\"SetWindowPos\"),hPFX,-2,Px,Py,W+Mw,H+Mh,0)\n";
    Code += L"end\n";
    return Code;
}

wstring CloseDebugWindow(wstring Name) {
    wstring Code = L"";
    Code += L"Debug" + Name + L".destroy()\n";
    return Code;
}

wstring CleanUp() {
    wstring Code = L"";
    Code += L"while Incomplete() or ShaderWorking==true do Tick() end\n";
    Code += L"writeShortInteger(CE_Connected,0)\n";
    Code += L"EXE(QM(\"ReleaseDC\"),hGFX,PFXdc)\n";
    Code += L"EXE(QM(\"ReleaseDC\"),hGDI,GDIdc)\n";
    Code += L"if DebugPending~=nil then\n";
    Code += CloseDebugWindow(L"Pending");
    Code += L"end\n";
    Code += L"if DebugActive~=nil then\n";
    Code += CloseDebugWindow(L"Active");
    Code += L"end\n";
    Code += L"if DebugHistory~=nil then\n";
    Code += CloseDebugWindow(L"History");
    Code += L"end\n";
    Code += L"if DebugShader~=nil then\n";
    Code += CloseDebugWindow(L"Shader");
    Code += L"end\n";
    Code += L"collectgarbage(\"collect\")\n";
    Code += MultiInstanceGuardRelease();
    return Code;
}

wstring CreateDebugWindow(wstring Name, wstring X, wstring Y, wstring W, wstring H, bool Del = true, bool Add = true) {
    wstring Code = L"";
    Code += L"Debug" + Name + L"=createForm()\n";
    Code += L"Debug" + Name + L".Caption=\"Piano-FX Pro Animation Debug\"\n";
    Code += L"Debug" + Name + L".PopupMode=0\n";
    Code += L"Debug" + Name + L".BorderStyle=\"bsSizeable\"\n";
    Code += L"Debug" + Name + L".Left=" + X + L"\n";
    Code += L"Debug" + Name + L".Top=" + Y + L"\n";
    Code += L"Debug" + Name + L".Width=" + W + L"\n";
    Code += L"Debug" + Name + L".Height=" + H + L"\n";
    Code += L"Debug" + Name + L".OnClose=function()\n";
    Code += L"SetMicroseconds(GetSongLength())\n";
    Code += CleanUp();
    Code += L"return caFree\n";
    Code += L"end\n";
    Code += L"Debug" + Name + L".show()\n";
    Code += L"Debug" + Name + L"List=createListBox(Debug" + Name + L")\n";
    Code += L"Debug" + Name + L"List.Align=\"alClient\"\n";
    Code += L"Debug" + Name + L"List.Items.add(\"" + Name + L":\")\n";
    Code += L"Debug" + Name + L"List.Items.add(\"\")\n";
    if (Add) {
        Code += L"function " + Name + L"Add(name)\n";
        Code += L"if Debug" + Name + L"List.Items~=nil then Debug" + Name + L"List.Items.insert(2,name) end\n";
        Code += L"return name\n";
        Code += L"end\n";
    }
    if (Del) {
        Code += L"function " + Name + L"Del(name)\n";
        Code += L"if Debug" + Name + L"List.Items~=nil then Debug" + Name + L"List.Items.delete(Debug" + Name + L"List.Items.indexOf(name)) end\n";
        Code += L"return name\n";
        Code += L"end\n";
    }
    return Code;
}

wstring EasingFunctions() {
    wstring Code = L"";
    Code += L"function EaseLinear(x)\n";
    Code += L"return x\n";
    Code += L"end\n";
    Code += L"function EaseInSine(x)\n";
    Code += L"return 1-math.cos((x*math.pi)/2)\n";
    Code += L"end\n";
    Code += L"function EaseOutSine(x)\n";
    Code += L"return math.sin((x*math.pi)/2)\n";
    Code += L"end\n";
    Code += L"function EaseInOutSine(x)\n";
    Code += L"return -(math.cos(math.pi*x)-1)/2\n";
    Code += L"end\n";
    Code += L"function EaseInQuad(x)\n";
    Code += L"return x*x\n";
    Code += L"end\n";
    Code += L"function EaseOutQuad(x)\n";
    Code += L"return 1-(1-x)*(1-x)\n";
    Code += L"end\n";
    Code += L"function EaseInOutQuad(x)\n";
    Code += L"return x<0.5 and 2*x*x or 1-math.pow(-2*x+2,2)/2\n";
    Code += L"end\n";
    Code += L"function EaseInCubic(x)\n";
    Code += L"return x*x*x\n";
    Code += L"end\n";
    Code += L"function EaseOutCubic(x)\n";
    Code += L"return 1-math.pow(1-x,3)\n";
    Code += L"end\n";
    Code += L"function EaseInOutCubic(x)\n";
    Code += L"return x<0.5 and 4*x*x*x or 1-math.pow(-2*x+2,3)/2\n";
    Code += L"end\n";
    Code += L"function EaseInQuart(x)\n";
    Code += L"return x*x*x*x\n";
    Code += L"end\n";
    Code += L"function EaseOutQuart(x)\n";
    Code += L"return 1-math.pow(1-x,4)\n";
    Code += L"end\n";
    Code += L"function EaseInOutQuart(x)\n";
    Code += L"return x<0.5 and 8*x*x*x*x or 1-math.pow(-2*x+2,4)/2\n";
    Code += L"end\n";
    Code += L"function EaseInQuint(x)\n";
    Code += L"return x*x*x*x*x\n";
    Code += L"end\n";
    Code += L"function EaseOutQuint(x)\n";
    Code += L"return 1-math.pow(1-x,5)\n";
    Code += L"end\n";
    Code += L"function EaseInOutQuint(x)\n";
    Code += L"return x<0.5 and 16*x*x*x*x*x or 1-math.pow(-2*x+2,5)/2\n";
    Code += L"end\n";
    Code += L"function EaseInExpo(x)\n";
    Code += L"return x==0 and 0 or math.pow(2,10*x-10)\n";
    Code += L"end\n";
    Code += L"function EaseOutExpo(x)\n";
    Code += L"return x==1 and 1 or 1-math.pow(2,-10*x)\n";
    Code += L"end\n";
    Code += L"function EaseInOutExpo(x)\n";
    Code += L"return x==0 and 0 or x==1 and 1 or x<0.5 and math.pow(2,20*x-10)/2 or (2-math.pow(2,-20*x+10))/2\n";
    Code += L"end\n";
    Code += L"function EaseInCirc(x)\n";
    Code += L"return 1-math.sqrt(1-math.pow(x,2))\n";
    Code += L"end\n";
    Code += L"function EaseOutCirc(x)\n";
    Code += L"return math.sqrt(1-math.pow(x-1,2))\n";
    Code += L"end\n";
    Code += L"function EaseInOutCirc(x)\n";
    Code += L"return x<0.5 and (1-math.sqrt(1-math.pow(2*x,2)))/2 or (math.sqrt(1-math.pow(-2*x+2,2))+1)/2\n";
    Code += L"end\n";
    Code += L"function EaseInBack(x)\n";
    Code += L"local c1=1.70158\n";
    Code += L"local c3=c1+1\n";
    Code += L"return c3*x*x*x-c1*x*x\n";
    Code += L"end\n";
    Code += L"function EaseOutBack(x)\n";
    Code += L"local c1=1.70158\n";
    Code += L"local c3=c1+1\n";
    Code += L"return 1+c3*math.pow(x-1,3)+c1*math.pow(x-1,2)\n";
    Code += L"end\n";
    Code += L"function EaseInOutBack(x)\n";
    Code += L"local c1=1.70158\n";
    Code += L"local c2=c1*1.525\n";
    Code += L"return x<0.5 and (math.pow(2*x,2)*((c2+1)*2*x-c2))/2 or (math.pow(2*x-2,2)*((c2+1)*(x*2-2)+c2)+2)/2\n";
    Code += L"end\n";
    Code += L"function EaseInElastic(x)\n";
    Code += L"local c4=(2*math.pi)/3\n";
    Code += L"return x==0 and 0 or x==1 and 1 or -math.pow(2,10*x-10)*math.sin((x*10-10.75)*c4)\n";
    Code += L"end\n";
    Code += L"function EaseOutElastic(x)\n";
    Code += L"local c4=(2*math.pi)/3\n";
    Code += L"return x==0 and 0 or x==1 and 1 or math.pow(2,-10*x)*math.sin((x*10-0.75)*c4)+1\n";
    Code += L"end\n";
    Code += L"function EaseInOutElastic(x)\n";
    Code += L"local c5=(2*math.pi)/4.5\n";
    Code += L"return x==0 and 0 or x==1 and 1 or x<0.5 and -(math.pow(2,20*x-10)*math.sin((20*x-11.125)*c5))/2 or (math.pow(2,-20*x+10)*math.sin((20*x-11.125)*c5))/2+1\n";
    Code += L"end\n";
    Code += L"function EaseInBounce(x)\n";
    Code += L"return 1-EaseOutBounce(1-x)\n";
    Code += L"end\n";
    Code += L"function EaseOutBounce(x)\n";
    Code += L"local n1=7.5625\n";
    Code += L"local d1=2.75\n";
    Code += L"if x<1/d1 then\n";
    Code += L"return n1*x*x\n";
    Code += L"elseif x<2/d1 then\n";
    Code += L"return n1*(x-1.5/d1)*(x-1.5/d1)+0.75\n";
    Code += L"elseif x<2.5/d1 then\n";
    Code += L"return n1*(x-2.25/d1)*(x-2.25/d1)+0.9375\n";
    Code += L"else\n";
    Code += L"return n1*(x-2.625/d1)*(x-2.625/d1)+0.984375\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"function EaseInOutBounce(x)\n";
    Code += L"return x<0.5 and (1-EaseOutBounce(1-2*x))/2 or (1+EaseOutBounce(2*x-1))/2\n";
    Code += L"end\n";
    Code += L"function CenterX(Zoom)\n";
    Code += L"return (GetWidth()*(1-Zoom))/2\n";
    Code += L"end\n";
    Code += L"function CenterY(Zoom)\n";
    Code += L"return -(GetHeight()*(1-Zoom))/2\n";
    Code += L"end\n";
    Code += L"function MapValue(Value,SourceMin,SourceMax,DestMin,DestMax)\n";
    Code += L"return (Value-SourceMin)/(SourceMax-SourceMin)*(DestMax-DestMin)+DestMin\n";
    Code += L"end\n";
    Code += L"function MapValueWithEasing(Value,SourceMin,SourceMax,DestMin,DestMax,EasingFunction)\n";
    Code += L"return EasingFunction((Value-SourceMin)/(SourceMax-SourceMin))*(DestMax-DestMin)+DestMin\n";
    Code += L"end\n";
    Code += L"function var2str(var)\n";
    Code += L"for name,value in pairs(_G) do\n";
    Code += L"if value==var then\n";
    Code += L"return name\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"function Incomplete()\n";
    Code += L"return readShortInteger(PointersInitialized)~=0 and GetMicroseconds()<GetSongLength()\n";
    Code += L"end\n";
    return Code;
}

wstring WaitUntil(bool TickBased, wstring DebugList) {
    wstring Code = L"";
    Code += L"function WaitUntil" + wstring(TickBased ? L"Tick" : L"Time") + L"(TargetTime)\n";
    Code += L"local DbgItem=" + DebugList + L"Add(\"WaitUntil" + wstring(TickBased ? L"Tick" : L"Time") + L"(\"..TargetTime..\")\")\n";
    Code += L"while (Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()<TargetTime and Incomplete()) do Tick() end\n";
    Code += DebugList + L"Del(DbgItem)\n";
    Code += L"end\n";
    return Code;
}

wstring AnimationFunction(bool IsTarget, bool IsSliced, bool TickBased, bool Wait, wstring DebugList1, wstring DebugList2) {
    wstring Code = L"";
    Code += L"function " + wstring(IsSliced ? L"Sliced" : L"") + wstring(IsTarget ? L"Target" : L"Smooth") + L"AnimationBy" + wstring(TickBased ? L"Tick" : L"Time") + wstring(Wait ? L"AndWait" : L"") + L"(" + wstring(IsTarget ? L"GetValueFunction" : L"StartValue") + L",EndValue,StartTime,EndTime" + wstring(IsSliced ? L",Spacing" : L"") + L",EasingFunction,SetValueFunction)\n";
    Code += Wait ? L"" : L"Spawn(function()\n";
    Code += L"local DbgItem=" + DebugList1 + L"Add(\"" + wstring(IsSliced ? L"Sliced" : L"") + wstring(IsTarget ? L"Target" : L"Smooth") + L"AnimationBy" + wstring(TickBased ? L"Tick" : L"Time") + wstring(Wait ? L"AndWait" : L"") + L"(\".." + wstring(IsTarget ? L"var2str(GetValueFunction)" : L"StartValue") + L"..\",\"..EndValue..\",\"..StartTime..\",\"..EndTime.." + wstring(IsSliced ? L"\",\"..Spacing.." : L"") + L"\",\"..var2str(EasingFunction)..\",\"..var2str(SetValueFunction)..\")\")\n";
    Code += L"while (Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()<StartTime and Incomplete()) do " + wstring(Wait ? L"Tick()" : L"Next()") + L" end\n";
    Code += IsTarget ? L"local StartValue=GetValueFunction()\n" : L"";
    Code += IsSliced ? L"local SliceIndex=1\n" : L"";
    Code += DebugList1 + L"Del(DbgItem)\n";
    Code += L"DbgItem=" + DebugList2 + L"Add(" + wstring(IsTarget ? (L"\"" + wstring(IsSliced ? L"Sliced" : L"") + L"SmoothAnimationBy" + wstring(TickBased ? L"Tick" : L"Time") + wstring(Wait ? L"AndWait" : L"") + L"(\"..StartValue..\",\"..EndValue..\",\"..StartTime..\",\"..EndTime.." + wstring(IsSliced ? L"\",\"..Spacing.." : L"") + L"\",\"..var2str(EasingFunction)..\",\"..var2str(SetValueFunction)..\")\"") : L"DbgItem") + L")\n";
    Code += L"while (Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()<=EndTime and Incomplete()) do\n";
    Code += L"SetValueFunction(StartValue+EasingFunction((Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue))\n";
    Code += IsSliced ? L"while (Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()<StartTime+Spacing*SliceIndex and Incomplete()) do " + wstring(Wait ? L"Tick()" : L"Next()") + L" end\n" : L"";
    Code += IsSliced ? L"SliceIndex=SliceIndex+1\n" : L"";
    Code += IsSliced ? L"" : (Wait ? L"Tick()\n" : L"Next()\n");
    Code += L"end\n";
    Code += L"SetValueFunction(EndValue)\n";
    Code += DebugList2 + L"Del(DbgItem)\n";
    Code += Wait ? L"" : L"end)\n";
    Code += L"end\n";
    return Code;
}

wstring InstantaneousAnimationFunction(bool TickBased, bool Wait, wstring DebugList) {
    wstring Code = L"";
    Code += L"function InstantaneousAnimationBy" + wstring(TickBased ? L"Tick" : L"Time") + wstring(Wait ? L"AndWait" : L"") + L"(Value,Time,SetValueFunction)\n";
    Code += Wait ? L"" : L"Spawn(function()\n";
    Code += L"local DbgItem=" + DebugList + L"Add(\"InstantaneousAnimationBy" + wstring(TickBased ? L"Tick" : L"Time") + wstring(Wait ? L"AndWait" : L"") + L"(\"..Value..\",\"..Time..\",\"..var2str(SetValueFunction)..\")\")\n";
    Code += L"while (Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L"()<Time and Incomplete()) do " + wstring(Wait ? L"Tick()" : L"Next()") + L" end\n";
    Code += L"SetValueFunction(Value)\n";
    Code += DebugList + L"Del(DbgItem)\n";
    Code += Wait ? L"" : L"end)\n";
    Code += L"end\n";
    return Code;
}

wstring MethodsLookup() {
    wstring Code = L"";
    Code += L"AddressCache={}\n";
    Code += L"function QueryCachedNativeMethod(MethodName)\n";
    Code += L"if AddressCache[MethodName]==nil then\n";
    Code += L"AddressCache[MethodName]=getAddress(MethodName,true)\n";
    Code += L"end\n";
    Code += L"return AddressCache[MethodName]\n";
    Code += L"end\n";
    Code += L"QM=QueryCachedNativeMethod\n";
    return Code;
}

wstring RunShader(bool TickBased) {
    wstring Code = L"";
    Code += L"function RunShaderBy" + wstring(TickBased ? L"Tick" : L"Time") + L"(StartTime,EndTime,ShaderFunction,...)\n";
    Code += L"local EffectParameters=table.pack(...)\n";
    Code += L"local Concat=\"\"\n";
    Code += L"for Entry=1,EffectParameters.n do Concat=Concat..\",\"..tostring(EffectParameters[Entry])end\n";
    Code += L"if hGDI==0 or DebugShaderList.Items==nil then return end\n";
    Code += L"ShaderLookupCounter=ShaderLookupCounter+1\n";
    Code += L"ShaderLookupTable[ShaderLookupCounter]={Func=ShaderFunction,Param={TimingMethod=Get" + wstring(TickBased ? L"Ticks" : L"Microseconds") + L",StartTime=StartTime,EndTime=EndTime,EffectParameters=EffectParameters}}\n";
    Code += L"DebugShaderList.Items.add(\"RunShaderBy" + wstring(TickBased ? L"Tick" : L"Time") + L"(\"..StartTime..\",\"..EndTime..\",\"..var2str(ShaderFunction)..Concat..\") [ID:\"..ShaderLookupCounter..\"]\",ShaderLookupCounter)\n";
    Code += L"end\n";
    return Code;
}

wstring ShaderEngine() {
    wstring Code = L"";
    // Shader debug window
    Code += L"if hGDI~=0 then\n";
    Code += CreateDebugWindow(L"Shader", L"750", L"0", L"350", L"500", false, false);
    Code += L"end\n";
    Code += RunShader(false);
    Code += RunShader(true);
    // Shader thread
    Code += L"ShaderWorking=false\n";
    Code += L"function ShaderEngine()\n";
    Code += L"if hGDI==0 then return end\n";
    Code += L"if ShaderWorking==true then return end\n";
    Code += L"LastUpdate=os.time()*S+os.clock()*S\n";
    Code += L"ShaderLookupTable={}\n";
    Code += L"ShaderPersistentData={}\n";
    Code += L"ShaderLookupCounter=0\n";
    Code += L"ShaderWorking=true\n";
    Code += L"local EnginePersistentData=nil\n";
    Code += L"while Incomplete() and DebugShaderList.Items~=nil do\n";
    Code += L"local W=GetWidth()\n";
    Code += L"local H=GetHeight()\n";
    Code += L"if EnginePersistentData==nil then\n";
    Code += L"EnginePersistentData={}\n";
    Code += L"EnginePersistentData.W=W\n";
    Code += L"EnginePersistentData.H=H\n";
    Code += L"EnginePersistentData.MEMdc=EXE(QM(\"CreateCompatibleDC\"),GDIdc)\n";
    Code += L"EnginePersistentData.BMP=EXE(QM(\"CreateCompatibleBitmap\"),GDIdc,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),EnginePersistentData.MEMdc,EnginePersistentData.BMP)\n";
    Code += L"EnginePersistentData.Font=EXE(QM(\"CreateFontW\"),\n";
    Code += L"16,-- FontHeight\n";
    Code += L"0,-- FontWidth\n";
    Code += L"0,-- Escapement\n";
    Code += L"0,-- Orientation\n";
    Code += L"400,-- FontWeight\n";
    Code += L"0,-- Italic\n";
    Code += L"0,-- Underline\n";
    Code += L"0,-- StrikeOut\n";
    Code += L"1,-- CharSet\n";
    Code += L"0,-- OutPrecision\n";
    Code += L"0,-- ClipPrecision\n";
    Code += L"0,-- Quality\n";
    Code += L"0,-- PitchAndFamily\n";
    Code += L"\"Fixedsys\")-- FaceName\n";
    Code += L"end\n";
    Code += L"if EnginePersistentData.W~=W or EnginePersistentData.H~=H then\n";
    Code += L"EXE(QM(\"DeleteDC\"),EnginePersistentData.MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),EnginePersistentData.BMP)\n";
    Code += L"EnginePersistentData.W=W\n";
    Code += L"EnginePersistentData.H=H\n";
    Code += L"EnginePersistentData.MEMdc=EXE(QM(\"CreateCompatibleDC\"),GDIdc)\n";
    Code += L"EnginePersistentData.BMP=EXE(QM(\"CreateCompatibleBitmap\"),GDIdc,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),EnginePersistentData.MEMdc,EnginePersistentData.BMP)\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"BitBlt\"),EnginePersistentData.MEMdc,0,0,W,H,PFXdc,0,0,SRCCOPY)\n";
    Code += L"local ShaderID=2\n";
    Code += L"if DebugShaderList.Items.Count>2 then\n";
    Code += L"while ShaderID<DebugShaderList.Items.Count do\n";
    Code += L"local ShaderEntry=ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]\n";
    Code += L"while ShaderEntry.Param.TimingMethod()>ShaderEntry.Param.EndTime do\n";
    Code += L"ShaderEntry.Func(EnginePersistentData.MEMdc,true,1,ShaderEntry.Param.TimingMethod()-ShaderEntry.Param.StartTime,W,H,ShaderPersistentData[DebugShaderList.Items.getData(ShaderID)],table.unpack(ShaderEntry.Param.EffectParameters))\n";
    Code += L"ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]=nil\n";
    Code += L"ShaderPersistentData[DebugShaderList.Items.getData(ShaderID)]=nil\n";
    Code += L"DebugShaderList.Items.delete(ShaderID)\n";
    Code += L"if ShaderID>=DebugShaderList.Items.Count then goto Finish end\n";
    Code += L"ShaderEntry=ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]\n";
    Code += L"end\n";
    Code += L"if ShaderEntry.Param.TimingMethod()<ShaderEntry.Param.StartTime then goto Skip end\n";
    Code += L"if ShaderPersistentData[DebugShaderList.Items.getData(ShaderID)]==nil then ShaderPersistentData[DebugShaderList.Items.getData(ShaderID)]={} end\n";
    Code += L"ShaderEntry.Func(EnginePersistentData.MEMdc,false,(ShaderEntry.Param.TimingMethod()-ShaderEntry.Param.StartTime)/(ShaderEntry.Param.EndTime-ShaderEntry.Param.StartTime),ShaderEntry.Param.TimingMethod()-ShaderEntry.Param.StartTime,W,H,ShaderPersistentData[DebugShaderList.Items.getData(ShaderID)],table.unpack(ShaderEntry.Param.EffectParameters))\n";
    Code += L"::Skip::\n";
    Code += L"ShaderID=ShaderID+1\n";
    Code += L"end\n";
    Code += L"::Finish::\n";
    Code += L"end\n";
    // Show FPS
    Code += L"if DisplayShaderFPS then\n";
    Code += L"local FrameTime=(os.time()*S+os.clock()*S)-LastUpdate+1\n";
    Code += L"local FPS=math.ceil((S/FrameTime)*100.0)/100.0\n";
    Code += L"local Text=\"Shader FPS: \"..FPS\n";
    Code += L"EXE(QM(\"SelectObject\"),EnginePersistentData.MEMdc,EnginePersistentData.Font)\n";
    Code += L"EXE(QM(\"SetBkMode\"),EnginePersistentData.MEMdc,1)\n";
    Code += L"EXE(QM(\"SetTextColor\"),EnginePersistentData.MEMdc,0x000000)\n";
    Code += L"EXE(QM(\"TextOutA\"),EnginePersistentData.MEMdc,16,16,Text,#Text)\n";
    Code += L"EXE(QM(\"SetTextColor\"),EnginePersistentData.MEMdc,0xFFFFFF)\n";
    Code += L"EXE(QM(\"TextOutA\"),EnginePersistentData.MEMdc,15,15,Text,#Text)\n";
    Code += L"end\n";
    Code += L"LastUpdate=os.time()*S+os.clock()*S\n";
    // Present!
    Code += L"EXE(QM(\"BitBlt\"),GDIdc,0,0,W,H,EnginePersistentData.MEMdc,0,0,SRCCOPY)\n";
    Code += L"Next()\n";
    Code += L"end\n";
    Code += L"LastUpdate=nil\n";
    Code += L"EXE(QM(\"DeleteObject\"),EnginePersistentData.Font)\n";
    Code += L"EXE(QM(\"DeleteDC\"),EnginePersistentData.MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),EnginePersistentData.BMP)\n";
    Code += L"EnginePersistentData=nil\n";
    Code += L"ShaderLookupTable=nil\n";
    Code += L"ShaderPersistentData=nil\n";
    Code += L"ShaderLookupCounter=nil\n";
    Code += L"ShaderWorking=false\n";
    Code += L"collectgarbage(\"collect\")\n";
    Code += L"end\n";
    return Code;
}

struct EncodeColor {
    static wstring ColorR;
    static wstring ColorG;
    static wstring ColorB;
    static wstring ColorA;
    static wstring MakeColor() { return L"(" + ColorB + L"<<16)|(" + ColorG + L"<<8)|" + ColorR; }
    static wstring MakeAlpha() { return L"(" + ColorA + L"<<16)|0x00000000"; }
};
wstring EncodeColor::ColorR = L"R";
wstring EncodeColor::ColorG = L"G";
wstring EncodeColor::ColorB = L"B";
wstring EncodeColor::ColorA = L"Alpha";

wstring BlankShader(bool IsDark) {
    wstring Code = L"";
    Code += L"function " + wstring(IsDark ? L"Dark" : L"Bright") + L"BlankShader(DC,Time,Elapsed,W,H,PersistentData)\n";
    Code += L"EXE(QM(\"PatBlt\"),DC,0,0,W,H," + wstring(IsDark ? L"BLACKNESS" : L"WHITENESS") + L")\n";
    Code += L"end\n";
    return Code;
}

wstring WaveBitBlt(bool Vertical, wstring SRC, bool Persistent) {
    wstring ShiftCode = L"math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount))";
    return L"EXE(QM(\"BitBlt\")," + wstring(Persistent ? L"PersistentData.PersiDC" : L"DC") + L"," + wstring(Vertical ? L"Line" : ShiftCode) + L"," + wstring(Vertical ? ShiftCode : L"Line") + L"," + wstring(Vertical ? L"MaxWaveWidth" : L"W") + L"," + wstring(Vertical ? L"H" : L"MaxWaveHeight") + L",DC," + wstring(Vertical ? L"Line,0" : L"0,Line") + L"," + SRC + L")\n";
}

wstring WaveShader(wstring prefix, bool Vertical, bool Glitch) {
    wstring Code = L"";
    Code += L"function " + prefix + L"Shader(DC,Final,Time,Elapsed,W,H,PersistentData,MaxWave" + wstring(Vertical ? L"Width" : L"Height") + L",MinWave" + wstring(Vertical ? L"Width" : L"Height") + L",ShiftingAmount)\n";
    Code += L"if MaxWave" + wstring(Vertical ? L"Width" : L"Height") + L"==nil then MaxWave" + wstring(Vertical ? L"Width" : L"Height") + L"=" + wstring(Vertical ? L"100" : L"10") + L" end\n";
    Code += L"if MinWave" + wstring(Vertical ? L"Width" : L"Height") + L"==nil then MinWave" + wstring(Vertical ? L"Width" : L"Height") + L"=" + wstring(Vertical ? L"20" : L"5") + L" end\n";
    Code += L"if ShiftingAmount==nil then ShiftingAmount=10 end\n";
    Code += Glitch ? L"if PersistentData.PersiDC==nil then\n" : L"";
    Code += Glitch ? L"PersistentData.PersiDC=EXE(QM(\"CreateCompatibleDC\"),DC)\n" : L"";
    Code += Glitch ? L"PersistentData.PersiBMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n" : L"";
    Code += Glitch ? L"PersistentData.PersiW=W\n" : L"";
    Code += Glitch ? L"PersistentData.PersiH=H\n" : L"";
    Code += Glitch ? L"EXE(QM(\"BitBlt\"),PersistentData.PersiDC,0,0,W,H,DC,0,0,SRCCOPY)\n" : L"";
    Code += Glitch ? L"end\n" : L"";
    Code += Glitch ? L"if PersistentData.PersiW~=W or PersistentData.PersiH~=H then\n" : L"";
    Code += Glitch ? L"EXE(QM(\"DeleteDC\"),PersistentData.PersiDC)\n" : L"";
    Code += Glitch ? L"EXE(QM(\"DeleteObject\"),PersistentData.PersiBMP)\n" : L"";
    Code += Glitch ? L"PersistentData.PersiDC=EXE(QM(\"CreateCompatibleDC\"),DC)\n" : L"";
    Code += Glitch ? L"PersistentData.PersiBMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n" : L"";
    Code += Glitch ? L"PersistentData.PersiW=W\n" : L"";
    Code += Glitch ? L"PersistentData.PersiH=H\n" : L"";
    Code += Glitch ? L"EXE(QM(\"BitBlt\"),PersistentData.PersiDC,0,0,W,H,DC,0,0,SRCCOPY)\n" : L"";
    Code += Glitch ? L"end\n" : L"";
    Code += L"local Line=0\n";
    Code += L"while Line<" + wstring(Vertical ? L"W" : L"H") + L" do\n";
    Code += Glitch ? L"if math.random(0,1)==0 then\n" : L"";
    Code += Glitch ? L"" : WaveBitBlt(Vertical, L"SRCCOPY", false);
    Code += Glitch ? WaveBitBlt(Vertical, L"SRCPAINT", true) : L"";
    Code += Glitch ? L"else\n" : L"";
    Code += Glitch ? WaveBitBlt(Vertical, L"SRCAND", true) : L"";
    Code += Glitch ? L"end\n" : L"";
    Code += L"Line=Line+math.random(MinWave" + wstring(Vertical ? L"Width" : L"Height") + L",MaxWave" + wstring(Vertical ? L"Width" : L"Height") + L")\n";
    Code += L"end\n";
    Code += Glitch ? L"EXE(QM(\"BitBlt\"),DC,0,0,W,H,PersistentData.PersiDC,0,0,SRCCOPY)\n" : L"";
    Code += Glitch ? L"if Final then\n" : L"";
    Code += Glitch ? L"EXE(QM(\"DeleteDC\"),PersistentData.PersiDC)\n" : L"";
    Code += Glitch ? L"EXE(QM(\"DeleteObject\"),PersistentData.PersiBMP)\n" : L"";
    Code += Glitch ? L"end\n" : L"";
    Code += L"end\n";
    return Code;
}

wstring ColorBurnShader() {
    wstring Code = L"";
    Code += L"function ColorBurnShader(DC,Final,Time,Elapsed,W,H,PersistentData,R,G,B)\n";
    Code += L"if R==nil then R=0 end\n";
    Code += L"if G==nil then G=255 end\n";
    Code += L"if B==nil then B=0 end\n";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    Code += L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),Color)\n";
    Code += L"EXE(QM(\"SelectObject\"),DC,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),DC,0,0,W,H,PATINVERT)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"end\n";
    return Code;
}

wstring RainbowBurnShader(bool Vertical, bool Animated, bool Reversed) {
    wstring Code = L"";
    Code += L"function " + wstring(Reversed ? L"Reversed" : L"") + wstring(Animated ? L"Animated" : L"") + wstring(Vertical ? L"Vertical" : L"") + L"RainbowBurnShader(DC,Final,Time,Elapsed,W,H,PersistentData,Initial_R,Initial_G,Initial_B,Final_R,Final_G,Final_B" + wstring(Animated ? L",Iteration" : L"") + L",Bar" + wstring(Vertical ? L"Width" : L"Height") + L")\n";
    Code += L"if Initial_R==nil then Initial_R=0 end\n";
    Code += L"if Initial_G==nil then Initial_G=255 end\n";
    Code += L"if Initial_B==nil then Initial_B=0 end\n";
    Code += L"if Final_R==nil then Final_R=0 end\n";
    Code += L"if Final_G==nil then Final_G=0 end\n";
    Code += L"if Final_B==nil then Final_B=255 end\n";
    Code += Animated ? L"if Iteration==nil then Iteration=10 end\n" : L"";
    Code += L"if Bar" + wstring(Vertical ? L"Width" : L"Height") + L"==nil then Bar" + wstring(Vertical ? L"Width" : L"Height") + L"=2 end\n";
    Code += L"local Line=0\n";
    Code += L"while Line<" + wstring(Vertical ? L"W" : L"H") + L" do\n";
    Code += Animated ? L"local OffsetedLine=math.abs((Line+" + wstring(Reversed ? L"(1.0-Time)" : L"Time") + L"*Iteration*" + wstring(Vertical ? L"W" : L"H") + L")%(2*" + wstring(Vertical ? L"W" : L"H") + L")-" + wstring(Vertical ? L"W" : L"H") + L")\n" : L"";
    Code += L"local R=math.ceil((" + wstring(Animated ? L"OffsetedLine" : L"Line") + L"/" + wstring(Vertical ? L"W" : L"H") + L")*(Final_R-Initial_R)+Initial_R)\n";
    Code += L"local G=math.ceil((" + wstring(Animated ? L"OffsetedLine" : L"Line") + L"/" + wstring(Vertical ? L"W" : L"H") + L")*(Final_G-Initial_G)+Initial_G)\n";
    Code += L"local B=math.ceil((" + wstring(Animated ? L"OffsetedLine" : L"Line") + L"/" + wstring(Vertical ? L"W" : L"H") + L")*(Final_B-Initial_B)+Initial_B)\n";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    Code += L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),Color)\n";
    Code += L"EXE(QM(\"SelectObject\"),DC,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),DC," + wstring(Vertical ? L"Line,0" : L"0,Line") + L"," + wstring(Vertical ? L"BarWidth,H" : L"W,BarHeight") + L",PATINVERT)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"Line=Line+Bar" + wstring(Vertical ? L"Width" : L"Height") + L"\n";
    Code += L"end\n";
    Code += L"end\n";
    return Code;
}

wstring ColorOffset(bool WithWave) {
    wstring Code = L"";
    Code += L"function ColorOffset" + wstring(WithWave ? L"Wave" : L"") + L"Shader(DC,Final,Time,Elapsed,W,H,PersistentData,OffsettingAmount" + wstring(WithWave ? L",MaxWaveHeight,MinWaveHeight,ShiftingAmount" : L"") + L")\n";
    Code += L"if OffsettingAmount==nil then OffsettingAmount=5 end\n";
    Code += WithWave ? L"if MaxWaveHeight==nil then MaxWaveHeight=40 end\n" : L"";
    Code += WithWave ? L"if MinWaveHeight==nil then MinWaveHeight=10 end\n" : L"";
    Code += WithWave ? L"if ShiftingAmount==nil then ShiftingAmount=10 end\n" : L"";
    Code += L"local MEMdc_R=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_R=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_G=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_G=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_B=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_B=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_Main=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_Main=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_R,BMP_R)\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),0x0000FF)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_R,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_R,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_R,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_G,BMP_G)\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),0x00FF00)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_G,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_G,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_G,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_B,BMP_B)\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),0xFF0000)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_B,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_B,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_B,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_Main,BMP_Main)\n";
    wstring ShiftCode = L"math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))";
    Code += WithWave ? L"local Line=0\n" : L"";
    Code += WithWave ? L"local OffsetRX=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"local OffsetRY=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"local OffsetGX=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"local OffsetGY=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"local OffsetBX=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"local OffsetBY=" + ShiftCode + L"\n" : L"";
    Code += WithWave ? L"while Line<H do\n" : L"";
    ShiftCode = WithWave ? L"math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount))" : ShiftCode;
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main," + ShiftCode + L"," + wstring(WithWave ? L"Line" : ShiftCode) + L",W," + wstring(WithWave ? L"MaxWaveHeight" : L"H") + L",MEMdc_R," + wstring(WithWave ? L"OffsetRX,Line+OffsetRY" : L"0,0") + L",SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main," + ShiftCode + L"," + wstring(WithWave ? L"Line" : ShiftCode) + L",W," + wstring(WithWave ? L"MaxWaveHeight" : L"H") + L",MEMdc_G," + wstring(WithWave ? L"OffsetGX,Line+OffsetGY" : L"0,0") + L",SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main," + ShiftCode + L"," + wstring(WithWave ? L"Line" : ShiftCode) + L",W," + wstring(WithWave ? L"MaxWaveHeight" : L"H") + L",MEMdc_B," + wstring(WithWave ? L"OffsetBX,Line+OffsetBY" : L"0,0") + L",SRCPAINT)\n";
    Code += WithWave ? L"Line=Line+math.random(MinWaveHeight,MaxWaveHeight)\n" : L"";
    Code += WithWave ? L"end\n" : L"";
    Code += L"EXE(QM(\"BitBlt\"),DC,0,0,W,H,MEMdc_Main,0,0,SRCCOPY)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_R)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_R)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_G)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_G)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_B)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_B)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_Main)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_Main)\n";
    Code += L"end\n";
    return Code;
}

wstring FadeShader(bool IsDark, bool IsReversed) {
    wstring Code = L"";
    Code += L"function " + wstring(IsReversed ? L"Reversed" : L"") + wstring(IsDark ? L"Dark" : L"Bright") + L"FadeShader(DC,Final,Time,Elapsed,W,H,PersistentData,MaxAlpha,MinAlpha)\n";
    Code += L"if MaxAlpha==nil then MaxAlpha=255.0 end\n";
    Code += L"if MinAlpha==nil then MinAlpha=0.0 end\n";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc,0,0,W,H," + wstring(IsDark ? L"BLACKNESS" : L"WHITENESS") + L")\n";
    Code += L"local Alpha=math.floor((MaxAlpha-MinAlpha)*" + wstring(IsReversed ? L"(1.0-Time)" : L"Time") + L"+MinAlpha)\n";
    EncodeColor::ColorA = L"Alpha";
    Code += L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"end\n";
    return Code;
}

wstring HatchBrushShader() {
    wstring Code = L"";
    Code += L"function HatchBrushShader(DC,Final,Time,Elapsed,W,H,PersistentData,R,G,B,BkR,BkG,BkB)\n";
    Code += L"if R==nil then R=0 end\n";
    Code += L"if G==nil then G=255 end\n";
    Code += L"if B==nil then B=0 end\n";
    Code += L"if BkR==nil then BkR=255 end\n";
    Code += L"if BkG==nil then BkG=0 end\n";
    Code += L"if BkB==nil then BkB=0 end\n";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    Code += L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += L"local Brush=EXE(QM(\"CreateHatchBrush\"),math.random(0,5),Color)\n";
    Code += L"EXE(QM(\"SelectObject\"),DC,Brush)\n";
    EncodeColor::ColorR = L"BkR";
    EncodeColor::ColorG = L"BkG";
    EncodeColor::ColorB = L"BkB";
    Code += L"local BkColor=" + EncodeColor::MakeColor() + L"\n";
    Code += L"EXE(QM(\"SetBKColor\"),DC,BkColor)\n";
    Code += L"EXE(QM(\"PatBlt\"),DC,0,0,W,H,PATINVERT)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"end\n";
    return Code;
}

wstring TextShader(bool Type, bool IsReversed) {
    wstring Code = L"";
    Code += L"function " + wstring(Type ? (IsReversed ? L"Delete" : L"Type") : L"") + L"TextShader(DC,Final,Time,Elapsed,W,H,PersistentData,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += L"if Text==nil then Text=\"Hello World\" end\n";
    Code += L"if FontHeight==nil then FontHeight=50 end\n";
    Code += L"if FontWidth==nil then FontWidth=30 end\n";
    Code += L"if FontWeight==nil then FontWeight=400 end\n";
    Code += L"if FaceName==nil then FaceName=\"Fixedsys\" end\n";
    Code += L"if Alignment==nil then Alignment=ALIGN_LEFT|ALIGN_TOP end\n";
    Code += L"if Italic==nil then Italic=0 end\n";
    Code += L"if Underline==nil then Underline=0 end\n";
    Code += L"if StrikeOut==nil then StrikeOut=0 end\n";
    Code += L"if PosX==nil then PosX=0 end\n";
    Code += L"if PosY==nil then PosY=0 end\n";
    Code += L"if NoBackground==nil then NoBackground=false end\n";
    Code += L"if TextR==nil then TextR=255 end\n";
    Code += L"if TextG==nil then TextG=255 end\n";
    Code += L"if TextB==nil then TextB=255 end\n";
    Code += L"if BkR==nil then BkR=255 end\n";
    Code += L"if BkG==nil then BkG=0 end\n";
    Code += L"if BkB==nil then BkB=0 end\n";
    EncodeColor::ColorR = L"TextR";
    EncodeColor::ColorG = L"TextG";
    EncodeColor::ColorB = L"TextB";
    Code += L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += L"EXE(QM(\"SetTextColor\"),DC,Color)\n";
    Code += L"if NoBackground==true then\n";
    Code += L"EXE(QM(\"SetBkMode\"),DC,1)\n";
    Code += L"end\n";
    Code += L"if NoBackground==false then\n";
    Code += L"EXE(QM(\"SetBkMode\"),DC,2)\n";
    Code += L"end\n";
    EncodeColor::ColorR = L"BkR";
    EncodeColor::ColorG = L"BkG";
    EncodeColor::ColorB = L"BkB";
    Code += L"local BkColor=" + EncodeColor::MakeColor() + L"\n";
    Code += L"EXE(QM(\"SetBKColor\"),DC,BkColor)\n";
    Code += L"local Font=EXE(QM(\"CreateFontW\"),\n";
    Code += L"FontHeight,-- FontHeight\n";
    Code += L"FontWidth,-- FontWidth\n";
    Code += L"0,-- Escapement\n";
    Code += L"0,-- Orientation\n";
    Code += L"FontWeight,-- FontWeight\n";
    Code += L"Italic,-- Italic\n";
    Code += L"Underline,-- Underline\n";
    Code += L"StrikeOut,-- StrikeOut\n";
    Code += L"1,-- CharSet\n";
    Code += L"0,-- OutPrecision\n";
    Code += L"0,-- ClipPrecision\n";
    Code += L"0,-- Quality\n";
    Code += L"0,-- PitchAndFamily\n";
    Code += L"{type=4,value=FaceName})-- FaceName\n";
    Code += L"EXE(QM(\"SelectObject\"),DC,Font)\n";
    Code += L"local TextLen=#wideStringToByteTable(Text)//2\n";
    Code += L"local MEM=createMemoryStream()\n";
    Code += L"MEM.Position=0\n";
    Code += L"MEM.size=8\n";
    Code += L"EXE(QM(\"GetTextExtentPoint32W\"),DC,{type=4,value=Text}," + wstring(Type ? (IsReversed ? L"math.ceil(TextLen*(1.0-Time))" : L"math.ceil(TextLen*Time)") : L"TextLen") + L",MEM.Memory)\n";
    Code += L"MEM.Position=0\n";
    Code += L"local TextW=MEM.readDword()\n";
    Code += L"local TextH=MEM.readDword()\n";
    Code += L"MEM.destroy()\n";
    Code += L"local TextX=0\n";
    Code += L"local TextY=0\n";
    wstring Left = L"PosX";
    wstring Center = L"PosX-TextW/2";
    wstring Right = L"PosX-TextW";
    wstring Top = L"PosY";
    wstring Middle = L"PosY-TextH/2";
    wstring Bottom = L"PosY-TextH";
    Code += L"if Alignment==0x0101 then\n";
    Code += L"TextX=math.floor(" + Left + L")\n";
    Code += L"TextY=math.floor(" + Top + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0102 then\n";
    Code += L"TextX=math.floor(" + Left + L")\n";
    Code += L"TextY=math.floor(" + Middle + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0103 then\n";
    Code += L"TextX=math.floor(" + Left + L")\n";
    Code += L"TextY=math.floor(" + Bottom + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0201 then\n";
    Code += L"TextX=math.floor(" + Center + L")\n";
    Code += L"TextY=math.floor(" + Top + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0202 then\n";
    Code += L"TextX=math.floor(" + Center + L")\n";
    Code += L"TextY=math.floor(" + Middle + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0203 then\n";
    Code += L"TextX=math.floor(" + Center + L")\n";
    Code += L"TextY=math.floor(" + Bottom + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0301 then\n";
    Code += L"TextX=math.floor(" + Right + L")\n";
    Code += L"TextY=math.floor(" + Top + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0302 then\n";
    Code += L"TextX=math.floor(" + Right + L")\n";
    Code += L"TextY=math.floor(" + Middle + L")\n";
    Code += L"end\n";
    Code += L"if Alignment==0x0303 then\n";
    Code += L"TextX=math.floor(" + Right + L")\n";
    Code += L"TextY=math.floor(" + Bottom + L")\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"TextOutW\"),DC,TextX,TextY,{type=4,value=Text}," + wstring(Type ? (IsReversed ? L"math.ceil(TextLen*(1.0-Time))" : L"math.ceil(TextLen*Time)") : L"TextLen") + L")\n";
    Code += L"EXE(QM(\"DeleteObject\"),Font)\n";
    Code += L"end\n";
    return Code;
}

wstring RotatePointTool() {
    wstring Code = L"";
    Code += L"function RotatePoint(X,Y,R,CX,CY)\n";
    Code += L"return CX+(X-CX)*math.cos(R)-(Y-CY)*math.sin(R),CY+(X-CX)*math.sin(R)+(Y-CY)*math.cos(R)\n";
    Code += L"end\n";
    return Code;
}

wstring RadialBlurShader(bool IsBlur, bool IsPaint) {
    wstring Code = L"";
    Code += L"function Radial" + wstring(IsBlur ? L"Blur" : (IsPaint ? L"Paint" : L"And")) + L"Shader(DC,Final,Time,Elapsed,W,H,PersistentData,MaxRotation,Precision" + wstring(IsBlur ? L",BlendAlpha" : L"") + L",Iteration)\n";
    Code += L"if MaxRotation==nil then MaxRotation=0.05 end\n";
    Code += L"if Precision==nil then Precision=4 end\n";
    Code += IsBlur ? L"if BlendAlpha==nil then BlendAlpha=(1<<7)-1 end\n" : L"";
    Code += L"if Iteration==nil then Iteration=5 end\n";
    Code += L"local MEM=createMemoryStream()\n";
    Code += L"MEM.size=24\n";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"for Current=1,Iteration do\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    Code += L"local Rotation=math.random(-math.abs(MaxRotation*math.pow(10.0,Precision)),math.abs(MaxRotation*math.pow(10.0,Precision)))/math.pow(10.0,Precision)\n";
    Code += L"local TopLeftX,TopLeftY=RotatePoint(0,0,Rotation,W/2,H/2)\n";
    Code += L"local TopRightX,TopRightY=RotatePoint(W,0,Rotation,W/2,H/2)\n";
    Code += L"local BottomLeftX,BottomLeftY=RotatePoint(0,H,Rotation,W/2,H/2)\n";
    Code += L"MEM.Position=0\n";
    Code += L"MEM.writeDword(math.floor(TopLeftX))\n";
    Code += L"MEM.writeDword(math.floor(TopLeftY))\n";
    Code += L"MEM.writeDword(math.floor(TopRightX))\n";
    Code += L"MEM.writeDword(math.floor(TopRightY))\n";
    Code += L"MEM.writeDword(math.floor(BottomLeftX))\n";
    Code += L"MEM.writeDword(math.floor(BottomLeftY))\n";
    Code += L"MEM.Position=0\n";
    Code += L"EXE(QM(\"PlgBlt\"),MEMdc,MEM.Memory,DC,0,0,W,H,0,0,0)\n";
    EncodeColor::ColorA = L"BlendAlpha";
    Code += IsBlur ? L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n" : L"";
    Code += IsBlur ? L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n" : L"";
    Code += IsBlur ? L"" : L"EXE(QM(\"BitBlt\"),DC,0,0,W,H,MEMdc,0,0," + wstring(IsPaint ? L"SRCPAINT" : L"SRCAND") + L")\n";
    Code += L"end\n";
    Code += L"MEM.destroy()\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"end\n";
    return Code;
}

wstring PolygonShader(bool IsColored, bool IsRandom) {
    wstring Code = L"";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    Code += L"function " + wstring(IsColored ? (IsRandom ? L"RandomColor" : L"Color") : L"Invert") + L"PolygonShader(DC,Final,Time,Elapsed,W,H,PersistentData,MaxSides,MinSides,MaxRadius,MinRadius,MaxCount,MinCount,MinRotationSpeed,MaxRotationSpeed,SpeedMultiplier,LineThickness,Delay" + wstring(IsColored ? (IsRandom ? L"" : L",R,G,B") : L"") + L")\n";
    Code += L"if MaxSides==nil then MaxSides=10 end\n";
    Code += L"if MinSides==nil then MinSides=3 end\n";
    Code += L"if MaxRadius==nil then MaxRadius=400 end\n";
    Code += L"if MinRadius==nil then MinRadius=100 end\n";
    Code += L"if MaxCount==nil then MaxCount=5 end\n";
    Code += L"if MinCount==nil then MinCount=1 end\n";
    Code += L"if MinRotationSpeed==nil then MinRotationSpeed=-0.05 end\n";
    Code += L"if MaxRotationSpeed==nil then MaxRotationSpeed=0.05 end\n";
    Code += L"if SpeedMultiplier==nil then SpeedMultiplier=1000 end\n";
    Code += L"if LineThickness==nil then LineThickness=50 end\n";
    Code += L"if Delay==nil then Delay=0.25*S end\n";
    Code += IsColored ? (IsRandom ? L"" : L"if R==nil then R=0 end\n") : L"";
    Code += IsColored ? (IsRandom ? L"" : L"if G==nil then G=255 end\n") : L"";
    Code += IsColored ? (IsRandom ? L"" : L"if B==nil then B=0 end\n") : L"";
    Code += L"if PersistentData.Shapes==nil then PersistentData.Shapes={} end\n";
    Code += IsColored ? (IsRandom ? L"if PersistentData.Colors==nil then PersistentData.Colors={} end\n" : L"") : L"";
    Code += IsColored ? (IsRandom ? L"if PersistentData.ShapesGenerated==nil then PersistentData.ShapesGenerated=false end\n" : L"") : L"";
    Code += L"if PersistentData.LastUpdate==nil then PersistentData.LastUpdate=0 end\n";
    Code += L"local Count=math.random(MinCount,MaxCount)\n";
    Code += L"if PersistentData.ShapesGenerated==true then Count=PersistentData.Count else PersistentData.Count=Count end\n";
    Code += L"local Polygons={}\n";
    Code += L"local MEM=createMemoryStream()\n";
    Code += L"for Shape=1,Count do\n";
    Code += L"local Sides=math.random(MinSides,MaxSides)\n";
    Code += L"local Radius=math.random(MinRadius,MaxRadius)\n";
    Code += L"local X=math.random(0,W)\n";
    Code += L"local Y=math.random(0,H)\n";
    Code += L"local Rotation=math.random()*(2*math.pi)\n";
    Code += L"local Speed=math.random(MinRotationSpeed*SpeedMultiplier,MaxRotationSpeed*SpeedMultiplier)/SpeedMultiplier\n";
    Code += L"local AngleStep=2*math.pi/Sides\n";
    Code += IsColored ? (IsRandom ? L"local R=math.random(0,255)\n" : L"") : L"";
    Code += IsColored ? (IsRandom ? L"local G=math.random(0,255)\n" : L"") : L"";
    Code += IsColored ? (IsRandom ? L"local B=math.random(0,255)\n" : L"") : L"";
    Code += L"if PersistentData.ShapesGenerated==true then\n";
    Code += L"Sides=PersistentData.Shapes[Shape].Sides\n";
    Code += L"Radius=PersistentData.Shapes[Shape].Radius\n";
    Code += L"X=PersistentData.Shapes[Shape].X\n";
    Code += L"Y=PersistentData.Shapes[Shape].Y\n";
    Code += L"PersistentData.Shapes[Shape].Rotation=PersistentData.Shapes[Shape].Rotation+PersistentData.Shapes[Shape].Speed\n";
    Code += L"Rotation=PersistentData.Shapes[Shape].Rotation\n";
    Code += L"AngleStep=PersistentData.Shapes[Shape].AngleStep\n";
    Code += L"else\n";
    Code += L"table.insert(PersistentData.Shapes,{Sides=Sides,Radius=Radius,X=X,Y=Y,Rotation=Rotation,Speed=Speed,AngleStep=AngleStep})\n";
    Code += L"end\n";
    Code += L"while Radius>0 do\n";
    Code += L"MEM.Position=0\n";
    Code += L"MEM.size=0\n";
    Code += L"for ThisSide=0,Sides-1 do\n";
    Code += L"MEM.size=MEM.size+8\n";
    Code += L"MEM.writeDword(X+math.floor(Radius*math.cos(ThisSide*AngleStep+Rotation)))\n";
    Code += L"MEM.writeDword(Y+math.floor(Radius*math.sin(ThisSide*AngleStep+Rotation)))\n";
    Code += L"end\n";
    Code += L"MEM.Position=0\n";
    Code += L"table.insert(Polygons,EXE(QM(\"CreatePolygonRgn\"),MEM.Memory,Sides,2))\n";
    Code += IsColored ? (IsRandom ? L"if PersistentData.ShapesGenerated==false then table.insert(PersistentData.Colors," + EncodeColor::MakeColor() + L") end\n" : L"") : L"";
    Code += L"Radius=Radius-LineThickness\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"PersistentData.ShapesGenerated=true\n";
    Code += L"MEM.destroy()\n";
    Code += IsColored ? (IsRandom ? L"" : L"local Color=" + EncodeColor::MakeColor() + L"\n") : L"";
    Code += IsColored ? (IsRandom ? L"" : L"local Brush=EXE(QM(\"CreateSolidBrush\"),Color)\n") : L"";
    Code += L"for " + wstring(IsRandom ? L"PolygonID" : L"_") + L",ThisPolygon in ipairs(Polygons) do\n";
    Code += IsColored ? (IsRandom ? L"local Brush=EXE(QM(\"CreateSolidBrush\"),PersistentData.Colors[PolygonID])\n" : L"") : L"";
    Code += IsColored ? L"EXE(QM(\"FrameRgn\"),DC,ThisPolygon,Brush,math.ceil(LineThickness/4),math.ceil(LineThickness/4))\n" : L"EXE(QM(\"InvertRgn\"),DC,ThisPolygon)\n";
    Code += IsColored ? (IsRandom ? L"EXE(QM(\"DeleteObject\"),Brush)\n" : L"") : L"";
    Code += L"EXE(QM(\"DeleteObject\"),ThisPolygon)\n";
    Code += L"end\n";
    Code += IsColored ? (IsRandom ? L"" : L"EXE(QM(\"DeleteObject\"),Brush)\n") : L"";
    Code += L"if Elapsed-PersistentData.LastUpdate>=Delay then\n";
    Code += L"PersistentData.Shapes={}\n";
    Code += IsColored ? (IsRandom ? L"PersistentData.Colors={}\n" : L"") : L"";
    Code += L"PersistentData.LastUpdate=Elapsed\n";
    Code += L"PersistentData.ShapesGenerated=false\n";
    Code += L"end\n";
    Code += L"end\n";
    return Code;
}

wstring TriangleFrameShader(bool IsRandom) {
    wstring Code = L"";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    EncodeColor::ColorA = L"Transparency";
    Code += L"function " + wstring(IsRandom ? L"RandomColor" : L"") + L"TriangleFrameShader(DC,Final,Time,Elapsed,W,H,PersistentData,Size,Speed,Transparency" + wstring(IsRandom ? L"" : L",R,G,B") + L")\n";
    Code += L"if Size==nil then Size=200 end\n";
    Code += L"if Speed==nil then Speed=5 end\n";
    Code += L"if Transparency==nil then Transparency=(1<<7)+(1<<6)-1 end\n";
    Code += IsRandom ? L"" : L"if R==nil then R=0 end\n";
    Code += IsRandom ? L"" : L"if G==nil then G=127 end\n";
    Code += IsRandom ? L"" : L"if B==nil then B=255 end\n";
    Code += L"if PersistentData.Offset==nil then PersistentData.Offset=1 end\n";
    Code += IsRandom ? L"if PersistentData.Colors==nil then PersistentData.Colors={} end\n" : L"";
    Code += IsRandom ? L"if PersistentData.ColorsGenerated==nil then PersistentData.ColorsGenerated=false end\n" : L"";
    Code += IsRandom ? L"if PersistentData.ColorOffset==nil then PersistentData.ColorOffset=2 end\n" : L"";
    Code += L"while PersistentData.Offset>0 do\n";
    Code += L"PersistentData.Offset=PersistentData.Offset-Size\n";
    Code += IsRandom ? L"PersistentData.ColorOffset=PersistentData.ColorOffset-2\n" : L"";
    Code += IsRandom ? L"while PersistentData.ColorOffset<0 do\n" : L"";
    Code += IsRandom ? L"PersistentData.ColorOffset=PersistentData.ColorOffset+#PersistentData.Colors\n" : L"";
    Code += IsRandom ? L"end\n" : L"";
    Code += L"end\n";
    Code += L"local CurrentOffset=PersistentData.Offset\n";
    Code += L"local Polygons={}\n";
    Code += L"local MEM=createMemoryStream()\n";
    Code += L"MEM.size=24\n";
    Code += L"while CurrentOffset<=W+Size do\n";
    Code += L"local X1,Y1=CurrentOffset,Size\n";
    Code += L"local X2,Y2=RotatePoint(X1,Y1,2*math.pi/3,CurrentOffset,0)\n";
    Code += L"local X3,Y3=RotatePoint(X2,Y2,2*math.pi/3,CurrentOffset,0)\n";
    Code += L"MEM.Position=0\n";
    Code += L"MEM.writeDword(math.floor(X1))\n";
    Code += L"MEM.writeDword(math.floor(Y1))\n";
    Code += L"MEM.writeDword(math.floor(X2))\n";
    Code += L"MEM.writeDword(math.floor(Y2))\n";
    Code += L"MEM.writeDword(math.floor(X3))\n";
    Code += L"MEM.writeDword(math.floor(Y3))\n";
    Code += L"MEM.Position=0\n";
    Code += L"table.insert(Polygons,EXE(QM(\"CreatePolygonRgn\"),MEM.Memory,3,2))\n";
    Code += IsRandom ? L"if PersistentData.ColorsGenerated==false then\n" : L"";
    Code += IsRandom ? L"local R=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"local G=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"local B=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"table.insert(PersistentData.Colors," + EncodeColor::MakeColor() + L")\n" : L"";
    Code += IsRandom ? L"end\n" : L"";
    Code += L"X1,Y1=RotatePoint(X1,Y1,math.pi,W/2,H/2)\n";
    Code += L"X2,Y2=RotatePoint(X2,Y2,math.pi,W/2,H/2)\n";
    Code += L"X3,Y3=RotatePoint(X3,Y3,math.pi,W/2,H/2)\n";
    Code += L"MEM.Position=0\n";
    Code += L"MEM.writeDword(math.floor(X1))\n";
    Code += L"MEM.writeDword(math.floor(Y1))\n";
    Code += L"MEM.writeDword(math.floor(X2))\n";
    Code += L"MEM.writeDword(math.floor(Y2))\n";
    Code += L"MEM.writeDword(math.floor(X3))\n";
    Code += L"MEM.writeDword(math.floor(Y3))\n";
    Code += L"MEM.Position=0\n";
    Code += L"table.insert(Polygons,EXE(QM(\"CreatePolygonRgn\"),MEM.Memory,3,2))\n";
    Code += IsRandom ? L"if PersistentData.ColorsGenerated==false then\n" : L"";
    Code += IsRandom ? L"local R=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"local G=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"local B=math.random(0,255)\n" : L"";
    Code += IsRandom ? L"table.insert(PersistentData.Colors," + EncodeColor::MakeColor() + L")\n" : L"";
    Code += IsRandom ? L"end\n" : L"";
    Code += L"CurrentOffset=CurrentOffset+Size\n";
    Code += L"end\n";
    Code += IsRandom ? L"PersistentData.ColorsGenerated=true\n" : L"";
    Code += L"MEM.destroy()\n";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc,0,0,W,H,DC,0,0,SRCCOPY)\n";
    Code += IsRandom ? L"" : L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += IsRandom ? L"" : L"local Brush=EXE(QM(\"CreateSolidBrush\"),Color)\n";
    Code += IsRandom ? L"" : L"EXE(QM(\"SelectObject\"),MEMdc,Brush)\n";
    Code += L"for " + wstring(IsRandom ? L"PolygonID" : L"_") + L",ThisPolygon in ipairs(Polygons) do\n";
    Code += IsRandom ? L"local Brush=EXE(QM(\"CreateSolidBrush\"),PersistentData.Colors[(PolygonID+PersistentData.ColorOffset-1)%#PersistentData.Colors+1])\n" : L"";
    Code += IsRandom ? L"EXE(QM(\"SelectObject\"),MEMdc,Brush)\n" : L"";
    Code += L"EXE(QM(\"FillRgn\"),MEMdc,ThisPolygon,Brush)\n";
    Code += IsRandom ? L"EXE(QM(\"DeleteObject\"),Brush)\n" : L"";
    Code += L"EXE(QM(\"DeleteObject\"),ThisPolygon)\n";
    Code += L"end\n";
    Code += IsRandom ? L"" : L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"PersistentData.Offset=PersistentData.Offset+Speed\n";
    Code += L"end\n";
    return Code;
}

wstring CRTShader() {
    wstring Code = L"";
    Code += L"function CRTShader(DC,Final,Time,Elapsed,W,H,PersistentData,ColorOffsetAmount,ColorOffsetTransparency,ScanLineTransparency,ScanLineHeight,ScanLineSpacing)\n";
    Code += L"if ColorOffsetAmount==nil then ColorOffsetAmount=2 end\n";
    Code += L"if ColorOffsetTransparency==nil then ColorOffsetTransparency=(1<<7)-1 end\n";
    Code += L"if ScanLineTransparency==nil then ScanLineTransparency=(1<<6)-1 end\n";
    Code += L"if ScanLineHeight==nil then ScanLineHeight=2 end\n";
    Code += L"if ScanLineSpacing==nil then ScanLineSpacing=2 end\n";
    Code += L"local MEMdc_R=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_R=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_G=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_G=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_B=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_B=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"local MEMdc_Main=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP_Main=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"if ColorOffsetTransparency>0 and ColorOffsetAmount~=0 then\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_R,BMP_R)\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),0x0000FF)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_R,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_R,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_R,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_G,BMP_G)\n";
    Code += L"Brush=EXE(QM(\"CreateSolidBrush\"),0x00FF00)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_G,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_G,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_G,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_B,BMP_B)\n";
    Code += L"Brush=EXE(QM(\"CreateSolidBrush\"),0xFF0000)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_B,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_B,0,0,W,H,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_B,0,0,W,H,DC,0,0,SRCAND)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc_Main,BMP_Main)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,ColorOffsetAmount,0,W,H,MEMdc_R,0,0,SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,-ColorOffsetAmount,0,W,H,MEMdc_G,0,0,SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,0,ColorOffsetAmount,W,H,MEMdc_B,0,0,SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,0,-ColorOffsetAmount,W,H,MEMdc_R,0,0,SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,ColorOffsetAmount,-ColorOffsetAmount,W,H,MEMdc_G,0,0,SRCPAINT)\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,-ColorOffsetAmount,ColorOffsetAmount,W,H,MEMdc_B,0,0,SRCPAINT)\n";
    EncodeColor::ColorA = L"ColorOffsetTransparency";
    Code += L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc_Main,0,0,W,H,BlendFunc)\n";
    Code += L"end\n";
    Code += L"if ScanLineTransparency>0 and ScanLineHeight~=0 then\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_Main,0,0,W,H,WHITENESS)\n";
    Code += L"local Offset=math.random(0,ScanLineSpacing+ScanLineHeight-1)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc_Main,0,Offset,W,ScanLineHeight,BLACKNESS)\n";
    Code += L"local Line=ScanLineSpacing+ScanLineHeight\n";
    Code += L"while Line<H do\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,0,Line+Offset,W,Line,MEMdc_Main,0,Offset,SRCCOPY)\n";
    Code += L"Line=Line*2\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc_Main,0,0,W,H,DC,0,0,SRCAND)\n";
    EncodeColor::ColorA = L"ScanLineTransparency";
    Code += L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc_Main,0,0,W,H,BlendFunc)\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_R)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_R)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_G)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_G)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_B)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_B)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc_Main)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP_Main)\n";
    Code += L"end\n";
    return Code;
}

wstring NoiseShader() {
    wstring Code = L"";
    Code += L"function NoiseShader(DC,Final,Time,Elapsed,W,H,PersistentData,MaxNoiseSize,MinNoiseSize,MaxNoiseAmount,MinNoiseAmount)\n";
    Code += L"if MaxNoiseSize==nil then MaxNoiseSize=3 end\n";
    Code += L"if MinNoiseSize==nil then MinNoiseSize=1 end\n";
    Code += L"if MaxNoiseAmount==nil then MaxNoiseAmount=(1<<9)-1 end\n";
    Code += L"if MinNoiseAmount==nil then MinNoiseAmount=(1<<7)-1 end\n";
    Code += L"local NoiseAmount=math.random(MinNoiseAmount,MaxNoiseAmount)\n";
    Code += L"for NoisePixel=1,NoiseAmount do\n";
    Code += L"local NoiseSize=math.random(MinNoiseSize,MaxNoiseSize)\n";
    Code += L"EXE(QM(\"PatBlt\"),DC,math.random(-NoiseSize+1,W),math.random(-NoiseSize+1,H),NoiseSize,NoiseSize,PATINVERT)\n";
    Code += L"end\n";
    Code += L"end\n";
    return Code;
}

wstring FrameShader(bool IsAnimated) {
    wstring Code = L"";
    Code += L"function " + wstring(IsAnimated ? L"Animated" : L"") + L"FrameShader(DC,Final,Time,Elapsed,W,H,PersistentData,FrameSize,Resolution" + wstring(IsAnimated ? L",Initial_R,Initial_G,Initial_B,Initial_A,Final_R,Final_G,Final_B,Final_A,Iteration" : L",R,G,B,A") + L")\n";
    Code += L"if FrameSize==nil then FrameSize=(1<<7)-1 end\n";
    Code += L"if Resolution==nil then Resolution=2 end\n";
    Code += IsAnimated ? L"if Initial_R==nil then Initial_R=255 end\n" : L"if R==nil then R=255 end\n";
    Code += IsAnimated ? L"if Initial_G==nil then Initial_G=0 end\n" : L"if G==nil then G=0 end\n";
    Code += IsAnimated ? L"if Initial_B==nil then Initial_B=0 end\n" : L"if B==nil then B=0 end\n";
    Code += IsAnimated ? L"if Initial_A==nil then Initial_A=255 end\n" : L"if A==nil then A=255 end\n";
    Code += IsAnimated ? L"if Final_R==nil then Final_R=255 end\n" : L"";
    Code += IsAnimated ? L"if Final_G==nil then Final_G=0 end\n" : L"";
    Code += IsAnimated ? L"if Final_B==nil then Final_B=0 end\n" : L"";
    Code += IsAnimated ? L"if Final_A==nil then Final_A=0 end\n" : L"";
    Code += IsAnimated ? L"if Iteration==nil then Iteration=10 end\n" : L"";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,1,1)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    Code += IsAnimated ? L"local OffsetedTime=math.abs((Time*Iteration)%2.0-1.0)\n" : L"";
    Code += IsAnimated ? L"local R=math.ceil(OffsetedTime*(Final_R-Initial_R)+Initial_R)\n" : L"";
    Code += IsAnimated ? L"local G=math.ceil(OffsetedTime*(Final_G-Initial_G)+Initial_G)\n" : L"";
    Code += IsAnimated ? L"local B=math.ceil(OffsetedTime*(Final_B-Initial_B)+Initial_B)\n" : L"";
    Code += IsAnimated ? L"local A=math.ceil(OffsetedTime*(Final_A-Initial_A)+Initial_A)\n" : L"";
    EncodeColor::ColorR = L"R";
    EncodeColor::ColorG = L"G";
    EncodeColor::ColorB = L"B";
    Code += L"local Color=" + EncodeColor::MakeColor() + L"\n";
    Code += L"local Brush=EXE(QM(\"CreateSolidBrush\"),Color)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,Brush)\n";
    Code += L"EXE(QM(\"PatBlt\"),MEMdc,0,0,1,1,PATCOPY)\n";
    Code += L"EXE(QM(\"DeleteObject\"),Brush)\n";
    Code += L"local Line=0\n";
    Code += L"while Line<FrameSize do\n";
    Code += L"local LeftTopA=math.ceil(((FrameSize-Line)/FrameSize)*A)\n";
    Code += L"local RightBottomA=math.ceil((Line/FrameSize)*A)\n";
    EncodeColor::ColorA = L"LeftTopA";
    Code += L"local BlendFuncLeftTop=" + EncodeColor::MakeAlpha() + L"\n";
    EncodeColor::ColorA = L"RightBottomA";
    Code += L"local BlendFuncRightBottom=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,Line,W,Resolution,MEMdc,0,0,1,1,BlendFuncLeftTop)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,Line,0,Resolution,H,MEMdc,0,0,1,1,BlendFuncLeftTop)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,H-FrameSize+Line,W,Resolution,MEMdc,0,0,1,1,BlendFuncRightBottom)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,W-FrameSize+Line,0,Resolution,H,MEMdc,0,0,1,1,BlendFuncRightBottom)\n";
    Code += L"Line=Line+Resolution\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"end\n";
    return Code;
}

wstring GlassShader(bool Reversed) {
    wstring Code = L"";
    Code += L"function " + wstring(Reversed ? L"Reversed" : L"") + L"GlassFadeShader(DC,Final,Time,Elapsed,W,H,PersistentData,BlendAlpha,MaxBlur,MinBlur)\n";
    Code += L"if BlendAlpha==nil then BlendAlpha=(1<<6)-1 end\n";
    Code += L"if MaxBlur==nil then MaxBlur=10 end\n";
    Code += L"if MinBlur==nil then MinBlur=0 end\n";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,W,H)\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    EncodeColor::ColorA = L"BlendAlpha";
    Code += L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n";
    Code += L"EXE(QM(\"BitBlt\"),MEMdc,0,0,W,H,DC,0,0,SRCCOPY)\n";
    Code += L"local Offset=math.ceil((MaxBlur-MinBlur)*" + wstring(Reversed ? L"(1.0-Time)" : L"Time") + L"+MinBlur)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),MEMdc,Offset,Offset,W,H,DC,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),MEMdc,Offset,-Offset,W,H,DC,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),MEMdc,-Offset,Offset,W,H,DC,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),MEMdc,-Offset,-Offset,W,H,DC,0,0,W,H,BlendFunc)\n";
    Code += L"EXE(QM(\"BitBlt\"),DC,0,0,W,H,MEMdc,0,0,SRCCOPY)\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"end\n";
    return Code;
}

wstring PixelateShader(bool WithAlpha) {
    wstring Code = L"";
    Code += L"function " + wstring(WithAlpha ? L"Alpha" : L"") + L"PixelateShader(DC,Final,Time,Elapsed,W,H,PersistentData,Multiplier" + wstring(WithAlpha ? L",Alpha" : L"") + L")\n";
    Code += L"if Multiplier==nil then Multiplier=4 end\n";
    Code += WithAlpha ? L"if Alpha==nil then Alpha=(1<<7)+(1<<6)-1 end\n" : L"";
    Code += L"local MEMdc=EXE(QM(\"CreateCompatibleDC\"),DC)\n";
    Code += L"local BMP=EXE(QM(\"CreateCompatibleBitmap\"),DC,math.floor(W/Multiplier),math.floor(H/Multiplier))\n";
    Code += L"EXE(QM(\"SelectObject\"),MEMdc,BMP)\n";
    Code += L"EXE(QM(\"AlphaBlend\"),MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier),DC,0,0,W,H,0x00FF0000)\n";
    EncodeColor::ColorA = L"Alpha";
    Code += WithAlpha ? L"local BlendFunc=" + EncodeColor::MakeAlpha() + L"\n" : L"";
    Code += L"EXE(QM(\"AlphaBlend\"),DC,0,0,W,H,MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier)," + wstring(WithAlpha ? L"BlendFunc" : L"0x00FF0000") + L")\n";
    Code += L"EXE(QM(\"DeleteDC\"),MEMdc)\n";
    Code += L"EXE(QM(\"DeleteObject\"),BMP)\n";
    Code += L"end\n";
    return Code;
}

wstring MatrixShader() {
    wstring Code = L"";
    Code += L"function MatrixShader(DC,Final,Time,Elapsed,W,H,PersistentData,Cols,CharHeight,FontWeight,MaxSpeed,MinSpeed,MaxTailLength,MinTailLength,FontName,R,G,B)\n";
    Code += L"if Cols==nil then Cols=50 end\n";
    Code += L"if CharHeight==nil then CharHeight=20 end\n";
    Code += L"if FontWeight==nil then FontWeight=400 end\n";
    Code += L"if MaxSpeed==nil then MaxSpeed=20 end\n";
    Code += L"if MinSpeed==nil then MinSpeed=5 end\n";
    Code += L"if MaxTailLength==nil then MaxTailLength=20 end\n";
    Code += L"if MinTailLength==nil then MinTailLength=5 end\n";
    Code += L"if FontName==nil then FontName=\"Fixedsys\" end\n";
    Code += L"if R==nil then R=0 end\n";
    Code += L"if G==nil then G=255 end\n";
    Code += L"if B==nil then B=0 end\n";
    Code += L"local CharWidth=math.ceil(W/Cols)\n";
    Code += L"if #PersistentData~=Cols then\n";
    Code += L"for Col=1,Cols do\n";
    Code += L"PersistentData[Col]={Y=math.random(0,H),Speed=math.random(MinSpeed,MaxSpeed),TailLength=math.random(MinTailLength,MaxTailLength)}\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"SetBkMode\"),DC,1)\n";
    Code += L"local Font=EXE(QM(\"CreateFontW\"),\n";
    Code += L"CharHeight,-- FontHeight\n";
    Code += L"CharWidth,-- FontWidth\n";
    Code += L"0,-- Escapement\n";
    Code += L"0,-- Orientation\n";
    Code += L"FontWeight,-- FontWeight\n";
    Code += L"0,-- Italic\n";
    Code += L"0,-- Underline\n";
    Code += L"0,-- StrikeOut\n";
    Code += L"1,-- CharSet\n";
    Code += L"0,-- OutPrecision\n";
    Code += L"0,-- ClipPrecision\n";
    Code += L"0,-- Quality\n";
    Code += L"0,-- PitchAndFamily\n";
    Code += L"{type=4,value=FontName})-- FaceName\n";
    Code += L"EXE(QM(\"SelectObject\"),DC,Font)\n";
    Code += L"for Col=1,Cols do\n";
    Code += L"local ColData=PersistentData[Col]\n";
    Code += L"ColData.Y=ColData.Y+ColData.Speed\n";
    Code += L"if ColData.Y>H+MaxTailLength*CharHeight then\n";
    Code += L"ColData.Y=0\n";
    Code += L"ColData.Speed=math.random(MinSpeed,MaxSpeed)\n";
    Code += L"ColData.TailLength=math.random(MinTailLength,MaxTailLength)\n";
    Code += L"end\n";
    Code += L"for TailPiece=0,ColData.TailLength do\n";
    Code += L"local PieceY=ColData.Y-TailPiece*CharHeight\n";
    Code += L"if PieceY>=-CharHeight and PieceY<H then\n";
    Code += L"local Brightness=1-TailPiece/ColData.TailLength\n";
    Code += L"local NewR=math.ceil(R*Brightness)\n";
    Code += L"local NewG=math.ceil(G*Brightness)\n";
    Code += L"local NewB=math.ceil(B*Brightness)\n";
    EncodeColor::ColorR = L"NewR";
    EncodeColor::ColorG = L"NewG";
    EncodeColor::ColorB = L"NewB";
    Code += L"local TextColor=" + EncodeColor::MakeColor() + L"\n";
    Code += L"EXE(QM(\"SetTextColor\"),DC,TextColor)\n";
    Code += L"EXE(QM(\"TextOutA\"),DC,(Col-1)*CharWidth,math.floor(PieceY),string.char(math.random(0x21,0x7E)),1)\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"end\n";
    Code += L"EXE(QM(\"DeleteObject\"),Font)\n";
    Code += L"end\n";
    return Code;
}

wstring BuiltinShaders() {
    wstring Code = L"";
    Code += BlankShader(true);
    Code += BlankShader(false);
    Code += WaveShader(L"Wave", false, false);
    Code += WaveShader(L"Glitch", true, true);
    Code += WaveShader(L"WaveGlitch", false, true);
    Code += ColorBurnShader();
    Code += RainbowBurnShader(false, false, false);
    Code += RainbowBurnShader(true, false, false);
    Code += RainbowBurnShader(false, true, false);
    Code += RainbowBurnShader(true, true, false);
    Code += RainbowBurnShader(false, true, true);
    Code += RainbowBurnShader(true, true, true);
    Code += ColorOffset(false);
    Code += ColorOffset(true);
    Code += FadeShader(true, false);
    Code += FadeShader(false, false);
    Code += FadeShader(true, true);
    Code += FadeShader(false, true);
    Code += HatchBrushShader();
    Code += TextShader(false, false);
    Code += TextShader(true, false);
    Code += TextShader(true, true);
    Code += RotatePointTool();
    Code += RadialBlurShader(true, false);
    Code += RadialBlurShader(false, true);
    Code += RadialBlurShader(false, false);
    Code += PolygonShader(false, false);
    Code += PolygonShader(true, false);
    Code += PolygonShader(true, true);
    Code += TriangleFrameShader(false);
    Code += TriangleFrameShader(true);
    Code += CRTShader();
    Code += NoiseShader();
    Code += FrameShader(false);
    Code += FrameShader(true);
    Code += GlassShader(false);
    Code += GlassShader(true);
    Code += PixelateShader(false);
    Code += PixelateShader(true);
    Code += MatrixShader();
    return Code;
}

//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT nCmdShow)
{
    InitCommonControls();
    SetThemeAppProperties(STAP_ALLOW_CONTROLS | STAP_ALLOW_NONCLIENT);

    extern int __argc;
    extern wchar_t** __wargv;

    //Debug console
    AllocConsole();
    _wfreopen(L"CONIN$", L"r", stdin);
    _wfreopen(L"CONOUT$", L"w", stdout);
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_TEXT);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    pos.X = 4;
    pos.Y = 2;
    SetConsoleCursorPosition(hConsole, pos);
    wcout << WelcomeText;
    wcout << L" v" << VersionString;
    wcout << L"\n\n\n";
    for (uint8_t i = 0; i < (1 << 6); i++) {
        wcout << L"=";
    }
    wcout << L"\n\n";
    wcout << StartupStage1Text << L"\n";

    if (__argc == 3) {
        wstring ARG1 = __wargv[1];
        wstring ARG2 = __wargv[2];
        if (ARG1 == L"OPEN") {
            if (ARG2 == L"PFXSTUDIO") {
                Config& config = Config::GetConfig();
                const VisualSettings& cVisual = config.GetVisualSettings();
                const ViewSettings& cView = config.GetViewSettings();
                const VideoSettings& cVideo = config.GetVideoSettings();
                const ControlSettings& cControl = config.GetControlSettings();
                const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
                wstring Code = L"";
                Code += MultiInstanceGuardInit();
                Code += L"-- Variable Definitions: \n";
                Code += DefineMemoryAddress(L"PointersInitialized", GetAddress(PointersInitialized), false);
                Code += L"collectgarbage(\"collect\")\n";// Clean up any leftovers from previous run
                Code += L"while readShortInteger(PointersInitialized)==0 do\n";
                Code += L"processMessages()\n";
                Code += L"end\n";
                Code += DefineMemoryAddress(L"SongLength", GetAddress(Ptr_to_m_llMaxTime), true);
                Code += DefineMemoryAddress(L"InitialSilence", GetAddress(Ptr_to_m_llMinTime), true);
                Code += DefineMemoryAddress(L"Microseconds", GetAddress(Ptr_to_m_llStartTime), true);
                Code += DefineMemoryAddress(L"Ticks", GetAddress(Ptr_to_m_iStartTick), true);
                Code += DefineMemoryAddress(L"Resolution", GetAddress(Ptr_to_m_MIDI_iResolution), true);
                Code += DefineMemoryAddress(L"NoteCount", GetAddress(Ptr_to_m_MIDI_iTotalNC), true);
                Code += DefineMemoryAddress(L"NotesPerSecond", GetAddress(Ptr_to_m_iNPS), true);
                Code += DefineMemoryAddress(L"Polyphony", GetAddress(Ptr_to_m_iPolyphony), true);
                Code += DefineMemoryAddress(L"Passed", GetAddress(Ptr_to_m_iPassed), true);
                Code += DefineMemoryAddress(L"Volume", cPlayback.GetVolumeAddress(), false);
                Code += DefineMemoryAddress(L"Mute", cPlayback.GetMuteAddress(), false);
                Code += DefineMemoryAddress(L"PlaybackSpeed", cPlayback.GetSpeedAddress(), false);
                Code += DefineMemoryAddress(L"NoteSpeed", cPlayback.GetNSpeedAddress(), false);
                Code += DefineMemoryAddress(L"OffsetX", cView.GetOffsetXAddress(), false);
                Code += DefineMemoryAddress(L"OffsetY", cView.GetOffsetYAddress(), false);
                Code += DefineMemoryAddress(L"Zoom", cView.GetZoomXAddress(), false);
                Code += DefineMemoryAddress(L"UpdateNotePos", GetAddress(Ptr_to_m_bUpdateNotePos), true);
                Code += DefineMemoryAddress(L"UpdateTrackColor", GetAddress(Ptr_to_m_bUpdateTrackColor), true);
                Code += DefineMemoryAddress(L"SameWidth", GetAddress(cVideo.bSameWidth), false);
                Code += DefineMemoryAddress(L"VelocityMapping", GetAddress(cVideo.bMapVel), false);
                Code += DefineMemoryAddress(L"StartKey", GetAddress(cVisual.iFirstKey), false);
                Code += DefineMemoryAddress(L"EndKey", GetAddress(cVisual.iLastKey), false);
                Code += DefineMemoryAddress(L"KeyMode", GetAddress(cVisual.eKeysShown), false);
                Code += DefineMemoryAddress(L"Width", GetAddress(Ptr_to_m_iWidth), true);
                Code += DefineMemoryAddress(L"Height", GetAddress(Ptr_to_m_iHeight), true);
                Code += DefineMemoryAddress(L"Paused", cPlayback.GetPausedAddress(), false);
                Code += DefineMemoryAddress(L"Keyboard", cView.GetKeyboardAddress(), false);
                Code += DefineMemoryAddress(L"VisualizePitchBends", GetAddress(cVideo.bVisualizePitchBends), false);
                Code += DefineMemoryAddress(L"PhigrosMode", GetAddress(cControl.bPhigros), false);
                Code += DefineMemoryAddress(L"ShowMarkers", GetAddress(cVideo.bShowMarkers), false);
                Code += DefineMemoryAddress(L"TickBased", GetAddress(cVideo.bTickBased), false);
                Code += DefineMemoryAddress(L"HideStatistics", GetAddress(cVideo.bDisableUI), false);
                Code += DefineMemoryAddress(L"RemoveOverlaps", GetAddress(cVideo.bOR), false);
                Code += DefineMemoryAddress(L"LimitFPS", GetAddress(cVideo.bLimitFPS), false);
                Code += DefineMemoryAddress(L"VelocityThreshold", GetAddress(cControl.iVelocityThreshold), false);
                Code += DefineMemoryAddress(L"Caption", GetAddress(Ptr_to_CaptionContent), true);
                Code += DefineMemoryAddress(L"CaptionAlignment", GetAddress(Ptr_to_CheatEngineCaption), true);
                Code += DefineMemoryAddress(L"DifficultyText", GetAddress(Ptr_to_Difficulty), true);
                Code += L"-- FFMPEG Video Export\n";
                Code += DefineMemoryAddress(L"CE_VideoOutput", GetAddress(Ptr_to_CE_VideoOutput), true);
                Code += DefineMemoryAddress(L"CE_Connected", GetAddress(Ptr_to_CE_Connected), true);
                Code += DefineMemoryAddress(L"CE_DoNextTick", GetAddress(Ptr_to_CE_DoNextTick), true);
                Code += DefineMemoryAddress(L"CE_Responded", GetAddress(Ptr_to_CE_Responded), true);
                Code += DefineMemoryAddress(L"CE_Disconnect", GetAddress(Ptr_to_CE_Disconnect), true);
                Code += L"while readShortInteger(CE_Disconnect)~=0 do\n";
                Code += L"processMessages()\n";
                Code += L"end\n";
                Code += L"-- Custom Variable Definitions: (Version: " VersionString L")\n";
                Code += L"\n";
                Code += L"-- Function Definitions: (DO NOT CHANGE!)\n";
                Code += InitializeVariables();
                Code += FakeThreadingEngine();
                Code += MethodsLookup();
                Code += CreateDebugWindow(L"Pending", L"0", L"0", L"400", L"250");
                Code += CreateDebugWindow(L"Active", L"0", L"280", L"400", L"220");
                Code += CreateDebugWindow(L"History", L"400", L"0", L"350", L"500", false);
                Code += GettersAndSetters(L"SongLength", IntSizeToCE(sizeof(*Ptr_to_m_llMaxTime)), true, L"History");
                Code += GettersAndSetters(L"InitialSilence", IntSizeToCE(sizeof(*Ptr_to_m_llMinTime)), true, L"History");
                Code += GettersAndSetters(L"Microseconds", IntSizeToCE(sizeof(*Ptr_to_m_llStartTime)), false, L"History");
                Code += GettersAndSetters(L"Ticks", IntSizeToCE(sizeof(*Ptr_to_m_iStartTick)), false, L"History", true);
                Code += GettersAndSetters(L"Resolution", IntSizeToCE(sizeof(*Ptr_to_m_MIDI_iResolution)), true, L"History");
                Code += GettersAndSetters(L"NoteCount", IntSizeToCE(sizeof(*Ptr_to_m_MIDI_iTotalNC)), true, L"History");
                Code += GettersAndSetters(L"NotesPerSecond", IntSizeToCE(sizeof(*Ptr_to_m_iNPS)), true, L"History");
                Code += GettersAndSetters(L"Polyphony", IntSizeToCE(sizeof(*Ptr_to_m_iPolyphony)), true, L"History");
                Code += GettersAndSetters(L"Passed", IntSizeToCE(sizeof(*Ptr_to_m_iPassed)), true, L"History");
                Code += GettersAndSetters(L"Volume", FloatSizeToCE(cPlayback.GetVolumeSize()), false, L"History");
                Code += GettersAndSetters(L"Mute", IntSizeToCE(cPlayback.GetMuteSize()), false, L"History");
                Code += GettersAndSetters(L"PlaybackSpeed", FloatSizeToCE(cPlayback.GetSpeedSize()), false, L"History");
                Code += GettersAndSetters(L"NoteSpeed", FloatSizeToCE(cPlayback.GetNSpeedSize()), false, L"History");
                Code += GettersAndSetters(L"OffsetX", FloatSizeToCE(cView.GetOffsetXSize()), false, L"History", false, true);
                Code += GettersAndSetters(L"OffsetY", FloatSizeToCE(cView.GetOffsetYSize()), false, L"History", false, true);
                Code += GettersAndSetters(L"Zoom", FloatSizeToCE(cView.GetZoomXSize()), false, L"History", false, true);
                Code += GettersAndSetters(L"SameWidth", IntSizeToCE(sizeof(cVideo.bSameWidth)), false, L"History", false, true);
                Code += GettersAndSetters(L"VelocityMapping", IntSizeToCE(sizeof(cVideo.bMapVel)), false, L"History");
                Code += GettersAndSetters(L"StartKey", IntSizeToCE(sizeof(cVisual.iFirstKey)), false, L"History", false, true);
                Code += GettersAndSetters(L"EndKey", IntSizeToCE(sizeof(cVisual.iLastKey)), false, L"History", false, true);
                Code += GettersAndSetters(L"KeyMode", IntSizeToCE(sizeof(cVisual.eKeysShown)), false, L"History", false, true);
                Code += GettersAndSetters(L"Width", IntSizeToCE(sizeof(*Ptr_to_m_iWidth)), true, L"History", true, true);
                Code += GettersAndSetters(L"Height", IntSizeToCE(sizeof(*Ptr_to_m_iHeight)), true, L"History", true, true);
                Code += SetResolutionFunction();
                Code += GettersAndSetters(L"Paused", IntSizeToCE(cPlayback.GetPausedSize()), false, L"History");
                Code += GettersAndSetters(L"Keyboard", IntSizeToCE(cView.GetKeyboardVarSize()), false, L"History");
                Code += GettersAndSetters(L"VisualizePitchBends", IntSizeToCE(sizeof(cVideo.bVisualizePitchBends)), false, L"History", false, true);
                Code += GettersAndSetters(L"PhigrosMode", IntSizeToCE(sizeof(cControl.bPhigros)), false, L"History");
                Code += GettersAndSetters(L"ShowMarkers", IntSizeToCE(sizeof(cVideo.bShowMarkers)), false, L"History");
                Code += GettersAndSetters(L"TickBased", IntSizeToCE(sizeof(cVideo.bTickBased)), false, L"History");
                Code += GettersAndSetters(L"HideStatistics", IntSizeToCE(sizeof(cVideo.bDisableUI)), false, L"History");
                Code += GettersAndSetters(L"RemoveOverlaps", IntSizeToCE(sizeof(cVideo.bOR)), false, L"History");
                Code += GettersAndSetters(L"LimitFPS", IntSizeToCE(sizeof(cVideo.bLimitFPS)), false, L"History");
                Code += GettersAndSetters(L"VelocityThreshold", IntSizeToCE(sizeof(cControl.iVelocityThreshold)), false, L"History");
                Code += StringTypeGettersAndSetters(L"Caption", MainScreen::CE_Caption_Size(), false, L"History", true);
                Code += StringTypeGettersAndSetters(L"DifficultyText", MainScreen::Difficulty_Size(), false, L"History");
                Code += EasingFunctions();
                Code += WaitUntil(false, L"Pending");
                Code += WaitUntil(true, L"Pending");
                Code += AnimationFunction(false, false, false, false, L"Pending", L"Active");
                Code += AnimationFunction(true, false, false, false, L"Pending", L"Active");
                Code += AnimationFunction(false, true, false, false, L"Pending", L"Active");
                Code += AnimationFunction(true, true, false, false, L"Pending", L"Active");
                Code += InstantaneousAnimationFunction(false, false, L"Pending");
                Code += AnimationFunction(false, false, true, false, L"Pending", L"Active");
                Code += AnimationFunction(true, false, true, false, L"Pending", L"Active");
                Code += AnimationFunction(false, true, true, false, L"Pending", L"Active");
                Code += AnimationFunction(true, true, true, false, L"Pending", L"Active");
                Code += InstantaneousAnimationFunction(true, false, L"Pending");
                Code += AnimationFunction(false, false, false, true, L"Pending", L"Active");
                Code += AnimationFunction(true, false, false, true, L"Pending", L"Active");
                Code += AnimationFunction(false, true, false, true, L"Pending", L"Active");
                Code += AnimationFunction(true, true, false, true, L"Pending", L"Active");
                Code += InstantaneousAnimationFunction(false, true, L"Pending");
                Code += AnimationFunction(false, false, true, true, L"Pending", L"Active");
                Code += AnimationFunction(true, false, true, true, L"Pending", L"Active");
                Code += AnimationFunction(false, true, true, true, L"Pending", L"Active");
                Code += AnimationFunction(true, true, true, true, L"Pending", L"Active");
                Code += InstantaneousAnimationFunction(true, true, L"Pending");
                Code += ShaderEngine();
                Code += L"-- Builtin GDI Shaders: \n";
                Code += BuiltinShaders();
                Code += L"-- Custom GDI Shaders: \n";
                Code += L"\n";
                Code += L"-- Initialization: \n";
                Code += L"writeShortInteger(CE_Connected,1)\n";
                Code += L"EXE(QM(\"SetForegroundWindow\"),hPFX)\n";
                Code += L"DisplayShaderFPS=true\n";
                Code += L"SetMicroseconds(GetInitialSilence())\n";
                Code += L"SetVolume(1.00)\n";
                Code += L"SetMute(0)\n";
                Code += L"SetPlaybackSpeed(1.00)\n";
                Code += L"SetNoteSpeed(0.25)\n";
                Code += L"SetStartKey(0)\n";
                Code += L"SetEndKey(127)\n";
                Code += L"SetKeyMode(0)\n";
                Code += L"SetOffsetX(CenterX(0))\n";
                Code += L"SetOffsetY(CenterY(0))\n";
                Code += L"SetZoom(0)\n";
                Code += L"SetSameWidth(0)\n";
                Code += L"SetVelocityMapping(0)\n";
                Code += L"SetPaused(0)\n";
                Code += L"SetKeyboard(1)\n";
                Code += L"SetVisualizePitchBends(1)\n";
                Code += L"SetPhigrosMode(1)\n";
                Code += L"SetShowMarkers(1)\n";
                Code += L"SetTickBased(1)\n";
                Code += L"SetHideStatistics(0)\n";
                Code += L"SetRemoveOverlaps(0)\n";
                Code += L"SetLimitFPS(0)\n";
                Code += L"SetVelocityThreshold(1)\n";
                Code += L"SetCaptionAlignment(ALIGN_CENTER)\n";
                Code += L"SetCaption(\"Welcome to Piano-FX Pro\")\n";
                Code += L"-- If you feel the default difficulty is inappropriate, change it with the following code: \n";
                Code += L"SetDifficultyText(\"IN Lv.12\")\n";
                Code += L"-- Do this if you have color events on the first tick: \n";
                Code += L"SetMicroseconds(0)\n";
                Code += L"WaitUntilTime(1)\n";
                Code += L"SetMicroseconds(GetInitialSilence())\n";
                Code += L"-- Main Program: \n";
                Code += L"Spawn(ShaderEngine)\n";
                Code += L"RunShaderByTime(GetInitialSilence(),GetInitialSilence()+2*S,ReversedDarkFadeShader)\n";
                Code += L"RunShaderByTime(GetInitialSilence(),GetInitialSilence()+2*S,ReversedGlassFadeShader,nil,10,0)\n";
                Code += L"RunShaderByTime(GetInitialSilence(),GetInitialSilence()+2*S,ReversedGlassFadeShader,nil,20,0)\n";
                Code += L"SmoothAnimationByTime(0,1,GetInitialSilence()+S,GetInitialSilence()+3*S,EaseOutBack,SetZoom)\n";
                Code += L"SmoothAnimationByTime(CenterX(0),CenterX(1),GetInitialSilence()+S,GetInitialSilence()+3*S,EaseOutBack,SetOffsetX)\n";
                Code += L"SmoothAnimationByTime(CenterY(0),CenterY(1),GetInitialSilence()+S,GetInitialSilence()+3*S,EaseOutBack,SetOffsetY)\n";
                Code += L"InstantaneousAnimationByTimeAndWait(\"\",0,SetCaption)\n";
                Code += L"-- Cleaning Up: \n";
                Code += CleanUp();
                LuaCode = Code;
                StudioMain();
                return 0;
            }
            if (ARG2 == L"CheatEngineTutorial") {
                DisplayTutorial(CheatEngineTutorial);
                return 0;
            }
            if (ARG2 == L"VisualizePitchBendTutorial") {
                DisplayTutorial(VisualizePitchBendTutorial);
                return 0;
            }
            if (ARG2 == L"GDITutorial") {
                DisplayTutorial(GDITutorial);
                return 0;
            }
            if (ARG2 == L"SMF3Intro") {
                DisplayTutorial(SMF3Intro);
                return 0;
            }
        }
    }

#ifdef INCLUDE_FFMPEG
    unsigned char* pData = new unsigned char[ffmpeg_len];
    unsigned int offset = 0;
    memcpy(pData + offset, ffmpeg1, sizeof(ffmpeg1));
    offset += sizeof(ffmpeg1);
    memcpy(pData + offset, ffmpeg2, sizeof(ffmpeg2));
    offset += sizeof(ffmpeg2);
    memcpy(pData + offset, ffmpeg3, sizeof(ffmpeg3));
    offset += sizeof(ffmpeg3);
    memcpy(pData + offset, ffmpeg4, sizeof(ffmpeg4));
    offset += sizeof(ffmpeg4);
    if (offset == ffmpeg_len) {
        constexpr uint8_t lzma_magic[] = { 0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00 };
        while (ffmpeg_len >= LZMA_STREAM_HEADER_SIZE * 2 && !memcmp(pData, lzma_magic, sizeof(lzma_magic))) {
            unsigned char* compressed = pData;
            uint64_t decompressed_size = 0;
            lzma_stream strm = LZMA_STREAM_INIT;
            lzma_stream_flags stream_flags;
            lzma_index* index = nullptr;
            auto pos = (int64_t)ffmpeg_len;
            lzma_ret ret;
            do {
                pos -= LZMA_STREAM_HEADER_SIZE;
                uint64_t footer_pos;
                for (;;) {
                    footer_pos = pos;

                    int i = 2;
                    if (*(uint32_t*)&compressed[footer_pos + 8] != 0)
                        break;

                    do {
                        pos -= 4;
                        --i;
                    } while (i >= 0 && *(uint32_t*)&compressed[footer_pos + i * 4] == 0);
                }
                ret = lzma_stream_footer_decode(&stream_flags, &compressed[footer_pos]);
                pos -= stream_flags.backward_size;
                lzma_index_decoder(&strm, &index, UINT64_MAX);
                strm.avail_in = stream_flags.backward_size;
                strm.next_in = &compressed[pos];
                pos += stream_flags.backward_size;
                ret = lzma_code(&strm, LZMA_RUN);
                pos -= stream_flags.backward_size + LZMA_STREAM_HEADER_SIZE;
                pos -= lzma_index_total_size(index);
                decompressed_size += lzma_index_uncompressed_size(index);
            } while (pos > 0);
            pData = new unsigned char[decompressed_size];
            uint8_t* write_ptr = pData;
            lzma_end(&strm);
            strm = LZMA_STREAM_INIT;
            lzma_stream_decoder(&strm, UINT64_MAX, LZMA_CONCATENATED);
            strm.next_in = compressed;
            strm.avail_in = ffmpeg_len;
            bool done = false;
            lzma_action action = LZMA_RUN;
            while (!done) {
                if (strm.avail_in == 0)
                    action = LZMA_FINISH;
                lzma_ret ret = lzma_code(&strm, action);
                if (strm.avail_out == 0) {
                    auto remaining = min(decompressed_size - (write_ptr - pData), 1 << 20);
                    strm.next_out = write_ptr;
                    strm.avail_out = remaining;
                    write_ptr += remaining;
                }
                switch (ret) {
                case LZMA_STREAM_END:
                    done = true;
                    break;
                case LZMA_OK:
                    break;
                }
            }
            ffmpeg_len = decompressed_size;
        }
        wstring FFmpegPath = ProgramPath() + L"\\FFMPEG.exe";
        if (!filesystem::exists(FFmpegPath)) {
            ofstream FFmpegFile(FFmpegPath, ios::binary);
            FFmpegFile.write(reinterpret_cast<const char*>(pData), ffmpeg_len);
            FFmpegFile.close();
        }
        delete[] pData;
    }
    else {
        wcout << L"Embedded ffmpeg.exe data length incorrect! \n";
        wcout << L"Expected: ";
        wcout << ffmpeg_len;
        wcout << L"\n";
        wcout << L"Actual: ";
        wcout << offset;
        wcout << L"\n";
        for (;;) {

        }
    }
#endif

    g_hInstance = hInstance;
    srand((unsigned int)time(NULL));

    // Ensure that the common control DLL is loaded. 
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    // Initialize COM. For the SH* functions
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return 1;

    // Register the window class
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0L;
    wc.cbWndExtra = 0L;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PFAICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // Window is only a container... never seen, thus null brush
    wc.hbrBackground = NULL; //( HBRUSH )GetStockObject( NULL_BRUSH );
    wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
    wc.lpszClassName = CLASSNAME;
    wc.hIconSm = NULL;
    if (!RegisterClassEx(&wc))
        return 1;

    // Register the graphics window class
    wc.style = CS_OWNDC | CS_SAVEBITS;
    wc.lpfnWndProc = GfxProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = GFXCLASSNAME;
    if (!RegisterClassEx(&wc))
        return 1;

    // Register the position control window class
    wc.style = NULL;
    wc.lpfnWndProc = PosnProc;
    wc.lpszClassName = POSNCLASSNAME;
    if (!RegisterClassEx(&wc))
        return 1;

    // In addition to getting settings, triggers loading of saved config
    Config& config = Config::GetConfig();
    ViewSettings& cView = config.GetViewSettings();

    // Create the application window
    g_hWnd = CreateWindowEx(WS_EX_APPWINDOW | RTL_STYLE, CLASSNAME, TitleIdle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, cView.GetMainLeft(), cView.GetMainTop(), cView.GetMainWidth(), cView.GetMainHeight(), NULL, NULL, wc.hInstance, NULL);

    if (!g_hWnd) return 1;

    // Accept drag and drop
    DragAcceptFiles(g_hWnd, true);

    // Creation order (z-order) matters big time for full screen

    // Create the controls rebar
    g_hWndBar = CreateRebar(g_hWnd);
    if (!g_hWndBar) return 1;

    // Create the graphics window
    g_hWndGfx = CreateWindowEx(WS_EX_CLIENTEDGE | RTL_STYLE, GFXCLASSNAME, NULL, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, NULL, NULL, NULL, NULL, g_hWnd, NULL, wc.hInstance, NULL);
    if (!g_hWndGfx) return 1;

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAINMENU));
    if (!hAccel) return 1;

    HANDLE hThread;

    MIDI::InitArrays();
    if (__argc == 2) {
        // Get the game going
        hThread = CreateThread(NULL, 0, GameThread, new IntroScreen(NULL, NULL), 0, NULL);
        if (!hThread) return 1;
        // Set up GUI and show
        SetPlayMode(GameState::Intro);
        SetOnTop(cView.GetOnTop());
        ShowControls(cView.GetControls());
        ShowWindow(g_hWndGfx, SW_SHOW);
        ShowWindow(g_hWnd, nCmdShow);
        UpdateWindow(g_hWnd);
        SetFocus(g_hWndGfx);
        PlayFile(__wargv[1]);
    }
    else {
        wcout << StartupStage2Text << L"\n";
        // Get the game going
        hThread = CreateThread(NULL, 0, GameThread, new SplashScreen(NULL, NULL), 0, NULL);
        if (!hThread) return 1;
        // Set up GUI and show
        SetPlayMode(GameState::Splash);
        SetOnTop(cView.GetOnTop());
        ShowControls(cView.GetControls());
        ShowWindow(g_hWndGfx, SW_SHOW);
        ShowWindow(g_hWnd, nCmdShow);
        UpdateWindow(g_hWnd);
        SetFocus(g_hWndGfx);
    }
    wcout << StartupStage3Text << L"\n";

    // Enter the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(g_hWnd, hAccel, &msg) &&
            !IsDialogMessage(g_hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Signal the game thread to exit and wait for it
    g_MsgQueue.ForcePush(msg);
    WaitForSingleObject(hThread, INFINITE);

    // Save settings
    config.SaveConfigValues();

    // Clean up
    UnregisterClass(CLASSNAME, wc.hInstance);
    CoUninitialize();
    return 0;
}

DWORD WINAPI GameThread(LPVOID lpParameter)
{
    if (!g_hWndGfx) return 0;

    // Initialize Direct3D
    Renderer11* pRenderer = new Renderer11();
    auto init_res = pRenderer->Init(g_hWndGfx, Config::GetConfig().GetVideoSettings().bLimitFPS);
    if (FAILED(get<0>(init_res)))
    {
        wchar_t msg[LONG_MAX_PATH] = {};
        _snwprintf_s(msg, LONG_MAX_PATH, L"Fatal error initializing Direct3D. Is DirectX 11 installed properly? \n%s failed with code 0x%x. ", get<1>(init_res), get<0>(init_res));
        MessageBoxW(g_hWnd, msg, ErrorTitle, MB_OK | MB_ICONERROR);
        PostMessage(g_hWnd, WM_QUIT, 1, 0);
        return 1;
    }

    // Create the game object
    GameState::GameError ErrorLevel;
    GameState* pGameState = reinterpret_cast<GameState*>(lpParameter);
    pGameState->SetHWnd(g_hWndGfx);
    pGameState->SetRenderer(pRenderer);
    if ((ErrorLevel = pGameState->Init()) != GameState::Success) {
        PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
    }

    wchar_t buf[LONG_MAX_PATH] = {};
    _snwprintf_s(buf, LONG_MAX_PATH, TitleSplash);
    SetWindowTextW(g_hWnd, buf);

    // Event, logic, render...
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        while (g_MsgQueue.Pop(msg)) {
            if ((ErrorLevel = pGameState->MsgProc(msg.hwnd, msg.message, msg.wParam, msg.lParam)) != GameState::Success) {
                PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
            }
        }

        if ((ErrorLevel = GameState::ChangeState(pGameState->NextState(), &pGameState)) != GameState::Success) {
            PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
        }
        if ((ErrorLevel = pGameState->Logic()) != GameState::Success) {
            PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
            if (GameState::ChangeState(new IntroScreen(NULL, NULL), &pGameState) != GameState::Success) {
                MessageBoxW(g_hWnd, GameState::Errors[ErrorLevel].c_str(), ErrorTitle, MB_OK | MB_ICONERROR);
                PostMessage(g_hWnd, WM_QUIT, 1, 0);
            }
        }
        if ((ErrorLevel = pGameState->Render()) != GameState::Success) {
            PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
            if (GameState::ChangeState(new IntroScreen(NULL, NULL), &pGameState) != GameState::Success) {
                MessageBoxW(g_hWnd, GameState::Errors[ErrorLevel].c_str(), ErrorTitle, MB_OK | MB_ICONERROR);
                PostMessage(g_hWnd, WM_QUIT, 1, 0);
            }
        }
    }

    delete pGameState;
    delete pRenderer;

    return 0;
}