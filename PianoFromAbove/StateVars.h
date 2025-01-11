#pragma once

#include <Windows.h>
using namespace std;

inline long long m_llStartTime;
inline string llStartTimeFormatted;
inline uint32_t polyphony;
inline string polyFormatted;
inline uint32_t nps;
inline string npsFormatted;
inline uint32_t passed;
inline string passedFormatted;
inline uint8_t FrameCount = 0;
inline int width = -1;
inline int height = -1;
inline int m_iStartTick;
inline uint16_t resolution = -1;
inline char CheatEngineCaption[1 << 10] = {};