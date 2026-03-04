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

#ifdef INCLUDE_FFMPEG
// Do not include these files in Debug configuration as these files are too large and would cause significant lag in intellisense! 
#include <ffmpeg1.h>
#include <ffmpeg2.h>
#include <ffmpeg3.h>
#include <ffmpeg4.h>
#include <ffmpeg5.h>
#include <ffmpeg6.h>
#endif

#include "MainProcs.h"
#include "resource.h"
#include "LanguagePacks.hpp"

#include "Config.h"
#include "GameState.h"
#include "Renderer.h"
#include "Misc.h"
#include "lzma.h"

#include "Studio.h"
#include "Tutorials.h"
#include "LuaCode.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nCmdShow);
DWORD WINAPI GameThread(LPVOID lpParameter);

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;
HWND g_hWndBar = NULL;
HWND g_hWndGfx = NULL;
bool g_bGfxDestroyed = false;
TSQueue< MSG > g_MsgQueue; // Producer/consumer to hold events for our game thread

string GetProcessName() {
    char buffer[MAX_PATH] = {};
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string fullPath(buffer);
    size_t pos = fullPath.find_last_of("\\");
    return (pos != string::npos) ? fullPath.substr(pos + 1) : fullPath;
}

string ProgramPath() {
    char szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

string MultiInstanceGuardInit() {
    string Code = "";
    Code += "if _G.Working then\n";
    Code += "showMessage(\"Do not run multiple instances at once!\")\n";
    Code += "return\n";
    Code += "end\n";
    Code += "_G.Working=true\n";
    return Code;
}

string MultiInstanceGuardRelease() {
    return "_G.Working=false\n";
}

string FakeThreadingEngine() {
    string Code = "";
    Code += "Threads={}\n";
    Code += "function Spawn(Func)\n";
    Code += "local Thread=coroutine.create(Func)\n";
    Code += "table.insert(Threads,Thread)\n";
    Code += "coroutine.resume(Thread)\n";
    Code += "end\n";
    Code += "function Tick()\n";
    Code += "local ThreadIdx=1\n";
    Code += "while ThreadIdx<=#Threads do\n";
    Code += "if coroutine.status(Threads[ThreadIdx])==\"dead\" then\n";
    Code += "table.remove(Threads,ThreadIdx)\n";
    Code += "else\n";
    Code += "local Status,Details=coroutine.resume(Threads[ThreadIdx])\n";
    Code += "if not Status then error(debug.traceback(Threads[ThreadIdx],Details)) end\n";
    Code += "ThreadIdx=ThreadIdx+1\n";
    Code += "end\n";
    Code += "end\n";
    Code += "processMessages()\n";
    Code += "collectgarbage(\"collect\")\n";
    Code += "end\n";
    Code += "function Next()\n";
    Code += "coroutine.yield()\n";
    Code += "end\n";
    return Code;
}

string InitializeVariables() {
    string Code = "";
    Code += "MS=1e+3\n";
    Code += "S=1e+6\n";
    Code += "EXE=executeCodeLocalEx\n";
    Code += "hPFX=EXE(\"FindWindowA\",\"PFX\",0)\n";
    Code += "hGFX=EXE(\"FindWindowExA\",hPFX,0,\"PFXGFX\",0)\n";
    Code += "hGDI=EXE(\"FindWindowA\",\"PFXGDI\",0)\n";
    Code += "PFXdc=EXE(\"GetDC\",hGFX)\n";
    Code += "GDIdc=EXE(\"GetDC\",hGDI)\n";
    Code += "openProcess(getWindowProcessID(hPFX))\n";
    Code += "SRCCOPY=0x00CC0020\n";
    Code += "SRCPAINT=0x00EE0086\n";
    Code += "SRCAND=0x008800C6\n";
    Code += "SRCINVERT=0x00660046\n";
    Code += "SRCERASE=0x00440328\n";
    Code += "NOTSRCCOPY=0x00330008\n";
    Code += "NOTSRCERASE=0x001100A6\n";
    Code += "MERGECOPY=0x00C000CA\n";
    Code += "MERGEPAINT=0x00BB0226\n";
    Code += "PATCOPY=0x00F00021\n";
    Code += "PATPAINT=0x00FB0A09\n";
    Code += "PATINVERT=0x005A0049\n";
    Code += "DSTINVERT=0x00550009\n";
    Code += "BLACKNESS=0x00000042\n";
    Code += "WHITENESS=0x00FF0062\n";
    Code += "BS_SOLID=0\n";
    Code += "BS_NULL=1\n";
    Code += "BS_HOLLOW=BS_NULL\n";
    Code += "BS_HATCHED=2\n";
    Code += "BS_PATTERN=3\n";
    Code += "BS_INDEXED=4\n";
    Code += "BS_DIBPATTERN=5\n";
    Code += "BS_DIBPATTERNPT=6\n";
    Code += "BS_PATTERN8X8=7\n";
    Code += "BS_DIBPATTERN8X8=8\n";
    Code += "BS_MONOPATTERN=9\n";
    Code += "HS_HORIZONTAL=0\n";
    Code += "HS_VERTICAL=1\n";
    Code += "HS_FDIAGONAL=2\n";
    Code += "HS_BDIAGONAL=3\n";
    Code += "HS_CROSS=4\n";
    Code += "HS_DIAGCROSS=5\n";
    Code += "HS_API_MAX=12\n";
    Code += "PS_SOLID=0\n";
    Code += "PS_DASH=1\n";
    Code += "PS_DOT=2\n";
    Code += "PS_DASHDOT=3\n";
    Code += "PS_DASHDOTDOT=4\n";
    Code += "PS_NULL=5\n";
    Code += "PS_INSIDEFRAME=6\n";
    Code += "PS_USERSTYLE=7\n";
    Code += "PS_ALTERNATE=8\n";
    Code += "PS_STYLE_MASK=0x0000000F\n";
    Code += "PS_ENDCAP_ROUND=0x00000000\n";
    Code += "PS_ENDCAP_SQUARE=0x00000100\n";
    Code += "PS_ENDCAP_FLAT=0x00000200\n";
    Code += "PS_ENDCAP_MASK=0x00000F00\n";
    Code += "PS_JOIN_ROUND=0x00000000\n";
    Code += "PS_JOIN_BEVEL=0x00001000\n";
    Code += "PS_JOIN_MITER=0x00002000\n";
    Code += "PS_JOIN_MASK=0x0000F000\n";
    Code += "PS_COSMETIC=0x00000000\n";
    Code += "PS_GEOMETRIC=0x00010000\n";
    Code += "PS_TYPE_MASK=0x000F0000\n";
    Code += "ALIGN_LEFT=0x0100\n";
    Code += "ALIGN_CENTER=0x0200\n";
    Code += "ALIGN_RIGHT=0x0300\n";
    Code += "ALIGN_TOP=0x0001\n";
    Code += "ALIGN_MIDDLE=0x0002\n";
    Code += "ALIGN_BOTTOM=0x0003\n";
    Code += "WHITE_BRUSH=0\n";
    Code += "LTGRAY_BRUSH=1\n";
    Code += "GRAY_BRUSH=2\n";
    Code += "DKGRAY_BRUSH=3\n";
    Code += "BLACK_BRUSH=4\n";
    Code += "NULL_BRUSH=5\n";
    Code += "HOLLOW_BRUSH=NULL_BRUSH\n";
    Code += "WHITE_PEN=6\n";
    Code += "BLACK_PEN=7\n";
    Code += "NULL_PEN=8\n";
    return Code;
}

string DefineMemoryAddress(string VariableName, string Address) {
    string Code = "";
    Code += VariableName + "=\"" + GetProcessName() + "+" + Address + "\"\n";
    return Code;
}

string GettersAndSetters(string VariableName, string Type, bool IsReadOnly, string DebugList, bool IsSigned = false, bool NotePosUpdateRequired = false) {
    string Code = "";
    Code += "function Get" + VariableName + "()\n";
    Code += "return read" + Type + "(" + VariableName + string(IsSigned ? ",true" : "") + ")\n";
    Code += "end\n";
    if (!IsReadOnly) {
        Code += "function Set" + VariableName + "(VAL)\n";
        Code += DebugList + "Add(\"" + VariableName + " -> \"..VAL)\n";
        Code += "write" + Type + "(" + VariableName + ",VAL)\n";
        if (NotePosUpdateRequired) Code += "writeShortInteger(UpdateNotePos,1)\n";
        Code += "end\n";
    }
    return Code;
}

string StringTypeGettersAndSetters(string VariableName, size_t Size, bool IsReadOnly, string DebugList) {
    string Code = "";
    Code += "function Get" + VariableName + "()\n";
    Code += "return readString(" + VariableName + ")\n";
    Code += "end\n";
    if (!IsReadOnly) {
        Code += "function Set" + VariableName + "(VAL)\n";
        Code += DebugList + "Add(\"" + VariableName + " -> \\\"\"..VAL..\"\\\"\")\n";
        Code += "if #VAL<" + to_string(Size) + " then\n";
        Code += "writeShortInteger(getAddress(" + VariableName + ")+#VAL,0)\n";
        Code += "writeString(" + VariableName + ",VAL)\n";
        Code += "end\n";
        Code += "end\n";
    }
    return Code;
}

string SetResolutionFunction() {
    string Code = "";
    Code += "function SetResolution(W,H)\n";
    Code += "HistoryAdd(\"Resolution -> \"..W..\"*\"..H)\n";
    Code += "RectData=createMemoryStream()\n";
    Code += "RectData.size=16\n";
    Code += "RectData.Position=0\n";
    Code += "EXE(\"GetWindowRect\",hPFX,RectData.Memory)\n";
    Code += "RectData.Position=0\n";
    Code += "local Px=RectData.readDword()\n";
    Code += "local Py=RectData.readDword()\n";
    Code += "local Pw=RectData.readDword()-Px\n";
    Code += "local Ph=RectData.readDword()-Py\n";
    Code += "RectData.Position=0\n";
    Code += "EXE(\"GetWindowRect\",hGFX,RectData.Memory)\n";
    Code += "RectData.Position=0\n";
    Code += "local Gx=RectData.readDword()\n";
    Code += "local Gy=RectData.readDword()\n";
    Code += "local Gw=RectData.readDword()-Gx\n";
    Code += "local Gh=RectData.readDword()-Gy\n";
    Code += "local Mw=Pw-Gw\n";
    Code += "local Mh=Ph-Gh\n";
    Code += "RectData.destroy()\n";
    Code += "RectData=nil\n";
    Code += "EXE(\"SetWindowPos\",hPFX,-2,Px,Py,W+Mw,H+Mh,0)\n";
    Code += "end\n";
    return Code;
}

string CloseDebugWindow(string Name) {
    string Code = "";
    Code += "Debug" + Name + ".destroy()\n";
    return Code;
}

string CleanUp() {
    string Code = "";
    Code += "EXE(\"ReleaseDC\",hGFX,PFXdc)\n";
    Code += "EXE(\"ReleaseDC\",hGDI,GDIdc)\n";
    Code += CloseDebugWindow("Pending");
    Code += CloseDebugWindow("Active");
    Code += CloseDebugWindow("History");
    Code += "if hGDI~=0 then\n";
    Code += CloseDebugWindow("Shader");
    Code += "end\n";
    Code += "ShaderLookupTable=nil\n";
    Code += "ShaderLookupCounter=nil\n";
    Code += "collectgarbage(\"collect\")\n";
    Code += MultiInstanceGuardRelease();
    return Code;
}

string CreateDebugWindow(string Name, string X, string Y, string W, string H, bool Del = true, bool Add = true) {
    string Code = "";
    Code += "Debug" + Name + "=createForm()\n";
    Code += "Debug" + Name + ".Caption=\"Piano-FX Pro Animation Debug\"\n";
    Code += "Debug" + Name + ".PopupMode=0\n";
    Code += "Debug" + Name + ".BorderStyle=\"bsSizeable\"\n";
    Code += "Debug" + Name + ".Left=" + X + "\n";
    Code += "Debug" + Name + ".Top=" + Y + "\n";
    Code += "Debug" + Name + ".Width=" + W + "\n";
    Code += "Debug" + Name + ".Height=" + H + "\n";
    Code += "Debug" + Name + ".OnClose=function()\n";
    Code += "SetMicroseconds(GetSongLength())\n";
    Code += CleanUp();
    Code += "return caFree\n";
    Code += "end\n";
    Code += "Debug" + Name + ".show()\n";
    Code += "Debug" + Name + "List=createListBox(Debug" + Name + ")\n";
    Code += "Debug" + Name + "List.Align=\"alClient\"\n";
    Code += "Debug" + Name + "List.Items.add(\"" + Name + ":\")\n";
    Code += "Debug" + Name + "List.Items.add(\"\")\n";
    if (Add) {
        Code += "function " + Name + "Add(name)\n";
        Code += "Debug" + Name + "List.Items.insert(2,name)\n";
        Code += "return name\n";
        Code += "end\n";
    }
    if (Del) {
        Code += "function " + Name + "Del(name)\n";
        Code += "Debug" + Name + "List.Items.delete(Debug" + Name + "List.Items.indexOf(name))\n";
        Code += "return name\n";
        Code += "end\n";
    }
    return Code;
}

string EasingFunctions() {
    string Code = "";
    Code += "function EaseLinear(x)\n";
    Code += "return x\n";
    Code += "end\n";
    Code += "function EaseInSine(x)\n";
    Code += "return 1-math.cos((x*math.pi)/2)\n";
    Code += "end\n";
    Code += "function EaseOutSine(x)\n";
    Code += "return math.sin((x*math.pi)/2)\n";
    Code += "end\n";
    Code += "function EaseInOutSine(x)\n";
    Code += "return -(math.cos(math.pi*x)-1)/2\n";
    Code += "end\n";
    Code += "function EaseInQuad(x)\n";
    Code += "return x*x\n";
    Code += "end\n";
    Code += "function EaseOutQuad(x)\n";
    Code += "return 1-(1-x)*(1-x)\n";
    Code += "end\n";
    Code += "function EaseInOutQuad(x)\n";
    Code += "return x<0.5 and 2*x*x or 1-math.pow(-2*x+2,2)/2\n";
    Code += "end\n";
    Code += "function EaseInCubic(x)\n";
    Code += "return x*x*x\n";
    Code += "end\n";
    Code += "function EaseOutCubic(x)\n";
    Code += "return 1-math.pow(1-x,3)\n";
    Code += "end\n";
    Code += "function EaseInOutCubic(x)\n";
    Code += "return x<0.5 and 4*x*x*x or 1-math.pow(-2*x+2,3)/2\n";
    Code += "end\n";
    Code += "function EaseInQuart(x)\n";
    Code += "return x*x*x*x\n";
    Code += "end\n";
    Code += "function EaseOutQuart(x)\n";
    Code += "return 1-math.pow(1-x,4)\n";
    Code += "end\n";
    Code += "function EaseInOutQuart(x)\n";
    Code += "return x<0.5 and 8*x*x*x*x or 1-math.pow(-2*x+2,4)/2\n";
    Code += "end\n";
    Code += "function EaseInQuint(x)\n";
    Code += "return x*x*x*x*x\n";
    Code += "end\n";
    Code += "function EaseOutQuint(x)\n";
    Code += "return 1-math.pow(1-x,5)\n";
    Code += "end\n";
    Code += "function EaseInOutQuint(x)\n";
    Code += "return x<0.5 and 16*x*x*x*x*x or 1-math.pow(-2*x+2,5)/2\n";
    Code += "end\n";
    Code += "function EaseInExpo(x)\n";
    Code += "return x==0 and 0 or math.pow(2,10*x-10)\n";
    Code += "end\n";
    Code += "function EaseOutExpo(x)\n";
    Code += "return x==1 and 1 or 1-math.pow(2,-10*x)\n";
    Code += "end\n";
    Code += "function EaseInOutExpo(x)\n";
    Code += "return x==0 and 0 or x==1 and 1 or x<0.5 and math.pow(2,20*x-10)/2 or (2-math.pow(2,-20*x+10))/2\n";
    Code += "end\n";
    Code += "function EaseInCirc(x)\n";
    Code += "return 1-math.sqrt(1-math.pow(x,2))\n";
    Code += "end\n";
    Code += "function EaseOutCirc(x)\n";
    Code += "return math.sqrt(1-math.pow(x-1,2))\n";
    Code += "end\n";
    Code += "function EaseInOutCirc(x)\n";
    Code += "return x<0.5 and (1-math.sqrt(1-math.pow(2*x,2)))/2 or (math.sqrt(1-math.pow(-2*x+2,2))+1)/2\n";
    Code += "end\n";
    Code += "function EaseInBack(x)\n";
    Code += "local c1=1.70158\n";
    Code += "local c3=c1+1\n";
    Code += "return c3*x*x*x-c1*x*x\n";
    Code += "end\n";
    Code += "function EaseOutBack(x)\n";
    Code += "local c1=1.70158\n";
    Code += "local c3=c1+1\n";
    Code += "return 1+c3*math.pow(x-1,3)+c1*math.pow(x-1,2)\n";
    Code += "end\n";
    Code += "function EaseInOutBack(x)\n";
    Code += "local c1=1.70158\n";
    Code += "local c2=c1*1.525\n";
    Code += "return x<0.5 and (math.pow(2*x,2)*((c2+1)*2*x-c2))/2 or (math.pow(2*x-2,2)*((c2+1)*(x*2-2)+c2)+2)/2\n";
    Code += "end\n";
    Code += "function EaseInElastic(x)\n";
    Code += "local c4=(2*math.pi)/3\n";
    Code += "return x==0 and 0 or x==1 and 1 or -math.pow(2,10*x-10)*math.sin((x*10-10.75)*c4)\n";
    Code += "end\n";
    Code += "function EaseOutElastic(x)\n";
    Code += "local c4=(2*math.pi)/3\n";
    Code += "return x==0 and 0 or x==1 and 1 or math.pow(2,-10*x)*math.sin((x*10-0.75)*c4)+1\n";
    Code += "end\n";
    Code += "function EaseInOutElastic(x)\n";
    Code += "local c5=(2*math.pi)/4.5\n";
    Code += "return x==0 and 0 or x==1 and 1 or x<0.5 and -(math.pow(2,20*x-10)*math.sin((20*x-11.125)*c5))/2 or (math.pow(2,-20*x+10)*math.sin((20*x-11.125)*c5))/2+1\n";
    Code += "end\n";
    Code += "function EaseInBounce(x)\n";
    Code += "return 1-EaseOutBounce(1-x)\n";
    Code += "end\n";
    Code += "function EaseOutBounce(x)\n";
    Code += "local n1=7.5625\n";
    Code += "local d1=2.75\n";
    Code += "if x<1/d1 then\n";
    Code += "return n1*x*x\n";
    Code += "elseif x<2/d1 then\n";
    Code += "return n1*(x-1.5/d1)*(x-1.5/d1)+0.75\n";
    Code += "elseif x<2.5/d1 then\n";
    Code += "return n1*(x-2.25/d1)*(x-2.25/d1)+0.9375\n";
    Code += "else\n";
    Code += "return n1*(x-2.625/d1)*(x-2.625/d1)+0.984375\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function EaseInOutBounce(x)\n";
    Code += "return x<0.5 and (1-EaseOutBounce(1-2*x))/2 or (1+EaseOutBounce(2*x-1))/2\n";
    Code += "end\n";
    Code += "function CenterX(Zoom)\n";
    Code += "return (GetWidth()*(1-Zoom))/2\n";
    Code += "end\n";
    Code += "function CenterY(Zoom)\n";
    Code += "return -(GetHeight()*(1-Zoom))/2\n";
    Code += "end\n";
    Code += "function var2str(var)\n";
    Code += "for name,value in pairs(_G) do\n";
    Code += "if value==var then\n";
    Code += "return name\n";
    Code += "end\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function Incomplete()\n";
    Code += "return GetMicroseconds()<GetSongLength()\n";
    Code += "end\n";
    return Code;
}

string WaitUntil(bool TickBased, string DebugList) {
    string Code = "";
    Code += "function WaitUntil" + string(TickBased ? "Tick" : "Time") + "(TargetTime)\n";
    Code += "local DbgItem=" + DebugList + "Add(\"WaitUntil" + string(TickBased ? "Tick" : "Time") + "(\"..TargetTime..\")\")\n";
    Code += "while (Get" + string(TickBased ? "Ticks" : "Microseconds") + "()<TargetTime and Incomplete()) do Tick() end\n";
    Code += DebugList + "Del(DbgItem)\n";
    Code += "end\n";
    return Code;
}

string AnimationFunction(bool IsTarget, bool IsSliced, bool TickBased, bool Wait, string DebugList1, string DebugList2) {
    string Code = "";
    Code += "function " + string(IsSliced ? "Sliced" : "") + string(IsTarget ? "Target" : "Smooth") + "AnimationBy" + string(TickBased ? "Tick" : "Time") + string(Wait ? "AndWait" : "") + "(" + string(IsTarget ? "GetValueFunction" : "StartValue") + ",EndValue,StartTime,EndTime" + string(IsSliced ? ",Spacing" : "") + ",EasingFunction,SetValueFunction)\n";
    Code += Wait ? "" : "Spawn(function()\n";
    Code += "local DbgItem=" + DebugList1 + "Add(\"" + string(IsSliced ? "Sliced" : "") + string(IsTarget ? "Target" : "Smooth") + "AnimationBy" + string(TickBased ? "Tick" : "Time") + string(Wait ? "AndWait" : "") + "(\".." + string(IsTarget ? "var2str(GetValueFunction)" : "StartValue") + "..\",\"..EndValue..\",\"..StartTime..\",\"..EndTime.." + string(IsSliced ? "\",\"..Spacing.." : "") + "\",\"..var2str(EasingFunction)..\",\"..var2str(SetValueFunction)..\")\")\n";
    Code += "while (Get" + string(TickBased ? "Ticks" : "Microseconds") + "()<StartTime and Incomplete()) do " + string(Wait ? "Tick()" : "Next()") + " end\n";
    Code += IsTarget ? "local StartValue=GetValueFunction()\n" : "";
    Code += IsSliced ? "local SliceIndex=1\n" : "";
    Code += DebugList1 + "Del(DbgItem)\n";
    Code += "DbgItem=" + DebugList2 + "Add(" + string(IsTarget ? ("\"" + string(IsSliced ? "Sliced" : "") + "SmoothAnimationBy" + string(TickBased ? "Tick" : "Time") + string(Wait ? "AndWait" : "") + "(\"..StartValue..\",\"..EndValue..\",\"..StartTime..\",\"..EndTime.." + string(IsSliced ? "\",\"..Spacing.." : "") + "\",\"..var2str(EasingFunction)..\",\"..var2str(SetValueFunction)..\")\"") : "DbgItem") + ")\n";
    Code += "while (Get" + string(TickBased ? "Ticks" : "Microseconds") + "()<=EndTime and Incomplete()) do\n";
    Code += "SetValueFunction(StartValue+EasingFunction((Get" + string(TickBased ? "Ticks" : "Microseconds") + "()-StartTime)/(EndTime-StartTime))*(EndValue-StartValue))\n";
    Code += IsSliced ? "while (Get" + string(TickBased ? "Ticks" : "Microseconds") + "()<StartTime+Spacing*SliceIndex and Incomplete()) do " + string(Wait ? "Tick()" : "Next()") + " end\n" : "";
    Code += IsSliced ? "SliceIndex=SliceIndex+1\n" : "";
    Code += IsSliced ? "" : (Wait ? "Tick()\n" : "Next()\n");
    Code += "end\n";
    Code += "SetValueFunction(EndValue)\n";
    Code += DebugList2 + "Del(DbgItem)\n";
    Code += Wait ? "" : "end)\n";
    Code += "end\n";
    return Code;
}

string InstantaneousAnimationFunction(bool TickBased, bool Wait, string DebugList) {
    string Code = "";
    Code += "function InstantaneousAnimationBy" + string(TickBased ? "Tick" : "Time") + string(Wait ? "AndWait" : "") + "(Value,Time,SetValueFunction)\n";
    Code += Wait ? "" : "Spawn(function()\n";
    Code += "local DbgItem=" + DebugList + "Add(\"InstantaneousAnimationBy" + string(TickBased ? "Tick" : "Time") + string(Wait ? "AndWait" : "") + "(\"..Value..\",\"..Time..\",\"..var2str(SetValueFunction)..\")\")\n";
    Code += "while (Get" + string(TickBased ? "Ticks" : "Microseconds") + "()<Time and Incomplete()) do " + string(Wait ? "Tick()" : "Next()") + " end\n";
    Code += "SetValueFunction(Value)\n";
    Code += DebugList + "Del(DbgItem)\n";
    Code += Wait ? "" : "end)\n";
    Code += "end\n";
    return Code;
}

string RunShader(bool TickBased) {
    string Code = "";
    Code += "function RunShaderBy" + string(TickBased ? "Tick" : "Time") + "(StartTime,EndTime,ShaderFunction,...)\n";
    Code += "local EffectParameters={...}\n";
    Code += "if hGDI==0 then return end\n";
    Code += "ShaderLookupCounter=ShaderLookupCounter+1\n";
    Code += "ShaderLookupTable[ShaderLookupCounter]={Func=ShaderFunction,Param={TimingMethod=Get" + string(TickBased ? "Ticks" : "Microseconds") + ",StartTime=StartTime,EndTime=EndTime,EffectParameters=EffectParameters}}\n";
    Code += "DebugShaderList.Items.add(\"RunShaderBy" + string(TickBased ? "Tick" : "Time") + "(\"..StartTime..\",\"..EndTime..\",\"..var2str(ShaderFunction)..\",\"..table.concat(EffectParameters,\",\")..\")\",ShaderLookupCounter)\n";
    Code += "end\n";
    return Code;
}

string ShaderEngine() {
    string Code = "";
    Code += "LastUpdate=os.time()*S+os.clock()*S\n";
    Code += "ShaderLookupTable={}\n";
    Code += "ShaderLookupCounter=0\n";
    // Shader debug window
    Code += "if hGDI~=0 then\n";
    Code += CreateDebugWindow("Shader", "750", "0", "350", "500", false, false);
    Code += "end\n";
    Code += RunShader(false);
    Code += RunShader(true);
    // Shader thread
    Code += "ShaderWorking=false\n";
    Code += "function ShaderEngine()\n";
    Code += "if hGDI==0 then return end\n";
    Code += "ShaderWorking=true\n";
    Code += "while Incomplete() do\n";
    Code += "local W=GetWidth()\n";
    Code += "local H=GetHeight()\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",GDIdc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",GDIdc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"BitBlt\",MEMdc,0,0,W,H,PFXdc,0,0,SRCCOPY)\n";
    Code += "local ShaderID=2\n";
    Code += "if DebugShaderList.Items.Count>2 then\n";
    Code += "while ShaderID<DebugShaderList.Items.Count do\n";
    Code += "local ShaderEntry=ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]\n";
    Code += "while ShaderEntry.Param.TimingMethod()>ShaderEntry.Param.EndTime do\n";
    Code += "ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]=nil\n";
    Code += "DebugShaderList.Items.delete(ShaderID)\n";
    Code += "if ShaderID>=DebugShaderList.Items.Count then goto Finish end\n";
    Code += "ShaderEntry=ShaderLookupTable[DebugShaderList.Items.getData(ShaderID)]\n";
    Code += "end\n";
    Code += "if ShaderEntry.Param.TimingMethod()<ShaderEntry.Param.StartTime then goto Skip end\n";
    Code += "ShaderEntry.Func(MEMdc,(ShaderEntry.Param.TimingMethod()-ShaderEntry.Param.StartTime)/(ShaderEntry.Param.EndTime-ShaderEntry.Param.StartTime),W,H,table.unpack(ShaderEntry.Param.EffectParameters))\n";
    Code += "::Skip::\n";
    Code += "ShaderID=ShaderID+1\n";
    Code += "end\n";
    Code += "::Finish::\n";
    Code += "end\n";
    // Show FPS
    Code += "if DisplayShaderFPS then\n";
    Code += "local FrameTime=(os.time()*S+os.clock()*S)-LastUpdate+1\n";
    Code += "local FPS=math.ceil((S/FrameTime)*100.0)/100.0\n";
    Code += "local Text=\"Shader FPS: \"..FPS\n";
    Code += "local Font=EXE(\"CreateFontA\",\n";
    Code += "16,-- FontHeight\n";
    Code += "0,-- FontWidth\n";
    Code += "0,-- Escapement\n";
    Code += "0,-- Orientation\n";
    Code += "400,-- FontWeight\n";
    Code += "0,-- Italic\n";
    Code += "0,-- Underline\n";
    Code += "0,-- StrikeOut\n";
    Code += "1,-- CharSet\n";
    Code += "0,-- OutPrecision\n";
    Code += "0,-- ClipPrecision\n";
    Code += "0,-- Quality\n";
    Code += "0,-- PitchAndFamily\n";
    Code += "\"Fixedsys\")-- FaceName\n";
    Code += "EXE(\"SelectObject\",MEMdc,Font)\n";
    Code += "EXE(\"SetBkMode\",MEMdc,1)\n";
    Code += "EXE(\"SetTextColor\",MEMdc,0x000000)\n";
    Code += "EXE(\"TextOutA\",MEMdc,16,16,Text,#Text)\n";
    Code += "EXE(\"SetTextColor\",MEMdc,0xFFFFFF)\n";
    Code += "EXE(\"TextOutA\",MEMdc,15,15,Text,#Text)\n";
    Code += "EXE(\"DeleteObject\",Font)\n";
    Code += "end\n";
    Code += "LastUpdate=os.time()*S+os.clock()*S\n";
    // Present!
    Code += "EXE(\"BitBlt\",GDIdc,0,0,W,H,MEMdc,0,0,SRCCOPY)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "Next()\n";
    Code += "end\n";
    Code += "ShaderWorking=false\n";
    Code += "end\n";
    return Code;
}

string BuiltinShaders() {
    string Code = "";
    Code += "function DarkBlankShader(dc,t,W,H)\n";
    Code += "EXE(\"PatBlt\",dc,0,0,W,H,BLACKNESS)\n";
    Code += "end\n";
    Code += "function BrightBlankShader(dc,t,W,H)\n";
    Code += "EXE(\"PatBlt\",dc,0,0,W,H,WHITENESS)\n";
    Code += "end\n";
    Code += "function WaveShader(dc,t,W,H,MaxWaveHeight,MinWaveHeight,ShiftingAmount)\n";
    Code += "if MaxWaveHeight==nil then MaxWaveHeight=10 end\n";
    Code += "if MinWaveHeight==nil then MinWaveHeight=5 end\n";
    Code += "if ShiftingAmount==nil then ShiftingAmount=10 end\n";
    Code += "local Line=0\n";
    Code += "while Line<=H do\n";
    Code += "EXE(\"BitBlt\",dc,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,dc,0,Line,SRCCOPY)\n";
    Code += "Line=Line+math.random(MinWaveHeight,MaxWaveHeight)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function WaveGlitchShader(dc,t,W,H,MaxWaveHeight,MinWaveHeight,ShiftingAmount)\n";
    Code += "if MaxWaveHeight==nil then MaxWaveHeight=10 end\n";
    Code += "if MinWaveHeight==nil then MinWaveHeight=5 end\n";
    Code += "if ShiftingAmount==nil then ShiftingAmount=10 end\n";
    Code += "local Line=0\n";
    Code += "while Line<=H do\n";
    Code += "if math.random(0,1)==0 then\n";
    Code += "EXE(\"BitBlt\",dc,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,dc,0,Line,SRCPAINT)\n";
    Code += "else\n";
    Code += "EXE(\"BitBlt\",dc,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,dc,0,Line,SRCAND)\n";
    Code += "end\n";
    Code += "Line=Line+math.random(MinWaveHeight,MaxWaveHeight)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function GlitchShader(dc,t,W,H,MaxSectionWidth,MinSectionWidth,ShiftingAmount)\n";
    Code += "if MaxSectionWidth==nil then MaxSectionWidth=150 end\n";
    Code += "if MinSectionWidth==nil then MinSectionWidth=10 end\n";
    Code += "if ShiftingAmount==nil then ShiftingAmount=10 end\n";
    Code += "local Line=0\n";
    Code += "while Line<=W do\n";
    Code += "if math.random(0,1)==0 then\n";
    Code += "EXE(\"BitBlt\",dc,Line,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),MaxSectionWidth,H,dc,Line,0,SRCPAINT)\n";
    Code += "else\n";
    Code += "EXE(\"BitBlt\",dc,Line,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),MaxSectionWidth,H,dc,Line,0,SRCAND)\n";
    Code += "end\n";
    Code += "Line=Line+math.random(MinSectionWidth,MaxSectionWidth)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function ColorBurnShader(dc,t,W,H,R,G,B)\n";
    Code += "if R==nil then R=0 end\n";
    Code += "if G==nil then G=255 end\n";
    Code += "if B==nil then B=0 end\n";
    Code += "local Color=(B<<16)|(G<<8)|R\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",Color)\n";
    Code += "EXE(\"SelectObject\",dc,Brush)\n";
    Code += "EXE(\"PatBlt\",dc,0,0,W,H,PATINVERT)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "end\n";
    Code += "function ColorOffsetShader(dc,t,W,H,OffsettingAmount)\n";
    Code += "if OffsettingAmount==nil then OffsettingAmount=5 end\n";
    Code += "local W=W\n";
    Code += "local H=H\n";
    Code += "local MEMdc_R=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_R=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_G=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_G=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_B=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_B=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_Main=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_Main=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,BMP_R)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x0000FF)\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_R,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_R,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,BMP_G)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x00FF00)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_G,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_G,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,BMP_B)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0xFF0000)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_B,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_B,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_Main,BMP_Main)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),W,H,MEMdc_R,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),W,H,MEMdc_G,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount)),W,H,MEMdc_B,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",dc,0,0,W,H,MEMdc_Main,0,0,SRCCOPY)\n";
    Code += "EXE(\"DeleteObject\",BMP_R)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_R)\n";
    Code += "EXE(\"DeleteObject\",BMP_G)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_G)\n";
    Code += "EXE(\"DeleteObject\",BMP_B)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_B)\n";
    Code += "EXE(\"DeleteObject\",BMP_Main)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_Main)\n";
    Code += "end\n";
    Code += "function ColorOffsetWaveShader(dc,t,W,H,OffsettingAmount,MaxWaveHeight,MinWaveHeight,ShiftingAmount)\n";
    Code += "if OffsettingAmount==nil then OffsettingAmount=5 end\n";
    Code += "if MaxWaveHeight==nil then MaxWaveHeight=40 end\n";
    Code += "if MinWaveHeight==nil then MinWaveHeight=10 end\n";
    Code += "if ShiftingAmount==nil then ShiftingAmount=10 end\n";
    Code += "local MEMdc_R=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_R=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_G=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_G=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_B=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_B=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_Main=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_Main=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,BMP_R)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x0000FF)\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_R,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_R,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,BMP_G)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x00FF00)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_G,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_G,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,BMP_B)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0xFF0000)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_B,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_B,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_Main,BMP_Main)\n";
    Code += "local Line=0\n";
    Code += "local OffsetRX=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "local OffsetRY=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "local OffsetGX=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "local OffsetGY=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "local OffsetBX=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "local OffsetBY=math.random(-math.abs(OffsettingAmount),math.abs(OffsettingAmount))\n";
    Code += "while Line<=H do\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,MEMdc_R,OffsetRX,Line+OffsetRY,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,MEMdc_G,OffsetGX,Line+OffsetGY,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,math.random(-math.abs(ShiftingAmount),math.abs(ShiftingAmount)),Line,W,MaxWaveHeight,MEMdc_B,OffsetBX,Line+OffsetBY,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",dc,0,Line,W,MaxWaveHeight,MEMdc_Main,0,Line,SRCCOPY)\n";
    Code += "Line=Line+math.random(MinWaveHeight,MaxWaveHeight)\n";
    Code += "end\n";
    Code += "EXE(\"DeleteObject\",BMP_R)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_R)\n";
    Code += "EXE(\"DeleteObject\",BMP_G)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_G)\n";
    Code += "EXE(\"DeleteObject\",BMP_B)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_B)\n";
    Code += "EXE(\"DeleteObject\",BMP_Main)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_Main)\n";
    Code += "end\n";
    Code += "function LightenFromDarkShader(dc,t,W,H,MaxAlpha,MinAlpha)\n";
    Code += "if MaxAlpha==nil then MaxAlpha=255.0 end\n";
    Code += "if MinAlpha==nil then MinAlpha=0.0 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"PatBlt\",MEMdc,0,0,W,H,BLACKNESS)\n";
    Code += "local Alpha=math.ceil((MaxAlpha-MinAlpha)*(1.0-t)+MinAlpha)\n";
    Code += "local BlendFunc=(Alpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function DarkenFromLightShader(dc,t,W,H,MaxAlpha,MinAlpha)\n";
    Code += "if MaxAlpha==nil then MaxAlpha=255.0 end\n";
    Code += "if MinAlpha==nil then MinAlpha=0.0 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"PatBlt\",MEMdc,0,0,W,H,WHITENESS)\n";
    Code += "local Alpha=math.ceil((MaxAlpha-MinAlpha)*(1.0-t)+MinAlpha)\n";
    Code += "local BlendFunc=(Alpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function DarkenShader(dc,t,W,H,MaxAlpha,MinAlpha)\n";
    Code += "if MaxAlpha==nil then MaxAlpha=255.0 end\n";
    Code += "if MinAlpha==nil then MinAlpha=0.0 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"PatBlt\",MEMdc,0,0,W,H,BLACKNESS)\n";
    Code += "local Alpha=math.ceil((MaxAlpha-MinAlpha)*t+MinAlpha)\n";
    Code += "local BlendFunc=(Alpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function LightenShader(dc,t,W,H,MaxAlpha,MinAlpha)\n";
    Code += "if MaxAlpha==nil then MaxAlpha=255.0 end\n";
    Code += "if MinAlpha==nil then MinAlpha=0.0 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"PatBlt\",MEMdc,0,0,W,H,WHITENESS)\n";
    Code += "local Alpha=math.ceil((MaxAlpha-MinAlpha)*t+MinAlpha)\n";
    Code += "local BlendFunc=(Alpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function HatchBrushShader(dc,t,W,H,R,G,B,BkR,BkG,BkB)\n";
    Code += "if R==nil then R=0 end\n";
    Code += "if G==nil then G=255 end\n";
    Code += "if B==nil then B=0 end\n";
    Code += "if BkR==nil then BkR=255 end\n";
    Code += "if BkG==nil then BkG=0 end\n";
    Code += "if BkB==nil then BkB=0 end\n";
    Code += "local Color=(B<<16)|(G<<8)|R\n";
    Code += "local Brush=EXE(\"CreateHatchBrush\",math.random(0,5),Color)\n";
    Code += "EXE(\"SelectObject\",dc,Brush)\n";
    Code += "local BkColor=(BkB<<16)|(BkG<<8)|BkR\n";
    Code += "EXE(\"SetBKColor\",dc,BkColor)\n";
    Code += "EXE(\"PatBlt\",dc,0,0,W,H,PATINVERT)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "end\n";
    Code += "function TextShader(dc,t,W,H,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "if Text==nil then Text=\"Hello World\" end\n";
    Code += "if FontHeight==nil then FontHeight=50 end\n";
    Code += "if FontWidth==nil then FontWidth=30 end\n";
    Code += "if FontWeight==nil then FontWeight=400 end\n";
    Code += "if FaceName==nil then FaceName=\"Fixedsys\" end\n";
    Code += "if Alignment==nil then Alignment=ALIGN_LEFT|ALIGN_TOP end\n";
    Code += "if Italic==nil then Italic=0 end\n";
    Code += "if Underline==nil then Underline=0 end\n";
    Code += "if StrikeOut==nil then StrikeOut=0 end\n";
    Code += "if PosX==nil then PosX=0 end\n";
    Code += "if PosY==nil then PosY=0 end\n";
    Code += "if NoBackground==nil then NoBackground=false end\n";
    Code += "if TextR==nil then TextR=255 end\n";
    Code += "if TextG==nil then TextG=255 end\n";
    Code += "if TextB==nil then TextB=255 end\n";
    Code += "if BkR==nil then BkR=255 end\n";
    Code += "if BkG==nil then BkG=0 end\n";
    Code += "if BkB==nil then BkB=0 end\n";
    Code += "TextShaderBase(dc,#Text,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "end\n";
    Code += "function TypeTextShader(dc,t,W,H,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "if Text==nil then Text=\"Hello World\" end\n";
    Code += "if FontHeight==nil then FontHeight=50 end\n";
    Code += "if FontWidth==nil then FontWidth=30 end\n";
    Code += "if FontWeight==nil then FontWeight=400 end\n";
    Code += "if FaceName==nil then FaceName=\"Fixedsys\" end\n";
    Code += "if Alignment==nil then Alignment=ALIGN_LEFT|ALIGN_TOP end\n";
    Code += "if Italic==nil then Italic=0 end\n";
    Code += "if Underline==nil then Underline=0 end\n";
    Code += "if StrikeOut==nil then StrikeOut=0 end\n";
    Code += "if PosX==nil then PosX=0 end\n";
    Code += "if PosY==nil then PosY=0 end\n";
    Code += "if NoBackground==nil then NoBackground=false end\n";
    Code += "if TextR==nil then TextR=255 end\n";
    Code += "if TextG==nil then TextG=255 end\n";
    Code += "if TextB==nil then TextB=255 end\n";
    Code += "if BkR==nil then BkR=255 end\n";
    Code += "if BkG==nil then BkG=0 end\n";
    Code += "if BkB==nil then BkB=0 end\n";
    Code += "TextShaderBase(dc,math.ceil(#Text*t),Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "end\n";
    Code += "function DeleteTextShader(dc,t,W,H,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "if Text==nil then Text=\"Hello World\" end\n";
    Code += "if FontHeight==nil then FontHeight=50 end\n";
    Code += "if FontWidth==nil then FontWidth=30 end\n";
    Code += "if FontWeight==nil then FontWeight=400 end\n";
    Code += "if FaceName==nil then FaceName=\"Fixedsys\" end\n";
    Code += "if Alignment==nil then Alignment=ALIGN_LEFT|ALIGN_TOP end\n";
    Code += "if Italic==nil then Italic=0 end\n";
    Code += "if Underline==nil then Underline=0 end\n";
    Code += "if StrikeOut==nil then StrikeOut=0 end\n";
    Code += "if PosX==nil then PosX=0 end\n";
    Code += "if PosY==nil then PosY=0 end\n";
    Code += "if NoBackground==nil then NoBackground=false end\n";
    Code += "if TextR==nil then TextR=255 end\n";
    Code += "if TextG==nil then TextG=255 end\n";
    Code += "if TextB==nil then TextB=255 end\n";
    Code += "if BkR==nil then BkR=255 end\n";
    Code += "if BkG==nil then BkG=0 end\n";
    Code += "if BkB==nil then BkB=0 end\n";
    Code += "TextShaderBase(dc,math.ceil(#Text*(1.0-t)),Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "end\n";
    Code += "function TextShaderBase(dc,Length,Text,FontHeight,FontWidth,FontWeight,FaceName,Alignment,Italic,Underline,StrikeOut,PosX,PosY,NoBackground,TextR,TextG,TextB,BkR,BkG,BkB)\n";
    Code += "local Color=(TextB<<16)|(TextG<<8)|TextR\n";
    Code += "EXE(\"SetTextColor\",dc,Color)\n";
    Code += "if NoBackground then\n";
    Code += "EXE(\"SetBkMode\",dc,1)\n";
    Code += "else\n";
    Code += "EXE(\"SetBkMode\",dc,2)\n";
    Code += "end\n";
    Code += "local BkColor=(BkB<<16)|(BkG<<8)|BkR\n";
    Code += "EXE(\"SetBKColor\",dc,BkColor)\n";
    Code += "local Font=EXE(\"CreateFontA\",\n";
    Code += "FontHeight,-- FontHeight\n";
    Code += "FontWidth,-- FontWidth\n";
    Code += "0,-- Escapement\n";
    Code += "0,-- Orientation\n";
    Code += "FontWeight,-- FontWeight\n";
    Code += "Italic,-- Italic\n";
    Code += "Underline,-- Underline\n";
    Code += "StrikeOut,-- StrikeOut\n";
    Code += "1,-- CharSet\n";
    Code += "0,-- OutPrecision\n";
    Code += "0,-- ClipPrecision\n";
    Code += "0,-- Quality\n";
    Code += "0,-- PitchAndFamily\n";
    Code += "FaceName)-- FaceName\n";
    Code += "EXE(\"SelectObject\",dc,Font)\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=8\n";
    Code += "EXE(\"GetTextExtentPoint32A\",dc,Text,Length,MEM.Memory)\n";
    Code += "MEM.Position=0\n";
    Code += "local W=MEM.readDword()\n";
    Code += "local H=MEM.readDword()\n";
    Code += "MEM.destroy()\n";
    Code += "MEM=nil\n";
    Code += "local X=math.floor(PosX)\n";
    Code += "local Y=math.floor(PosY)\n";
    Code += "if Alignment==0x0101 then\n";
    Code += "X=math.floor(PosX)\n";
    Code += "Y=math.floor(PosY)\n";
    Code += "end\n";
    Code += "if Alignment==0x0102 then\n";
    Code += "X=math.floor(PosX)\n";
    Code += "Y=math.floor(PosY-H/2)\n";
    Code += "end\n";
    Code += "if Alignment==0x0103 then\n";
    Code += "X=math.floor(PosX)\n";
    Code += "Y=math.floor(PosY-H)\n";
    Code += "end\n";
    Code += "if Alignment==0x0201 then\n";
    Code += "X=math.floor(PosX-W/2)\n";
    Code += "Y=math.floor(PosY)\n";
    Code += "end\n";
    Code += "if Alignment==0x0202 then\n";
    Code += "X=math.floor(PosX-W/2)\n";
    Code += "Y=math.floor(PosY-H/2)\n";
    Code += "end\n";
    Code += "if Alignment==0x0203 then\n";
    Code += "X=math.floor(PosX-W/2)\n";
    Code += "Y=math.floor(PosY-H)\n";
    Code += "end\n";
    Code += "if Alignment==0x0301 then\n";
    Code += "X=math.floor(PosX-W)\n";
    Code += "Y=math.floor(PosY)\n";
    Code += "end\n";
    Code += "if Alignment==0x0302 then\n";
    Code += "X=math.floor(PosX-W)\n";
    Code += "Y=math.floor(PosY-H/2)\n";
    Code += "end\n";
    Code += "if Alignment==0x0303 then\n";
    Code += "X=math.floor(PosX-W)\n";
    Code += "Y=math.floor(PosY-H)\n";
    Code += "end\n";
    Code += "EXE(\"TextOutA\",dc,X,Y,Text,Length)\n";
    Code += "EXE(\"DeleteObject\",Font)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function RotatePoint(X,Y,Angle,OriginX,OriginY)\n";
    Code += "return OriginX+(X-OriginX)*math.cos(Angle)-(Y-OriginY)*math.sin(Angle),OriginY+(X-OriginX)*math.sin(Angle)+(Y-OriginY)*math.cos(Angle)\n";
    Code += "end\n";
    Code += "function RadialBlurShader(dc,t,W,H,MaxRotation,BlendAlpha,Iteration)\n";
    Code += "if MaxRotation==nil then MaxRotation=0.02 end\n";
    Code += "if BlendAlpha==nil then BlendAlpha=1<<6 end\n";
    Code += "if Iteration==nil then Iteration=3 end\n";
    Code += "for Current=1,Iteration do\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "local Rotation=math.random(-math.abs(MaxRotation*1000.0),math.abs(MaxRotation*1000.0))/1000.0\n";
    Code += "local TopLeftX,TopLeftY=RotatePoint(0,0,Rotation,W/2,H/2)\n";
    Code += "local TopRightX,TopRightY=RotatePoint(W,0,Rotation,W/2,H/2)\n";
    Code += "local BottomLeftX,BottomLeftY=RotatePoint(0,H,Rotation,W/2,H/2)\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=24\n";
    Code += "MEM.writeDword(math.floor(TopLeftX))\n";
    Code += "MEM.writeDword(math.floor(TopLeftY))\n";
    Code += "MEM.writeDword(math.floor(TopRightX))\n";
    Code += "MEM.writeDword(math.floor(TopRightY))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftX))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftY))\n";
    Code += "MEM.Position=0\n";
    Code += "EXE(\"PlgBlt\",MEMdc,MEM.Memory,dc,0,0,W,H,0,0,0)\n";
    Code += "MEM.destroy()\n";
    Code += "local BlendFunc=(BlendAlpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function RadialPaintShader(dc,t,W,H,MaxRotation,Iteration)\n";
    Code += "if MaxRotation==nil then MaxRotation=0.02 end\n";
    Code += "if BlendAlpha==nil then BlendAlpha=1<<6 end\n";
    Code += "if Iteration==nil then Iteration=3 end\n";
    Code += "for Current=1,Iteration do\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "local Rotation=math.random(-math.abs(MaxRotation*1000.0),math.abs(MaxRotation*1000.0))/1000.0\n";
    Code += "local TopLeftX,TopLeftY=RotatePoint(0,0,Rotation,W/2,H/2)\n";
    Code += "local TopRightX,TopRightY=RotatePoint(W,0,Rotation,W/2,H/2)\n";
    Code += "local BottomLeftX,BottomLeftY=RotatePoint(0,H,Rotation,W/2,H/2)\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=24\n";
    Code += "MEM.writeDword(math.floor(TopLeftX))\n";
    Code += "MEM.writeDword(math.floor(TopLeftY))\n";
    Code += "MEM.writeDword(math.floor(TopRightX))\n";
    Code += "MEM.writeDword(math.floor(TopRightY))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftX))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftY))\n";
    Code += "MEM.Position=0\n";
    Code += "EXE(\"PlgBlt\",MEMdc,MEM.Memory,dc,0,0,W,H,0,0,0)\n";
    Code += "MEM.destroy()\n";
    Code += "EXE(\"BitBlt\",dc,0,0,W,H,MEMdc,0,0,SRCPAINT)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function RadialAndShader(dc,t,W,H,MaxRotation,Iteration)\n";
    Code += "if MaxRotation==nil then MaxRotation=0.02 end\n";
    Code += "if BlendAlpha==nil then BlendAlpha=1<<6 end\n";
    Code += "if Iteration==nil then Iteration=3 end\n";
    Code += "for Current=1,Iteration do\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "local Rotation=math.random(-math.abs(MaxRotation*1000.0),math.abs(MaxRotation*1000.0))/1000.0\n";
    Code += "local TopLeftX,TopLeftY=RotatePoint(0,0,Rotation,W/2,H/2)\n";
    Code += "local TopRightX,TopRightY=RotatePoint(W,0,Rotation,W/2,H/2)\n";
    Code += "local BottomLeftX,BottomLeftY=RotatePoint(0,H,Rotation,W/2,H/2)\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=24\n";
    Code += "MEM.writeDword(math.floor(TopLeftX))\n";
    Code += "MEM.writeDword(math.floor(TopLeftY))\n";
    Code += "MEM.writeDword(math.floor(TopRightX))\n";
    Code += "MEM.writeDword(math.floor(TopRightY))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftX))\n";
    Code += "MEM.writeDword(math.floor(BottomLeftY))\n";
    Code += "MEM.Position=0\n";
    Code += "EXE(\"PlgBlt\",MEMdc,MEM.Memory,dc,0,0,W,H,0,0,0)\n";
    Code += "MEM.destroy()\n";
    Code += "EXE(\"BitBlt\",dc,0,0,W,H,MEMdc,0,0,SRCAND)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function InvertPolygonShader(dc,t,W,H,MaxSides,MinSides,MaxRadius,MinRadius,MaxCount,MinCount,LineThickness,Delay)\n";
    Code += "if MaxSides==nil then MaxSides=10 end\n";
    Code += "if MinSides==nil then MinSides=3 end\n";
    Code += "if MaxRadius==nil then MaxRadius=400 end\n";
    Code += "if MinRadius==nil then MinRadius=100 end\n";
    Code += "if MaxCount==nil then MaxCount=5 end\n";
    Code += "if MinCount==nil then MinCount=1 end\n";
    Code += "if LineThickness==nil then LineThickness=50 end\n";
    Code += "if Delay==nil then Delay=4 end\n";
    Code += "local Count=math.random(MinCount,MaxCount)\n";
    Code += "if Polygons==nil then Polygons={} end\n";
    Code += "if FrameCount==nil then FrameCount=0 end\n";
    Code += "if FrameCount==0 then\n";
    Code += "for Shape=1,Count do\n";
    Code += "local Sides=math.random(MinSides,MaxSides)\n";
    Code += "local Radius=math.random(MinRadius,MaxRadius)\n";
    Code += "local X=math.random(0,W)\n";
    Code += "local Y=math.random(0,H)\n";
    Code += "local Rotation=math.random()*(2*math.pi)\n";
    Code += "local AngleStep=2*math.pi/Sides\n";
    Code += "while Radius>0 do\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=0\n";
    Code += "for ThisSide=0,Sides-1 do\n";
    Code += "MEM.size=MEM.size+8\n";
    Code += "MEM.writeDword(X+math.floor(Radius*math.cos(ThisSide*AngleStep+Rotation)))\n";
    Code += "MEM.writeDword(Y+math.floor(Radius*math.sin(ThisSide*AngleStep+Rotation)))\n";
    Code += "end\n";
    Code += "MEM.Position=0\n";
    Code += "table.insert(Polygons,EXE(\"CreatePolygonRgn\",MEM.Memory,Sides,2))\n";
    Code += "MEM.destroy()\n";
    Code += "Radius=Radius-LineThickness\n";
    Code += "end\n";
    Code += "end\n";
    Code += "end\n";;
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"InvertRgn\",dc,ThisPolygon)\n";
    Code += "end\n";
    Code += "FrameCount=FrameCount+1\n";
    Code += "if FrameCount>=Delay then\n";
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"DeleteObject\",ThisPolygon)\n";
    Code += "end\n";
    Code += "FrameCount=0\n";
    Code += "Polygons={}\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function ColorPolygonShader(dc,t,W,H,MaxSides,MinSides,MaxRadius,MinRadius,MaxCount,MinCount,LineThickness,Delay,R,G,B)\n";
    Code += "if MaxSides==nil then MaxSides=10 end\n";
    Code += "if MinSides==nil then MinSides=3 end\n";
    Code += "if MaxRadius==nil then MaxRadius=400 end\n";
    Code += "if MinRadius==nil then MinRadius=100 end\n";
    Code += "if MaxCount==nil then MaxCount=5 end\n";
    Code += "if MinCount==nil then MinCount=1 end\n";
    Code += "if LineThickness==nil then LineThickness=50 end\n";
    Code += "if Delay==nil then Delay=4 end\n";
    Code += "if R==nil then R=0 end\n";
    Code += "if G==nil then G=255 end\n";
    Code += "if B==nil then B=0 end\n";
    Code += "local Count=math.random(MinCount,MaxCount)\n";
    Code += "if Polygons==nil then Polygons={} end\n";
    Code += "if FrameCount==nil then FrameCount=0 end\n";
    Code += "if FrameCount==0 then\n";
    Code += "for Shape=1,Count do\n";
    Code += "local Sides=math.random(MinSides,MaxSides)\n";
    Code += "local Radius=math.random(MinRadius,MaxRadius)\n";
    Code += "local X=math.random(0,W)\n";
    Code += "local Y=math.random(0,H)\n";
    Code += "local Rotation=math.random()*(2*math.pi)\n";
    Code += "local AngleStep=2*math.pi/Sides\n";
    Code += "while Radius>0 do\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=0\n";
    Code += "for ThisSide=0,Sides-1 do\n";
    Code += "MEM.size=MEM.size+8\n";
    Code += "MEM.writeDword(X+math.floor(Radius*math.cos(ThisSide*AngleStep+Rotation)))\n";
    Code += "MEM.writeDword(Y+math.floor(Radius*math.sin(ThisSide*AngleStep+Rotation)))\n";
    Code += "end\n";
    Code += "MEM.Position=0\n";
    Code += "table.insert(Polygons,EXE(\"CreatePolygonRgn\",MEM.Memory,Sides,2))\n";
    Code += "MEM.destroy()\n";
    Code += "Radius=Radius-LineThickness\n";
    Code += "end\n";
    Code += "end\n";
    Code += "end\n";;
    Code += "local Color=(B<<16)|(G<<8)|R\n";
    Code += "local Pen=EXE(\"CreatePen\",PS_SOLID,0,Color)\n";
    Code += "EXE(\"SelectObject\",dc,Pen)\n";
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"FrameRgn\",dc,ThisPolygon,Pen,{type=0,value=LineThickness/4},{type=0,value=LineThickness/4})\n";
    Code += "end\n";
    Code += "EXE(\"DeleteObject\",Pen)\n";
    Code += "FrameCount=FrameCount+1\n";
    Code += "if FrameCount>=Delay then\n";
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"DeleteObject\",ThisPolygon)\n";
    Code += "end\n";
    Code += "FrameCount=0\n";
    Code += "Polygons={}\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function RandomColorPolygonShader(dc,t,W,H,MaxSides,MinSides,MaxRadius,MinRadius,MaxCount,MinCount,LineThickness,Delay)\n";
    Code += "if MaxSides==nil then MaxSides=10 end\n";
    Code += "if MinSides==nil then MinSides=3 end\n";
    Code += "if MaxRadius==nil then MaxRadius=400 end\n";
    Code += "if MinRadius==nil then MinRadius=100 end\n";
    Code += "if MaxCount==nil then MaxCount=5 end\n";
    Code += "if MinCount==nil then MinCount=1 end\n";
    Code += "if LineThickness==nil then LineThickness=50 end\n";
    Code += "if Delay==nil then Delay=4 end\n";
    Code += "local Count=math.random(MinCount,MaxCount)\n";
    Code += "if Polygons==nil then Polygons={} end\n";
    Code += "if Colors==nil then Colors={} end\n";
    Code += "if FrameCount==nil then FrameCount=0 end\n";
    Code += "if FrameCount==0 then\n";
    Code += "for Shape=1,Count do\n";
    Code += "local Sides=math.random(MinSides,MaxSides)\n";
    Code += "local Radius=math.random(MinRadius,MaxRadius)\n";
    Code += "local X=math.random(0,W)\n";
    Code += "local Y=math.random(0,H)\n";
    Code += "local Rotation=math.random()*(2*math.pi)\n";
    Code += "local AngleStep=2*math.pi/Sides\n";
    Code += "local R=math.random(0,255)\n";
    Code += "local G=math.random(0,255)\n";
    Code += "local B=math.random(0,255)\n";
    Code += "while Radius>0 do\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=0\n";
    Code += "for ThisSide=0,Sides-1 do\n";
    Code += "MEM.size=MEM.size+8\n";
    Code += "MEM.writeDword(X+math.floor(Radius*math.cos(ThisSide*AngleStep+Rotation)))\n";
    Code += "MEM.writeDword(Y+math.floor(Radius*math.sin(ThisSide*AngleStep+Rotation)))\n";
    Code += "end\n";
    Code += "MEM.Position=0\n";
    Code += "table.insert(Polygons,EXE(\"CreatePolygonRgn\",MEM.Memory,Sides,2))\n";
    Code += "MEM.destroy()\n";
    Code += "table.insert(Colors,(B<<16)|(G<<8)|R)\n";
    Code += "Radius=Radius-LineThickness\n";
    Code += "end\n";
    Code += "end\n";
    Code += "end\n";;
    Code += "local Color=(B<<16)|(G<<8)|R\n";
    Code += "for PolygonID,ThisPolygon in pairs(Polygons) do\n";
    Code += "local Pen=EXE(\"CreatePen\",PS_SOLID,0,Colors[PolygonID])\n";
    Code += "EXE(\"SelectObject\",dc,Pen)\n";
    Code += "EXE(\"FrameRgn\",dc,ThisPolygon,Pen,{type=0,value=LineThickness/4},{type=0,value=LineThickness/4})\n";
    Code += "EXE(\"DeleteObject\",Pen)\n";
    Code += "end\n";
    Code += "FrameCount=FrameCount+1\n";
    Code += "if FrameCount>=Delay then\n";
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"DeleteObject\",ThisPolygon)\n";
    Code += "end\n";
    Code += "FrameCount=0\n";
    Code += "Polygons={}\n";
    Code += "Colors={}\n";
    Code += "end\n";
    Code += "end\n";
    Code += "function TriangleFrameShader(dc,t,W,H,Size,Speed,Transparency,R,G,B)\n";
    Code += "if Size==nil then Size=200 end\n";
    Code += "if Speed==nil then Speed=5 end\n";
    Code += "if Transparency==nil then Transparency=191 end\n";
    Code += "if R==nil then R=0 end\n";
    Code += "if G==nil then G=127 end\n";
    Code += "if B==nil then B=255 end\n";
    Code += "local W=W\n";
    Code += "local H=H\n";
    Code += "if GlobalOffset==nil then\n";
    Code += "GlobalOffset=0\n";
    Code += "end\n";
    Code += "local CurrentOffset=GlobalOffset\n";
    Code += "while CurrentOffset>0 do\n";
    Code += "CurrentOffset=CurrentOffset-Size\n";
    Code += "end\n";
    Code += "local Polygons={}\n";
    Code += "while CurrentOffset<=W+Size do\n";
    Code += "local X1,Y1=CurrentOffset,Size\n";
    Code += "local X2,Y2=RotatePoint(X1,Y1,2*math.pi/3,CurrentOffset,0)\n";
    Code += "local X3,Y3=RotatePoint(X2,Y2,2*math.pi/3,CurrentOffset,0)\n";
    Code += "MEM=createMemoryStream()\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=24\n";
    Code += "MEM.writeDword(math.floor(X1))\n";
    Code += "MEM.writeDword(math.floor(Y1))\n";
    Code += "MEM.writeDword(math.floor(X2))\n";
    Code += "MEM.writeDword(math.floor(Y2))\n";
    Code += "MEM.writeDword(math.floor(X3))\n";
    Code += "MEM.writeDword(math.floor(Y3))\n";
    Code += "MEM.Position=0\n";
    Code += "table.insert(Polygons,EXE(\"CreatePolygonRgn\",MEM.Memory,3,2))\n";
    Code += "X1,Y1=RotatePoint(X1,Y1,math.pi,W/2,H/2)\n";
    Code += "X2,Y2=RotatePoint(X2,Y2,math.pi,W/2,H/2)\n";
    Code += "X3,Y3=RotatePoint(X3,Y3,math.pi,W/2,H/2)\n";
    Code += "MEM.Position=0\n";
    Code += "MEM.size=24\n";
    Code += "MEM.writeDword(math.floor(X1))\n";
    Code += "MEM.writeDword(math.floor(Y1))\n";
    Code += "MEM.writeDword(math.floor(X2))\n";
    Code += "MEM.writeDword(math.floor(Y2))\n";
    Code += "MEM.writeDword(math.floor(X3))\n";
    Code += "MEM.writeDword(math.floor(Y3))\n";
    Code += "MEM.Position=0\n";
    Code += "table.insert(Polygons,EXE(\"CreatePolygonRgn\",MEM.Memory,3,2))\n";
    Code += "CurrentOffset=CurrentOffset+Size\n";
    Code += "end\n";
    Code += "MEM.destroy()\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"BitBlt\",MEMdc,0,0,W,H,dc,0,0,SRCCOPY)\n";
    Code += "local Color=(B<<16)|(G<<8)|R\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",Color)\n";
    Code += "EXE(\"SelectObject\",MEMdc,Brush)\n";
    Code += "for _,ThisPolygon in pairs(Polygons) do\n";
    Code += "EXE(\"FillRgn\",MEMdc,ThisPolygon,Brush)\n";
    Code += "EXE(\"DeleteObject\",ThisPolygon)\n";
    Code += "end\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "local BlendFunc=(Transparency<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "GlobalOffset=GlobalOffset+Speed\n";
    Code += "end\n";
    Code += "function CRTShader(dc,t,W,H,ColorOffsetAmount,Transparency)\n";
    Code += "if ColorOffsetAmount==nil then ColorOffsetAmount=2 end\n";
    Code += "if Transparency==nil then Transparency=127 end\n";
    Code += "local MEMdc_R=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_R=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_G=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_G=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_B=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_B=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "local MEMdc_Main=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP_Main=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "if Transparency<255 and ColorOffsetAmount~=0 then\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,BMP_R)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x0000FF)\n";
    Code += "EXE(\"SelectObject\",MEMdc_R,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_R,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_R,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,BMP_G)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x00FF00)\n";
    Code += "EXE(\"SelectObject\",MEMdc_G,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_G,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_G,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,BMP_B)\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0xFF0000)\n";
    Code += "EXE(\"SelectObject\",MEMdc_B,Brush)\n";
    Code += "EXE(\"PatBlt\",MEMdc_B,0,0,W,H,PATCOPY)\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",MEMdc_B,0,0,W,H,dc,0,0,SRCAND)\n";
    Code += "EXE(\"SelectObject\",MEMdc_Main,BMP_Main)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,ColorOffsetAmount,0,W,H,MEMdc_R,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,-ColorOffsetAmount,0,W,H,MEMdc_G,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,0,ColorOffsetAmount,W,H,MEMdc_B,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,0,-ColorOffsetAmount,W,H,MEMdc_R,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,ColorOffsetAmount,-ColorOffsetAmount,W,H,MEMdc_G,0,0,SRCPAINT)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,-ColorOffsetAmount,ColorOffsetAmount,W,H,MEMdc_B,0,0,SRCPAINT)\n";
    Code += "if Transparency>0 then\n";
    Code += "local BlendFunc=(Transparency<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",MEMdc_Main,0,0,W,H,dc,0,0,W,H,BlendFunc)\n";
    Code += "end\n";
    Code += "else\n";
    Code += "EXE(\"SelectObject\",MEMdc_Main,BMP_Main)\n";
    Code += "EXE(\"BitBlt\",MEMdc_Main,0,0,W,H,dc,0,0,SRCCOPY)\n";
    Code += "end\n";
    Code += "local Brush=EXE(\"CreateSolidBrush\",0x000000)\n";
    Code += "EXE(\"SelectObject\",MEMdc_Main,Brush)\n";
    Code += "local Line=math.random(0,2)\n";
    Code += "while Line<=H do\n";
    Code += "EXE(\"PatBlt\",MEMdc_Main,0,Line,W,1,PATCOPY)\n";
    Code += "Line=Line+3\n";
    Code += "end\n";
    Code += "EXE(\"DeleteObject\",Brush)\n";
    Code += "EXE(\"BitBlt\",dc,0,0,W,H,MEMdc_Main,0,0,SRCCOPY)\n";
    Code += "EXE(\"DeleteObject\",BMP_R)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_R)\n";
    Code += "EXE(\"DeleteObject\",BMP_G)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_G)\n";
    Code += "EXE(\"DeleteObject\",BMP_B)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_B)\n";
    Code += "EXE(\"DeleteObject\",BMP_Main)\n";
    Code += "EXE(\"DeleteDC\",MEMdc_Main)\n";
    Code += "end\n";
    Code += "function PixelateShader(dc,t,W,H,Multiplier)\n";
    Code += "if Multiplier==nil then Multiplier=4 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,math.floor(W/Multiplier),math.floor(H/Multiplier))\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"AlphaBlend\",MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier),dc,0,0,W,H,0x00FF0000)\n";
    Code += "EXE(\"AlphaBlend\",dc,0,0,W,H,MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier),0x00FF0000)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "end\n";
    Code += "function AlphaPixelateShader(dc,t,W,H,Multiplier,Alpha)\n";
    Code += "if Multiplier==nil then Multiplier=4 end\n";
    Code += "if Alpha==nil then Alpha=63 end\n";
    Code += "local MEMdc=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP=EXE(\"CreateCompatibleBitmap\",dc,math.floor(W/Multiplier),math.floor(H/Multiplier))\n";
    Code += "local MEMdc2=EXE(\"CreateCompatibleDC\",dc)\n";
    Code += "local BMP2=EXE(\"CreateCompatibleBitmap\",dc,W,H)\n";
    Code += "EXE(\"SelectObject\",MEMdc,BMP)\n";
    Code += "EXE(\"AlphaBlend\",MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier),dc,0,0,W,H,0x00FF0000)\n";
    Code += "EXE(\"SelectObject\",MEMdc2,BMP2)\n";
    Code += "EXE(\"AlphaBlend\",MEMdc2,0,0,W,H,MEMdc,0,0,math.floor(W/Multiplier),math.floor(H/Multiplier),0x00FF0000)\n";
    Code += "local BlendFunc=(Alpha<<16)|0x00000000\n";
    Code += "EXE(\"AlphaBlend\",MEMdc2,0,0,W,H,dc,0,0,W,H,BlendFunc)\n";
    Code += "EXE(\"BitBlt\",dc,0,0,W,H,MEMdc2,0,0,SRCCOPY)\n";
    Code += "EXE(\"DeleteObject\",BMP)\n";
    Code += "EXE(\"DeleteDC\",MEMdc)\n";
    Code += "EXE(\"DeleteObject\",BMP2)\n";
    Code += "EXE(\"DeleteDC\",MEMdc2)\n";
    Code += "end\n";
    Code += "function MatrixShader(dc,t,W,H,Cols,CharHeight,FontWeight,MaxSpeed,MinSpeed,MaxTailLength,MinTailLength,Font,R,G,B)\n";
    Code += "if Cols==nil then Cols=50 end\n";
    Code += "if CharHeight==nil then CharHeight=16 end\n";
    Code += "if FontWeight==nil then FontWeight=400 end\n";
    Code += "if MaxSpeed==nil then MaxSpeed=20 end\n";
    Code += "if MinSpeed==nil then MinSpeed=5 end\n";
    Code += "if MaxTailLength==nil then MaxTailLength=20 end\n";
    Code += "if MinTailLength==nil then MinTailLength=5 end\n";
    Code += "if Font==nil then Font=\"Fixedsys\" end\n";
    Code += "if R==nil then R=0 end\n";
    Code += "if G==nil then G=255 end\n";
    Code += "if B==nil then B=0 end\n";
    Code += "local CharWidth=math.floor(W/Cols)\n";
    Code += "if MatrixState==nil or #MatrixState~=Cols then\n";
    Code += "MatrixState={}\n";
    Code += "for Col=1,Cols do\n";
    Code += "MatrixState[Col]={Y=math.random(0,H),Speed=math.random(MinSpeed,MaxSpeed),TailLength=math.random(MinTailLength,MaxTailLength)}\n";
    Code += "end\n";
    Code += "end\n";
    Code += "EXE(\"SetBkMode\",dc,1)\n";
    Code += "local Font=EXE(\"CreateFontA\",\n";
    Code += "CharHeight,-- FontHeight\n";
    Code += "CharWidth,-- FontWidth\n";
    Code += "0,-- Escapement\n";
    Code += "0,-- Orientation\n";
    Code += "FontWeight,-- FontWeight\n";
    Code += "0,-- Italic\n";
    Code += "0,-- Underline\n";
    Code += "0,-- StrikeOut\n";
    Code += "1,-- CharSet\n";
    Code += "0,-- OutPrecision\n";
    Code += "0,-- ClipPrecision\n";
    Code += "0,-- Quality\n";
    Code += "0,-- PitchAndFamily\n";
    Code += "Font)-- FaceName\n";
    Code += "EXE(\"SelectObject\",dc,Font)\n";
    Code += "for Col=1,Cols do\n";
    Code += "local ColData=MatrixState[Col]\n";
    Code += "ColData.Y=ColData.Y+ColData.Speed\n";
    Code += "if ColData.Y>H+MaxTailLength*CharHeight then\n";
    Code += "ColData.Y=0\n";
    Code += "ColData.Speed=math.random(MinSpeed,MaxSpeed)\n";
    Code += "ColData.TailLength=math.random(MinTailLength,MaxTailLength)\n";
    Code += "end\n";
    Code += "for TailPiece=0,ColData.TailLength do\n";
    Code += "local PieceY=ColData.Y-TailPiece*CharHeight\n";
    Code += "if PieceY>=0 and PieceY<=H then\n";
    Code += "local Brightness=1-TailPiece/ColData.TailLength\n";
    Code += "local NewR=math.ceil(R*Brightness)\n";
    Code += "local NewG=math.ceil(G*Brightness)\n";
    Code += "local NewB=math.ceil(B*Brightness)\n";
    Code += "local TextColor=(NewB<<16)|(NewG<<8)|NewR\n";
    Code += "EXE(\"SetTextColor\",dc,TextColor)\n";
    Code += "EXE(\"TextOutA\",dc,Col*CharWidth,math.floor(PieceY),string.char(math.random(0,1<<7)),1)\n";
    Code += "end\n";
    Code += "end\n";
    Code += "end\n";
    Code += "EXE(\"DeleteObject\",Font)\n";
    Code += "end\n";
    return Code;
}

//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT nCmdShow)
{
    extern int __argc;
    extern char** __argv;

    //Debug console
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    SetConsoleOutputCP(65001);
    wcin.imbue(locale("en_US.UTF-8"));
    wcout.imbue(locale("en_US.UTF-8"));
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
    wcout << StartupStage1Text << "\n";

    if (__argc == 3) {
        string ARG1 = __argv[1];
        string ARG2 = __argv[2];
        if (ARG1 == "OPEN") {
            if (ARG2 == "PFXSTUDIO") {
                Config& config = Config::GetConfig();
                const VisualSettings& cVisual = config.GetVisualSettings();
                const ViewSettings& cView = config.GetViewSettings();
                const VideoSettings& cVideo = config.GetVideoSettings();
                const ControlsSettings& cControls = config.GetControlsSettings();
                const PlaybackSettings& cPlayback = config.GetPlaybackSettings();
                string Code = "";
                Code += MultiInstanceGuardInit();
                Code += "collectgarbage(\"collect\")\n";// Clean up any leftovers from previous run
                Code += "-- Variable Definitions: \n";
                Code += DefineMemoryAddress("SongLength", GetAddress(TotalTime));
                Code += DefineMemoryAddress("Microseconds", GetAddress(m_llStartTime));
                Code += DefineMemoryAddress("Ticks", GetAddress(m_iStartTick));
                Code += DefineMemoryAddress("Resolution", GetAddress(resolution));
                Code += DefineMemoryAddress("NoteCount", GetAddress(TotalNC));
                Code += DefineMemoryAddress("NotesPerSecond", GetAddress(nps));
                Code += DefineMemoryAddress("Polyphony", GetAddress(polyphony));
                Code += DefineMemoryAddress("Passed", GetAddress(passed));
                Code += DefineMemoryAddress("Volume", cPlayback.GetVolumeAddress());
                Code += DefineMemoryAddress("Mute", cPlayback.GetMuteAddress());
                Code += DefineMemoryAddress("PlaybackSpeed", cPlayback.GetSpeedAddress());
                Code += DefineMemoryAddress("NoteSpeed", cPlayback.GetNSpeedAddress());
                Code += DefineMemoryAddress("OffsetX", cView.GetOffsetXAddress());
                Code += DefineMemoryAddress("OffsetY", cView.GetOffsetYAddress());
                Code += DefineMemoryAddress("Zoom", cView.GetZoomXAddress());
                Code += DefineMemoryAddress("UpdateNotePos", GetAddress(UpdateNotePos));
                Code += DefineMemoryAddress("SameWidth", GetAddress(cVideo.bSameWidth));
                Code += DefineMemoryAddress("VelocityMapping", GetAddress(cVideo.bMapVel));
                Code += DefineMemoryAddress("StartKey", GetAddress(cVisual.iFirstKey));
                Code += DefineMemoryAddress("EndKey", GetAddress(cVisual.iLastKey));
                Code += DefineMemoryAddress("KeyMode", GetAddress(cVisual.eKeysShown));
                Code += DefineMemoryAddress("Width", GetAddress(width));
                Code += DefineMemoryAddress("Height", GetAddress(height));
                Code += DefineMemoryAddress("Paused", cPlayback.GetPausedAddress());
                Code += DefineMemoryAddress("Keyboard", cView.GetKeyboardAddress());
                Code += DefineMemoryAddress("VisualizePitchBends", GetAddress(cVideo.bVisualizePitchBends));
                Code += DefineMemoryAddress("PhigrosMode", GetAddress(cControls.bPhigros));
                Code += DefineMemoryAddress("ShowMarkers", GetAddress(cVideo.bShowMarkers));
                Code += DefineMemoryAddress("TickBased", GetAddress(cVideo.bTickBased));
                Code += DefineMemoryAddress("HideStatistics", GetAddress(cVideo.bDisableUI));
                Code += DefineMemoryAddress("RemoveOverlaps", GetAddress(cVideo.bOR));
                Code += DefineMemoryAddress("LimitFPS", GetAddress(cVideo.bLimitFPS));
                Code += DefineMemoryAddress("VelocityThreshold", GetAddress(cControls.iVelocityThreshold));
                Code += DefineMemoryAddress("Caption", GetAddress(CheatEngineCaption));
                Code += DefineMemoryAddress("DifficultyText", GetAddress(Difficulty));
                Code += "-- Custom Variable Definitions: (Version: " + WStringToUtf8(VersionString) + ")\n";
                Code += "\n";
                Code += "-- Function Definitions: (DO NOT CHANGE!)\n";
                Code += InitializeVariables();
                Code += FakeThreadingEngine();
                Code += CreateDebugWindow("Pending", "0", "0", "400", "250");
                Code += CreateDebugWindow("Active", "0", "280", "400", "220");
                Code += CreateDebugWindow("History", "400", "0", "350", "500", false);
                Code += GettersAndSetters("SongLength", IntSizeToCE(sizeof(TotalTime)), true, "History");
                Code += GettersAndSetters("Microseconds", IntSizeToCE(sizeof(m_llStartTime)), false, "History");
                Code += GettersAndSetters("Ticks", IntSizeToCE(sizeof(m_iStartTick)), true, "History", true);
                Code += GettersAndSetters("Resolution", IntSizeToCE(sizeof(resolution)), true, "History");
                Code += GettersAndSetters("NoteCount", IntSizeToCE(sizeof(TotalNC)), true, "History");
                Code += GettersAndSetters("NotesPerSecond", IntSizeToCE(sizeof(nps)), true, "History");
                Code += GettersAndSetters("Polyphony", IntSizeToCE(sizeof(polyphony)), true, "History");
                Code += GettersAndSetters("Passed", IntSizeToCE(sizeof(passed)), true, "History");
                Code += GettersAndSetters("Volume", FloatSizeToCE(cPlayback.GetVolumeSize()), false, "History");
                Code += GettersAndSetters("Mute", IntSizeToCE(cPlayback.GetMuteSize()), false, "History");
                Code += GettersAndSetters("PlaybackSpeed", FloatSizeToCE(cPlayback.GetSpeedSize()), false, "History");
                Code += GettersAndSetters("NoteSpeed", FloatSizeToCE(cPlayback.GetNSpeedSize()), false, "History");
                Code += GettersAndSetters("OffsetX", FloatSizeToCE(cView.GetOffsetXSize()), false, "History", false, true);
                Code += GettersAndSetters("OffsetY", FloatSizeToCE(cView.GetOffsetYSize()), false, "History", false, true);
                Code += GettersAndSetters("Zoom", FloatSizeToCE(cView.GetZoomXSize()), false, "History", false, true);
                Code += GettersAndSetters("SameWidth", IntSizeToCE(sizeof(cVideo.bSameWidth)), false, "History", false, true);
                Code += GettersAndSetters("VelocityMapping", IntSizeToCE(sizeof(cVideo.bMapVel)), false, "History");
                Code += GettersAndSetters("StartKey", IntSizeToCE(sizeof(cVisual.iFirstKey)), false, "History", false, true);
                Code += GettersAndSetters("EndKey", IntSizeToCE(sizeof(cVisual.iLastKey)), false, "History", false, true);
                Code += GettersAndSetters("KeyMode", IntSizeToCE(sizeof(cVisual.eKeysShown)), false, "History", false, true);
                Code += GettersAndSetters("Width", IntSizeToCE(sizeof(width)), true, "History", true, true);
                Code += GettersAndSetters("Height", IntSizeToCE(sizeof(height)), true, "History", true, true);
                Code += SetResolutionFunction();
                Code += GettersAndSetters("Paused", IntSizeToCE(cPlayback.GetPausedSize()), false, "History");
                Code += GettersAndSetters("Keyboard", IntSizeToCE(cView.GetKeyboardVarSize()), false, "History");
                Code += GettersAndSetters("VisualizePitchBends", IntSizeToCE(sizeof(cVideo.bVisualizePitchBends)), false, "History", false, true);
                Code += GettersAndSetters("PhigrosMode", IntSizeToCE(sizeof(cControls.bPhigros)), false, "History");
                Code += GettersAndSetters("ShowMarkers", IntSizeToCE(sizeof(cVideo.bShowMarkers)), false, "History");
                Code += GettersAndSetters("TickBased", IntSizeToCE(sizeof(cVideo.bTickBased)), false, "History");
                Code += GettersAndSetters("HideStatistics", IntSizeToCE(sizeof(cVideo.bDisableUI)), false, "History");
                Code += GettersAndSetters("RemoveOverlaps", IntSizeToCE(sizeof(cVideo.bOR)), false, "History");
                Code += GettersAndSetters("LimitFPS", IntSizeToCE(sizeof(cVideo.bLimitFPS)), false, "History");
                Code += GettersAndSetters("VelocityThreshold", IntSizeToCE(sizeof(cControls.iVelocityThreshold)), false, "History");
                Code += StringTypeGettersAndSetters("Caption", sizeof(CheatEngineCaption) / sizeof(CheatEngineCaption[0]), false, "History");
                Code += StringTypeGettersAndSetters("DifficultyText", sizeof(Difficulty) / sizeof(Difficulty[0]), false, "History");
                Code += EasingFunctions();
                Code += WaitUntil(false, "Pending");
                Code += WaitUntil(true, "Pending");
                Code += AnimationFunction(false, false, false, false, "Pending", "Active");
                Code += AnimationFunction(true, false, false, false, "Pending", "Active");
                Code += AnimationFunction(false, true, false, false, "Pending", "Active");
                Code += AnimationFunction(true, true, false, false, "Pending", "Active");
                Code += InstantaneousAnimationFunction(false, false, "Pending");
                Code += AnimationFunction(false, false, true, false, "Pending", "Active");
                Code += AnimationFunction(true, false, true, false, "Pending", "Active");
                Code += AnimationFunction(false, true, true, false, "Pending", "Active");
                Code += AnimationFunction(true, true, true, false, "Pending", "Active");
                Code += InstantaneousAnimationFunction(true, false, "Pending");
                Code += AnimationFunction(false, false, false, true, "Pending", "Active");
                Code += AnimationFunction(true, false, false, true, "Pending", "Active");
                Code += AnimationFunction(false, true, false, true, "Pending", "Active");
                Code += AnimationFunction(true, true, false, true, "Pending", "Active");
                Code += InstantaneousAnimationFunction(false, true, "Pending");
                Code += AnimationFunction(false, false, true, true, "Pending", "Active");
                Code += AnimationFunction(true, false, true, true, "Pending", "Active");
                Code += AnimationFunction(false, true, true, true, "Pending", "Active");
                Code += AnimationFunction(true, true, true, true, "Pending", "Active");
                Code += InstantaneousAnimationFunction(true, true, "Pending");
                Code += ShaderEngine();
                Code += "-- Builtin GDI Shaders:\n";
                Code += BuiltinShaders();
                Code += "-- Custom GDI Shaders:\n";
                Code += "\n";
                Code += "-- Initialization:\n";
                Code += "EXE(\"SetForegroundWindow\",hPFX)\n";
                Code += "DisplayShaderFPS=true\n";
                Code += "SetMicroseconds(-3*S)\n";
                Code += "SetVolume(1.00)\n";
                Code += "SetMute(0)\n";
                Code += "SetPlaybackSpeed(1.00)\n";
                Code += "SetNoteSpeed(0.25)\n";
                Code += "SetStartKey(0)\n";
                Code += "SetEndKey(127)\n";
                Code += "SetKeyMode(0)\n";
                Code += "SetOffsetX(0.00)\n";
                Code += "SetOffsetY(0.00)\n";
                Code += "SetZoom(0.00)\n";
                Code += "SetSameWidth(1)\n";
                Code += "SetVelocityMapping(0)\n";
                Code += "SetPaused(0)\n";
                Code += "SetKeyboard(1)\n";
                Code += "SetVisualizePitchBends(1)\n";
                Code += "SetPhigrosMode(1)\n";
                Code += "SetShowMarkers(1)\n";
                Code += "SetTickBased(1)\n";
                Code += "SetHideStatistics(0)\n";
                Code += "SetRemoveOverlaps(0)\n";
                Code += "SetLimitFPS(1)\n";
                Code += "SetVelocityThreshold(1)\n";
                Code += "SetCaption(\"Welcome to Piano-FX Pro\")\n";
                Code += "-- If you feel the default difficulty is inappropriate, change it with the following code: \n";
                Code += "SetDifficultyText(\"IN Lv.12\")\n";
                Code += "-- Do this if you have color events on the first tick:\n";
                Code += "SetMicroseconds(0)\n";
                Code += "WaitUntilTime(1)\n";
                Code += "SetMicroseconds(-3*S)\n";
                Code += "-- Main Program:\n";
                Code += "Spawn(ShaderEngine)\n";
                Code += "SmoothAnimationByTime(0,1,-S,0,EaseOutBack,SetZoom)\n";
                Code += "SmoothAnimationByTime(CenterX(0),CenterX(1),-S,0,EaseOutBack,SetOffsetX)\n";
                Code += "SmoothAnimationByTime(CenterY(0),CenterY(1),-S,0,EaseOutBack,SetOffsetY)\n";
                Code += "RunShaderByTime(-3*S,-2*S,DarkBlankShader)\n";
                Code += "RunShaderByTime(-2*S,0,LightenFromDarkShader)\n";
                Code += "InstantaneousAnimationByTimeAndWait(\"\",0,SetCaption)\n";
                Code += "-- Cleaning Up:\n";
                Code += "while Incomplete() or ShaderWorking do Tick() end\n";
                Code += CleanUp();
                LuaCode = Code;
                StudioMain();
                return 0;
            }
            if (ARG2 == "CheatEngineTutorial") {
                DisplayTutorial(CheatEngineTutorial);
                return 0;
            }
            if (ARG2 == "VisualizePitchBendTutorial") {
                DisplayTutorial(VisualizePitchBendTutorial);
                return 0;
            }
            if (ARG2 == "GDITutorial") {
                DisplayTutorial(GDITutorial);
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
    memcpy(pData + offset, ffmpeg5, sizeof(ffmpeg5));
    offset += sizeof(ffmpeg5);
    memcpy(pData + offset, ffmpeg6, sizeof(ffmpeg6));
    offset += sizeof(ffmpeg6);
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
                while (true) {
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
        string FFmpegPath = ProgramPath() + "\\FFMPEG.exe";
        if (!filesystem::exists(FFmpegPath)) {
            ofstream FFmpegFile(FFmpegPath, ios::binary);
            FFmpegFile.write(reinterpret_cast<const char*>(pData), ffmpeg_len);
            FFmpegFile.close();
        }
        delete[] pData;
    }
    else {
        cout << "Embedded ffmpeg.exe data length incorrect! \n";
        cout << "Expected: ";
        cout << ffmpeg_len;
        cout << "\n";
        cout << "Actual: ";
        cout << offset;
        cout << "\n";
        while (true) {

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
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = GfxProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = GFXCLASSNAME;
    if (!RegisterClassEx(&wc))
        return 1;

    // Register the position control window class
    wc.style = 0;
    wc.lpfnWndProc = PosnProc;
    wc.lpszClassName = POSNCLASSNAME;
    if (!RegisterClassEx(&wc))
        return 1;

    // In addition to getting settings, triggers loading of saved config
    Config& config = Config::GetConfig();
    ViewSettings& cView = config.GetViewSettings();

    // Create the application window
    g_hWnd = CreateWindowEx(NULL, CLASSNAME, MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle5 MainWindowTitle7 MainWindowTitle8, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, cView.GetMainLeft(), cView.GetMainTop(), cView.GetMainWidth(), cView.GetMainHeight(), NULL, NULL, wc.hInstance, NULL);

    if (!g_hWnd) return 1;

    // Accept drag and drop
    DragAcceptFiles(g_hWnd, true);

    // Creation order (z-order) matters big time for full screen

    // Create the controls rebar
    g_hWndBar = CreateRebar(g_hWnd);
    if (!g_hWndBar) return 1;

    // Create the graphics window
    g_hWndGfx = CreateWindowEx(NULL, GFXCLASSNAME, NULL, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS, NULL, NULL, NULL, NULL, g_hWnd, NULL, wc.hInstance, NULL);
    if (!g_hWndGfx) return 1;

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAINMENU));
    if (!hAccel) return 1;

    HANDLE hThread;

    MIDI::InitArrays();
    if (__argc == 2) {
        // Get the game going
        hThread = CreateThread(NULL, 0, GameThread, new SplashScreen(NULL, NULL, false), 0, NULL);
        if (!hThread) return 1;
        // Set up GUI and show
        SetPlayMode(GameState::Intro);
        SetOnTop(cView.GetOnTop());
        ShowControls(cView.GetControls());
        ShowWindow(g_hWndGfx, SW_SHOW);
        ShowWindow(g_hWnd, nCmdShow);
        UpdateWindow(g_hWnd);
        SetFocus(g_hWndGfx);
        size_t llFilenameLength = mbstowcs(nullptr, __argv[1], 0);
        wstring sFilename(llFilenameLength, L'\0');
        mbstowcs(&sFilename[0], __argv[1], llFilenameLength);
        PlayFile(sFilename);
    }
    else {
        wcout << StartupStage2Text << "\n";
        // Get the game going
        hThread = CreateThread(NULL, 0, GameThread, new SplashScreen(NULL, NULL, true), 0, NULL);
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
    wcout << StartupStage3Text << "\n";

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
    D3D12Renderer* pRenderer = new D3D12Renderer();
    auto init_res = pRenderer->Init(g_hWndGfx, Config::GetConfig().GetVideoSettings().bLimitFPS);
    if (FAILED(get<0>(init_res)))
    {
        wchar_t msg[1 << 10] = {};
        _snwprintf_s(msg, 1024, L"Fatal error initializing D3D12.\n%S failed with code 0x%x.", get<1>(init_res), get<0>(init_res));
        MessageBox(g_hWnd, msg, TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
        PostMessage(g_hWnd, WM_QUIT, 1, 0);
        return 1;
    }

    // Create the game object
    GameState* pGameState = reinterpret_cast<GameState*>(lpParameter);
    pGameState->SetHWnd(g_hWndGfx);
    pGameState->SetRenderer(pRenderer);
    pGameState->Init();
    GameState::GameError ErrorLevel;

    wchar_t buf[1 << 10] = {};
    _snwprintf_s(buf, 1 << 10, MainWindowTitle1 L" v" VersionString L" | " MainWindowTitle2 L" | " MainWindowTitle3 MainWindowTitle6 MainWindowTitle7 MainWindowTitle8);
    SetWindowTextW(g_hWnd, buf);

    // Event, logic, render...
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        while (g_MsgQueue.Pop(msg))
            pGameState->MsgProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);

        if ((ErrorLevel = GameState::ChangeState(pGameState->NextState(), &pGameState)) != GameState::Success)
            PostMessage(g_hWnd, WM_COMMAND, ID_GAMEERROR, ErrorLevel);
        pGameState->Logic();
        pGameState->Render();
    }

    delete pGameState;
    delete pRenderer;

    return 0;
}