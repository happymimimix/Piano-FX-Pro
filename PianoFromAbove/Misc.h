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
#include "Globals.h"
using namespace std;

#define VersionString L"4.2"

template <typename T>
string GetAddress(const T& Variable) {
    HMODULE ProcessBaseAddress = GetModuleHandle(NULL);
    uintptr_t VariableAddress = reinterpret_cast<uintptr_t>(&Variable);
    uintptr_t OffsetAddress = VariableAddress - reinterpret_cast<uintptr_t>(ProcessBaseAddress);
    stringstream sout;
    sout << uppercase << hex << OffsetAddress;
    return sout.str();
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
    long long GetMicroSecs();
    long long GetTicks();
    long long GetTicksPerSec() { return m_llTicksPerSec; }

    // Status accessors
    bool IsStarted() { return m_bStarted; }
    bool IsPaused() { return m_bPaused; }

    // Manual timer stuff
    void AddManualTime(long long time);
    void SetFrameRate(unsigned rate);
    void IncrementFrame();
    bool m_bManualTimer;
    double m_dFramerate;

private:
    static const long long m_llPrecisionLimit = 1000000000ll;
    static const long long m_llPrecisionThrottle = 1000ll;

    // Timer stuff
    long long GetRawTicks();
    long long m_llStartTicks;
    long long m_llTicksPerSec;

    // More manual timer stuff
    long long m_llManualTicks;
    long long m_llManualTicksPerFrame;

    // Ticks stored when the timer was paused
    long long m_llPausedTicks;

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
    static const int QueueSize = 1024;
    T m_tQueue[QueueSize]; // Does this need to be volatile? Unsure. Doesn't work with MSG if volatile.
    volatile int m_iWrite;
    volatile int m_iRead;
};

template< class T >
bool TSQueue<T>::Push(const T& tElement)
{
    int iNextElement = (m_iWrite + 1) % QueueSize;

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
    int iNextElement = (m_iRead + 1) % QueueSize;
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