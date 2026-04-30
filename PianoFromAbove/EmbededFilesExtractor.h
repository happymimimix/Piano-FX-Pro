#include <PFXSTUDIO_Global_Imports.h>
#ifdef INCLUDE_FFMPEG
#include <Colorizer.exe.h>
#include <PFXGDI.exe.h>
#endif
#define LONG_MAX_PATH 0x0FFF

wstring ProgramDIR() {
    wchar_t szFilePath[LONG_MAX_PATH] = {};
    GetModuleFileNameW(NULL, szFilePath, LONG_MAX_PATH);
    *wcsrchr(szFilePath, '\\') = 0;
    return szFilePath;
}

void ExtractEmbededFiles() {
#ifdef INCLUDE_FFMPEG
    wstring ColorizerPath = ProgramDIR() + L"\\Colorizer.exe";
    if (!filesystem::exists(ColorizerPath)) {
        ofstream ColorizerFile(ColorizerPath, ios::binary);
        ColorizerFile.write(reinterpret_cast<const char*>(Colorizer_exe), Colorizer_exe_len);
        ColorizerFile.close();
    }
    wstring PFXGDIPath = ProgramDIR() + L"\\PFXGDI.exe";
    if (!filesystem::exists(PFXGDIPath)) {
        ofstream PFXGDIFile(PFXGDIPath, ios::binary);
        PFXGDIFile.write(reinterpret_cast<const char*>(PFXGDI_exe), PFXGDI_exe_len);
        PFXGDIFile.close();
    }
#endif
}