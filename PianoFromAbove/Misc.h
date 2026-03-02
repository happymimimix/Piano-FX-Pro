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

#define VersionString L"4.2"

// Type definitions... 
typedef signed long mtk_t; // Midi tick timing type
typedef signed long long mms_t; // Midi microsecond timing type
typedef unsigned char key_t; // Midi note key and velocity type
typedef uint16_t track_t; // Midi track id type
typedef uint8_t chan_t; // Midi channel id type
typedef uint32_t TnC_t; // Any code that does `track * 16 + channel` must use this type
typedef uint8_t msg_t; // Midi message type
typedef uint32_t msgln_t; // Midi message length type (always 32bit unsigned)
typedef uint32_t idx_t; // Array indexing type
typedef int64_t sidx_t; // Array indexing type (signed)
constexpr idx_t IDX_MAX = static_cast<idx_t>(-1); // Maximum array size
typedef uint32_t color_t; // Color type
typedef uint32_t bpm_t; // Anything that has something to do with tempo, beat, and measure
typedef signed int win32_t; // Classic signed 32bit integer used for interfacing with Win32 API (should not appear in any game logic)

template <typename T>
string GetAddress(const T& Variable) {
    HMODULE ProcessBaseAddress = GetModuleHandle(NULL);
    uintptr_t VariableAddress = reinterpret_cast<uintptr_t>(&Variable);
    uintptr_t OffsetAddress = VariableAddress - reinterpret_cast<uintptr_t>(ProcessBaseAddress);
    stringstream sout;
    sout << uppercase << hex << OffsetAddress;
    return sout.str();
}

inline string IntSizeToCE(uint8_t Size) {
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

inline string FloatSizeToCE(uint8_t Size) {
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

inline string WStringToUtf8(const wstring& wstr)
{
    if (wstr.empty()) return {};

    size_t size_needed = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        (int)wstr.size(),
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
        (int)wstr.size(),
        &result[0],
        size_needed,
        nullptr,
        nullptr
    );

    return result;
}