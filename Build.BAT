CD /d "%~dp0"
del .\.vs /f /s /q
rmdir .\.vs /s /q
del .\Release /f /s /q
rmdir .\Release /s /q
del .\PianoFromAbove\Release /f /s /q
rmdir .\PianoFromAbove\Release /s /q
del .\Piano-FX-Pro.exe /f /q
del .\Piano-FX-Pro-SoftwareRendered.exe /f /q
msbuild .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Release /p:Platform=x64
copy .\Release\Piano-FX-Pro.exe .\Piano-FX-Pro.exe /v /y
msbuild .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=EnforceSoftwareRender /p:Platform=x64
copy .\Release\Piano-FX-Pro-SoftwareRendered.exe .\Piano-FX-Pro-SoftwareRendered.exe /v /y
del .\.vs /f /s /q
rmdir .\.vs /s /q
del .\Release /f /s /q
rmdir .\Release /s /q
del .\PianoFromAbove\Release /f /s /q
rmdir .\PianoFromAbove\Release /s /q