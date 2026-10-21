/*************************************************************************************************
*
* File: Misc.cpp
*
* Description: Implements small helper objects
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include <Windows.h>
#include <Wincrypt.h>
#include <tchar.h>
#include <algorithm>
#include <Misc.h>

//-----------------------------------------------------------------------------
// The Timer class
//-----------------------------------------------------------------------------

DeltaPerformanceCounter::DeltaPerformanceCounter() {
    LARGE_INTEGER Temp = {};
    QueryPerformanceFrequency(&Temp);
    TimerResolution = Temp.QuadPart;
    QueryRealDeltaPerformanceCounter();
}

mms_t DeltaPerformanceCounter::QueryDeltaPerformanceCounter() {
    if (ConstantDelta) return ConstantDelta;
    else return QueryRealDeltaPerformanceCounter();
};

mms_t DeltaPerformanceCounter::QueryRealDeltaPerformanceCounter() {
    LARGE_INTEGER Temp = {};
    QueryPerformanceCounter(&Temp);
    mms_t Delta = Temp.QuadPart - LastQuery;
    LastQuery = Temp.QuadPart;
    return Delta;
};

//-----------------------------------------------------------------------------
// Small utility functions
//-----------------------------------------------------------------------------


wstring Util::Utf8ToWstring(const string& u8str)
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

string Util::WstringToUtf8(const wstring& wstr)
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

color_t Util::RandColor()
{
    color_t R, G, B;
    HSVtoRGB(rand() % 360, rand() % 40 + 60, rand() % 20 + 80, R, G, B);
    return (0x00 << 24) | (B << 16) | (G << 8) | R;
}

void Util::RGBtoHSV(color_t R, color_t G, color_t B, color_t& H, color_t& S, color_t& V)
{
    double dR = R / 255.0, dG = G / 255.0, dB = B / 255.0;
    double M = max(max(dR, dG), dB);
    double m = min(min(dR, dG), dB);
    double C = M - m;

    double dH = C == 0 ? 0.0 :
        M == dR ? (dG - dB) / C :
        M == dG ? (dB - dR) / C + 2 :
        M == dB ? (dR - dG) / C + 4 : 0.0;
    if (dH < 0) dH += 6.0;
    double dV = M;
    double dS = (dV > 0.0 ? C / dV : 0.0);

    H = static_cast<color_t>(dH * 60.0 + 0.5) % 360;
    V = static_cast<color_t>(dV * 100.0 + 0.5);
    S = static_cast<color_t>(dS * 100.0 + 0.5);
}

void Util::HSVtoRGB(color_t H, color_t S, color_t V, color_t& R, color_t& G, color_t& B)
{
    double dH = H / 60.0, dS = S / 100.0, dV = V / 100.0;
    double C = dV * dS;
    double m = dV - C;

    double dR1 = 0.0, dG1 = 0.0, dB1 = 0.0;
    if (dH >= 0 && dH < 1.0)
    {
        dR1 = C;
        dG1 = C * dH;
        dB1 = 0;
    }
    if (dH >= 1 && dH < 2.0)
    {
        dR1 = C * (2.0 - dH);
        dG1 = C;
        dB1 = 0;
    }
    if (dH >= 2 && dH < 3.0)
    {
        dR1 = 0;
        dG1 = C;
        dB1 = C * (dH - 2.0);
    }
    if (dH >= 3 && dH < 4.0)
    {
        dR1 = 0;
        dG1 = C * (4.0 - dH);
        dB1 = C;
    }
    if (dH >= 4 && dH < 5.0)
    {
        dR1 = C * (dH - 4.0);
        dG1 = 0;
        dB1 = C;
    }
    if (dH >= 5 && dH < 6.0)
    {
        dR1 = C;
        dG1 = 0;
        dB1 = C * (6.0 - dH);
    }

    R = static_cast<color_t>((dR1 + m) * 255.0 + 0.5);
    G = static_cast<color_t>((dG1 + m) * 255.0 + 0.5);
    B = static_cast<color_t>((dB1 + m) * 255.0 + 0.5);
}


void Util::EncodeHSLShift(signed short H, signed char S, signed char L, color_t* Packed)
{
    if (H < 0) {
        H *= -1;
        *reinterpret_cast<unsigned short*>(&H) += 180;
    }
    if (S < 0) {
        S *= -1;
        *reinterpret_cast<unsigned char*>(&S) += 100;
    }
    if (L < 0) {
        L *= -1;
        *reinterpret_cast<unsigned char*>(&L) += 100;
    }
    *Packed = 0;
    *Packed += *reinterpret_cast<unsigned char*>(&L) % 201;
    *Packed *= 201;
    *Packed += *reinterpret_cast<unsigned char*>(&S) % 201;
    *Packed *= 361;
    *Packed += *reinterpret_cast<unsigned short*>(&H) % 361;
}

void Util::DecodeHSLShift(color_t Packed, signed short* H, signed char* S, signed char* L)
{
    *reinterpret_cast<unsigned short*>(H) = Packed % 361;
    Packed /= 361;
    *reinterpret_cast<unsigned char*>(S) = Packed % 201;
    Packed /= 201;
    *reinterpret_cast<unsigned char*>(L) = Packed % 201;
    if (*reinterpret_cast<unsigned short*>(H) > 180) {
        *reinterpret_cast<unsigned short*>(H) -= 180;
        *H *= -1;
    }
    if (*reinterpret_cast<unsigned char*>(S) > 100) {
        *reinterpret_cast<unsigned char*>(S) -= 100;
        *S *= -1;
    }
    if (*reinterpret_cast<unsigned char*>(L) > 100) {
        *reinterpret_cast<unsigned char*>(L) -= 100;
        *L *= -1;
    }
}
