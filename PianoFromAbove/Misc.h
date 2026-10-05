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
#ifdef DBG
#include <DebugLanguageOverride.h>
#endif

#define VersionString L"4.3"

// Type definitions... 
typedef signed long long mtk_t; // Midi tick timing type
typedef signed long long mms_t; // Midi microsecond timing type
typedef unsigned char key_t; // Midi note key and velocity type
typedef signed char skey_t; // If value is 192 or more, reinterpret cast to this type
typedef uint16_t track_t; // Midi track id type
typedef uint8_t chan_t; // Midi channel id type
typedef uint32_t TnC_t; // Any code that does `track * 16 + channel` must use this type
typedef uint8_t msg_t; // Midi message type
typedef uint32_t msgln_t; // Midi message length type (always 32bit unsigned)
typedef int64_t fileln_t; // File length type (always 64bit signed)
#ifndef BIG_INDEX
typedef uint32_t idx_t; // Array indexing type
#else
static_assert(sizeof(void*) == 8, "CRITICAL ERROR: BIG INDEX IS NOT SUPPORTED IN 32BIT!");
typedef uint64_t idx_t; // Array indexing type
#endif
typedef uint32_t sidx_t; // Small array indexing type
constexpr idx_t IDX_MAX = static_cast<idx_t>(~0); // Maximum array size
constexpr sidx_t SIDX_MAX = static_cast<sidx_t>(~0); // Maximum small array size
constexpr track_t TRACK_INVALID = static_cast<track_t>(~0); // A track ID that is impossible for any regular midi track to get
typedef uint32_t color_t; // RGBA combined color type
typedef signed long bpm_t; // Anything that has something to do with tempo, beat, and measure
typedef signed int win32_t; // Classic signed 32bit integer, aka dword, used for interfacing with Win32 API (should not appear in any game logic)
typedef signed short winword_t; // Classic signed 16bit integer, aka word, used for interfacing with Win32 API (should not appear in any game logic)
#define LONG_MAX_PATH 0x0FFF

inline static constexpr mms_t MS = 1e+3;
inline static constexpr mms_t S = 1e+6;

#define NoteVelFormula(velvar) (static_cast<unsigned char>(m_dVolume > 1.0 ? static_cast<double>(INT8_MAX) - (static_cast<double>(INT8_MAX) - static_cast<double>(velvar)) * (2.0 - m_dVolume) : static_cast<double>(velvar) * m_dVolume))
#define IsNote(type) (type == MIDIChannelEvent::NoteOn || type == MIDIChannelEvent::NoteOff)
#define IsNotNote(type) (type != MIDIChannelEvent::NoteOn && type != MIDIChannelEvent::NoteOff)
#define IsOn(type,vel) (type == MIDIChannelEvent::NoteOn && vel > 0)
#define IsOff(type,vel) (type == MIDIChannelEvent::NoteOff || (type == MIDIChannelEvent::NoteOn && vel == 0))
#define off2on(code) (code & 0x0F | 0x90)

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

__forceinline string CEPtr() {
    return "Pointer->";
}

inline wstring GetExePath(void) {
    wchar_t szFilePath[LONG_MAX_PATH] = {};
    GetModuleFileNameW(NULL, szFilePath, LONG_MAX_PATH);
    *wcsrchr(szFilePath, '\\') = 0;
    return szFilePath;
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

__forceinline uint64_t Div255_64(uint64_t Input, uint64_t AddPattern, uint64_t LaneMask)
{
    return ((Input + AddPattern) + (((Input + AddPattern) >> 8) & LaneMask)) >> 8;
}

__forceinline uint32_t Div255_32(uint32_t Input, uint32_t AddPattern, uint32_t LaneMask)
{
    return ((Input + AddPattern) + (((Input + AddPattern) >> 8) & LaneMask)) >> 8;
}

__forceinline void BlendNoteColor(NoteColor* Dst, NoteColor* Src)
{
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

namespace std {// Why there's fucking no expoenential_upper_bound in std:: already? 
    template <class _UFwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _UFwdIt upper_bound_unchecked(_UFwdIt _UFirst, _UFwdIt _ULast, const _Ty& _Val, _Pr _Pred) {
        // find first element that _Val is before
        _Iter_diff_t<_UFwdIt> _Count = _STD distance(_UFirst, _ULast);

        while (0 < _Count) { // divide and conquer, find half that contains answer
            _Iter_diff_t<_UFwdIt> _Count2 = _Count / 2;
            const auto _UMid = _STD next(_UFirst, _Count2);
            if (_Pred(_Val, *_UMid)) {
                _Count = _Count2;
            }
            else { // try top half
                _UFirst = _Next_iter(_UMid);
                _Count -= _Count2 + 1;
            }
        }
        return _UFirst;
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound_right(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        _Adl_verify_range(_First, _Last);
        // const auto _UFirst = _Get_unwrapped(_First);
        const auto _ULast = _Get_unwrapped(_Last);
        auto _UCurrent = _Get_unwrapped(_Hint);
        if (_Pred(_Val, *_UCurrent)) {
            _Seek_wrapped(_First, _UCurrent);
            return _First;
        }
        _Iter_diff_t<_FwdIt> _Step = 1;

        Next:
        const auto _UNext = _STD next(_UCurrent, _Step);
        if (_UNext >= _ULast) {
            _UCurrent = upper_bound_unchecked(_UCurrent + 1, _ULast, _Val, _Pred);
        }
        else if (_Pred(_Val, *_UNext)) {
            _UCurrent = upper_bound_unchecked(_UCurrent + 1, _UNext + 1, _Val, _Pred);
        }
        else {
            _UCurrent = _UNext;
            _Step++;
            goto Next;
        }

        _Seek_wrapped(_First, _UCurrent);
        return _First;
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound_right(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_upper_bound_right(_First, _Last, _Val, _Hint, less<>{});
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound_left(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        _Adl_verify_range(_First, _Last);
        const auto _UFirst = _Get_unwrapped(_First);
        // const auto _ULast = _Get_unwrapped(_Last);
        auto _UCurrent = _Get_unwrapped(_Hint);
        if (!_Pred(_Val,*_UCurrent)) {
            _Seek_wrapped(_First, _UCurrent);
            return _First;
        }
        _Iter_diff_t<_FwdIt> _Step = -1;

        Next:
        const auto _UNext = _STD next(_UCurrent, _Step);
        if (_UNext < _UFirst) {
            _UCurrent = upper_bound_unchecked(_UFirst, _UCurrent, _Val, _Pred);
        }
        else if (_Pred( _Val,*_UNext)) {
            _UCurrent = _UNext;
            _Step--;
            goto Next;
        }
        else {
            _UCurrent = upper_bound_unchecked(_UNext + 1, _UCurrent, _Val, _Pred);
        }

        _Seek_wrapped(_First, _UCurrent);
        return _First;
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound_left(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_upper_bound_left(_First, _Last, _Val, _Hint, less<>{});
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        return _Pred(_Val, *_Hint) ?_STD exponential_upper_bound_left(_First, _Last, _Val, _Hint, _Pred) : _STD exponential_upper_bound_right(_First, _Last, _Val, _Hint, _Pred);
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_upper_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_upper_bound(_First, _Last, _Val, _Hint, less<>{});
    }

    template <class _UFwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _UFwdIt lower_bound_unchecked(_UFwdIt _UFirst, _UFwdIt _ULast, const _Ty& _Val, _Pr _Pred) {
        // find first element not before _Val
        _Iter_diff_t<_UFwdIt> _Count = _STD distance(_UFirst, _ULast);

        while (0 < _Count) { // divide and conquer, find half that contains answer
            const _Iter_diff_t<_UFwdIt> _Count2 = _Count / 2;
            const auto _UMid = _STD next(_UFirst, _Count2);
            if (_Pred(*_UMid, _Val)) { // try top half
                _UFirst = _Next_iter(_UMid);
                _Count -= _Count2 + 1;
            }
            else {
                _Count = _Count2;
            }
        }
        return _UFirst;
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound_right(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        _Adl_verify_range(_First, _Last);
        // const auto _UFirst = _Get_unwrapped(_First);
        const auto _ULast = _Get_unwrapped(_Last);
        auto _UCurrent = _Get_unwrapped(_Hint);
        if (!_Pred(*_UCurrent, _Val)) {
            _Seek_wrapped(_First, _UCurrent);
            return _First;
        }
        _Iter_diff_t<_FwdIt> _Step = 1;

        Next:
        const auto _UNext = _STD next(_UCurrent, _Step);
        if (_UNext >= _ULast) {
            _UCurrent = lower_bound_unchecked(_UCurrent + 1, _ULast, _Val, _Pred);
        }
        else if(_Pred(*_UNext, _Val)) {
            _UCurrent = _UNext;
            _Step++;
            goto Next;
        }
        else {
            _UCurrent = lower_bound_unchecked(_UCurrent + 1, _UNext + 1, _Val, _Pred);
        }

        _Seek_wrapped(_First, _UCurrent);
        return _First;
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound_right(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_lower_bound_right(_First, _Last, _Val, _Hint, less<>{});
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound_left(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        _Adl_verify_range(_First, _Last);
        const auto _UFirst = _Get_unwrapped(_First);
        // const auto _ULast = _Get_unwrapped(_Last);
        auto _UCurrent = _Get_unwrapped(_Hint);
        if (_Pred(*_UCurrent, _Val)) {
            _Seek_wrapped(_First, _UCurrent);
            return _First;
        }
        _Iter_diff_t<_FwdIt> _Step = -1;

        Next:
        const auto _UNext = _STD next(_UCurrent, _Step);
        if (_UNext < _UFirst) {
            _UCurrent = lower_bound_unchecked(_UFirst, _UCurrent, _Val, _Pred);
        }
        else if (_Pred(*_UNext, _Val)) {
            _UCurrent = lower_bound_unchecked(_UNext + 1, _UCurrent, _Val, _Pred);
        }
        else {
            _UCurrent = _UNext;
            _Step--;
            goto Next;
        }

        _Seek_wrapped(_First, _UCurrent);
        return _First;
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound_left(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_lower_bound_left(_First, _Last, _Val, _Hint, less<>{});
    }

    template <class _FwdIt, class _Ty, class _Pr>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint, _Pr _Pred) {
        if (_First == _Last) return _Last;
        // find first element not before _Val
        return _Pred(*_Hint, _Val) ? _STD exponential_lower_bound_right(_First, _Last, _Val, _Hint, _Pred) : _STD exponential_lower_bound_left(_First, _Last, _Val, _Hint, _Pred);
    }

    template <class _FwdIt, class _Ty>
    _NODISCARD _CONSTEXPR20 _FwdIt exponential_lower_bound(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, const _FwdIt _Hint) {
        // find first element not before _Val
        return _STD exponential_lower_bound(_First, _Last, _Val, _Hint, less<>{});
    }
}
