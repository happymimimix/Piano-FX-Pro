#include <PFXSTUDIO_Global_Imports.h>
#include <Colorizer.exe.h>
#include <PFXGDI.exe.h>

string ProgramDIR() {
    char szFilePath[MAX_PATH + 1] = {};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0;
    return szFilePath;
}

void ExtractEmbededFiles() {
    string ColorizerPath = ProgramDIR() + "\\Colorizer.exe";
    if (!filesystem::exists(ColorizerPath)) {
        ofstream ColorizerFile(ColorizerPath, ios::binary);
        ColorizerFile.write(reinterpret_cast<const char*>(Colorizer_exe), Colorizer_exe_len);
        ColorizerFile.close();
    }
    string PFXGDIPath = ProgramDIR() + "\\PFXGDI.exe";
    if (!filesystem::exists(PFXGDIPath)) {
        ofstream PFXGDIFile(PFXGDIPath, ios::binary);
        PFXGDIFile.write(reinterpret_cast<const char*>(PFXGDI_exe), PFXGDI_exe_len);
        PFXGDIFile.close();
    }
}