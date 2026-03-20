/*************************************************************************************************
*
* File: Misc.h
*
* Description: Defines small helper objects
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <string>
using namespace std;

#define VersionString L"4.3"

// Type definitions... 
typedef signed long mtk_t; // Midi tick timing type
typedef signed long long mms_t; // Midi microsecond timing type
typedef unsigned long ums_t; // Midi microsecond timing type (unsigned)
typedef unsigned char key_t; // Midi note key and velocity type
typedef signed char skey_t; // If value is 192 or more, reinterpret cast to this type
typedef uint16_t track_t; // Midi track id type
typedef uint8_t chan_t; // Midi channel id type
typedef uint32_t TnC_t; // Any code that does `track * 16 + channel` must use this type
typedef uint8_t msg_t; // Midi message type
typedef uint32_t msgln_t; // Midi message length type (always 32bit unsigned)
typedef uint32_t idx_t; // Array indexing type
typedef int64_t sidx_t; // Array indexing type (signed)
constexpr idx_t IDX_MAX = static_cast<idx_t>(-1); // Maximum array size
typedef uint32_t color_t; // Color type
typedef signed long bpm_t; // Anything that has something to do with tempo, beat, and measure
typedef signed int win32_t; // Classic signed 32bit integer, aka dword, used for interfacing with Win32 API (should not appear in any game logic)
typedef signed short winword_t; // Classic signed 16bit integer, aka word, used for interfacing with Win32 API (should not appear in any game logic)

template <typename T>
__forceinline string GetAddress(const T& Variable) {
    HMODULE ProcessBaseAddress = GetModuleHandle(NULL);
    uintptr_t VariableAddress = reinterpret_cast<uintptr_t>(&Variable);
    uintptr_t OffsetAddress = VariableAddress - reinterpret_cast<uintptr_t>(ProcessBaseAddress);
    stringstream sout;
    sout << uppercase << hex << OffsetAddress;
    return sout.str();
}

__forceinline string IntSizeToCE(uint8_t Size) {
    if (Size == 1) {
        return "ShortInteger";
    }
    if (Size == 2) {
        return "SmallInteger";
    }
    if (Size == 4) {
        return "Integer";
    }
    if (Size == 8) {
        return "Qword";
    }
    return "Bytes";
}

__forceinline string FloatSizeToCE(uint8_t Size) {
    if (Size == 4) {
        return "Float";
    }
    if (Size == 8) {
        return "Double";
    }
    return "Bytes";
}

//The timer
class Timer
{
public:
    // Initializes variables
    ~Timer();
    void Init(bool manual);

    // The various clock actions
    void Start();
    void Pause();
    void Unpause();

    // Gets the timer's time
    double GetSecs();
    mms_t GetMicroSecs();
    mms_t GetTicks();
    mms_t GetTicksPerSec() { return m_llTicksPerSec; }

    // Status accessors
    bool IsStarted() { return m_bStarted; }
    bool IsPaused() { return m_bPaused; }

    // Manual timer stuff
    void AddManualTime(mms_t time);
    void SetFrameRate(mms_t rate);
    void IncrementFrame();
    bool m_bManualTimer;
    double m_dFramerate;

private:
    static const mms_t m_llPrecisionLimit = 1000000000ll;
    static const mms_t m_llPrecisionThrottle = 1000ll;

    // Timer stuff
    mms_t GetRawTicks();
    mms_t m_llStartTicks;
    mms_t m_llTicksPerSec;

    // More manual timer stuff
    mms_t m_llManualTicks;
    mms_t m_llManualTicksPerFrame;

    // Ticks stored when the timer was paused
    mms_t m_llPausedTicks;

    // Timer status
    bool m_bStarted;
    bool m_bPaused;
};

//-----------------------------------------------------------------------------
// Small utility functions
//-----------------------------------------------------------------------------

class Util
{
public:
    static wchar_t* StringToWstring(const string& s);
    static char* WstringToString(const wstring& s);
    static void ParseLongHex(const string& sText, string& sVal);
    static color_t RandColor();
    static void RGBtoHSV(color_t R, color_t G, color_t B, color_t& H, color_t& S, color_t& V);
    static void HSVtoRGB(color_t H, color_t S, color_t V, color_t& R, color_t& G, color_t& B);
private:
    static char m_sBuf[16384];
    static wchar_t m_wsBuf[16384];
};

//-----------------------------------------------------------------------------
// The thread safe queue (TSQueue) class. Only safe for a single producer and
// a single consumer
//-----------------------------------------------------------------------------

template < typename T >
class TSQueue
{
public:
    TSQueue() : m_iWrite(0), m_iRead(0) { }
    bool Push(const T& tElement);
    bool Pop(T& tElement);

    void ForcePush(const T& tElement) { while (!Push(tElement)); }

private:
    static const win32_t QueueSize = 1 << 16;
    T m_tQueue[QueueSize]; // Does this need to be volatile? Unsure. Doesn't work with MSG if volatile.
    volatile win32_t m_iWrite;
    volatile win32_t m_iRead;
};

template< class T >
bool TSQueue<T>::Push(const T& tElement)
{
    win32_t iNextElement = (m_iWrite + 1) % QueueSize;

    // Is the queue full?
    if (iNextElement == m_iRead) return false;

    // Push the element. Order of execution is very important.
    m_tQueue[m_iWrite] = tElement;
    m_iWrite = iNextElement;

    return true;
}

template< class T >
bool TSQueue<T>::Pop(T& tElement)
{
    // Is the queue empty?
    if (m_iWrite == m_iRead) return false;

    // Compute where to read. Read. Update read pointer. Order very important.
    win32_t iNextElement = (m_iRead + 1) % QueueSize;
    tElement = m_tQueue[m_iRead];
    m_iRead = iNextElement;

    return true;
}

__forceinline wstring Utf8ToWString(const string& u8str)
{
    if (u8str.empty()) return {};

    size_t size_needed = MultiByteToWideChar(
        CP_UTF8,
        0,
        u8str.data(),
        (win32_t)u8str.size(),
        nullptr,
        0
    );

    wstring result(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        u8str.data(),
        (win32_t)u8str.size(),
        &result[0],
        size_needed
    );

    return result;
}

__forceinline string WStringToUtf8(const wstring& wstr)
{
    if (wstr.empty()) return {};

    size_t size_needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        (win32_t)wstr.size(),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    string result(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        (win32_t)wstr.size(),
        &result[0],
        size_needed,
        nullptr,
        nullptr
    );

    return result;
}

struct NoteColor
{
    color_t iPrimaryRGB, iDarkRGB, iVeryDarkRGB;
};

__forceinline uint64_t Div255_64(uint64_t Input, uint64_t AddPattern, uint64_t LaneMask) {
    return ((Input + AddPattern) + (((Input + AddPattern) >> 8) & LaneMask)) >> 8;
}

__forceinline uint32_t Div255_32(uint32_t Input, uint32_t AddPattern, uint32_t LaneMask) {
    return ((Input + AddPattern) + (((Input + AddPattern) >> 8) & LaneMask)) >> 8;
}

__forceinline void BlendNoteColor(NoteColor* Dst, NoteColor* Src) {
    uint8_t InvertAlpha = (Src->iPrimaryRGB >> 24) & 0xFF;
    if (InvertAlpha == 0xFF) { return; }
    else if (InvertAlpha == 0x00) {
        memcpy(Dst, Src, sizeof(NoteColor));
        return;
    }
    uint8_t Alpha = InvertAlpha ^ 0xFF;
    uint64_t* P64Src = reinterpret_cast<uint64_t*>(Src);
    uint64_t* P64Dst = reinterpret_cast<uint64_t*>(Dst);
    static constexpr uint64_t MASK01 = 0x00FF00FF00FF00FF;
    static constexpr uint64_t PLUS01 = 0x0080008000800080;
    static constexpr uint64_t MASK02 = 0x0000FF000000FF00;
    static constexpr uint64_t PLUS02 = 0x0000800080008000;
    static constexpr uint32_t MASK03 = 0xFF000000;
    static constexpr uint32_t MASK04 = 0x00FF00FF;
    static constexpr uint32_t PLUS04 = 0x00800080;
    static constexpr uint64_t MASK05 = 0x0000FF00FF00FF00;
    uint64_t sRPBPRDBD = *P64Src & MASK01;
    uint64_t sGPGVGD = *P64Src & MASK02;
    uint32_t* P32Src = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(P64Src) + 6);
    sGPGVGD |= *P32Src & MASK03;
    uint32_t sRVBV = Src->iVeryDarkRGB & MASK04;
    uint64_t dRPBPRDBD = *P64Dst & MASK01;
    uint64_t dGPGVGD = *P64Dst & MASK02;
    uint32_t* P32Dst = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(P64Dst) + 6);
    dGPGVGD |= *P32Dst & MASK03;
    uint32_t dRVBV = Dst->iVeryDarkRGB & MASK04;
    *P64Dst = Div255_64(sRPBPRDBD * Alpha + dRPBPRDBD * InvertAlpha, PLUS01, MASK01) & MASK01;
    uint64_t Tmp02 = Div255_64(sGPGVGD * Alpha + dGPGVGD * InvertAlpha, PLUS02, MASK05) & MASK05;
    *P64Dst |= Tmp02 & MASK02;
    Dst->iVeryDarkRGB = Div255_32(sRVBV * Alpha + dRVBV * InvertAlpha, PLUS04, MASK04) & MASK04;
    Dst->iVeryDarkRGB |= static_cast<uint32_t>((Tmp02 >> 16) & 0x0000FF00);
}