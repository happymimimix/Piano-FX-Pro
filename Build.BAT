CD /d "%~dp0"
del .\.vs /f /s /q
rmdir .\.vs /s /q
del .\Release /f /s /q
rmdir .\Release /s /q
del .\PianoFromAbove\Release /f /s /q
rmdir .\PianoFromAbove\Release /s /q
del .\Piano-FX-Pro.exe /f /q
del .\Piano-FX-Pro-SoftwareRendered.exe /f /q
del .\Piano-FX-Pro-CHN.exe /f /q
del .\Piano-FX-Pro-ARN.exe /f /q
del .\Piano-FX-Pro-JPN.exe /f /q
del .\Piano-FX-Pro-KOR.exe /f /q
del .\Piano-FX-Pro-RUS.exe /f /q
del .\Piano-FX-Pro-DRN.exe /f /q
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Release /p:Platform=x64
copy .\Release\Piano-FX-Pro.exe .\Piano-FX-Pro.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=EnforceSoftwareRender /p:Platform=x64
copy .\Release\Piano-FX-Pro-SoftwareRendered.exe .\Piano-FX-Pro-SoftwareRendered.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Chinese /p:Platform=x64
copy .\Release\Piano-FX-Pro-CHN.exe .\Piano-FX-Pro-CHN.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Arinoglish /p:Platform=x64
copy .\Release\Piano-FX-Pro-ARN.exe .\Piano-FX-Pro-ARN.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Japanese /p:Platform=x64
copy .\Release\Piano-FX-Pro-JPN.exe .\Piano-FX-Pro-JPN.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Korean /p:Platform=x64
copy .\Release\Piano-FX-Pro-KOR.exe .\Piano-FX-Pro-KOR.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=Russian /p:Platform=x64
copy .\Release\Piano-FX-Pro-RUS.exe .\Piano-FX-Pro-RUS.exe /v /y
"%~dp1msbuild.exe" .\Piano-FX-Pro.sln -t:Rebuild -p:Configuration=DoubleArinoglish /p:Platform=x64
copy .\Release\Piano-FX-Pro-DRN.exe .\Piano-FX-Pro-DRN.exe /v /y
del .\.vs /f /s /q
rmdir .\.vs /s /q
del .\Release /f /s /q
rmdir .\Release /s /q
del .\PianoFromAbove\Release /f /s /q
rmdir .\PianoFromAbove\Release /s /q
