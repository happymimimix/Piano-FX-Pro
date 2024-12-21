@echo off
cd /D "%~dp0"
title Fl Studio 12.3 Debloater
echo|set/p=[2J[H[1m[92m[40m
echo.Fl Studio 12.3 Debloater v1.01 (Typical)
echo.Made by: Happy_mimimix
echo.
IF NOT EXIST ".\FL.exe" (
echo.[91mPlease put this script inside Fl Studio's installation directory! 
echo.
pause
exit
)
echo.[91mNotice: We strongly recommend that you check the source code of this tool by right-clicking and edit to prevent it from accidentally deleting some components that are important to you. 
echo.
pause
echo|set/p=[93m
echo.Removing IL Downloader... 
"..\Downloader\uninstall.exe" /S
del "..\Downloader" /f /s /q
rmdir "..\Downloader" /s /q
echo.Removing unecessary templates and presets... 
del ".\Data\Patches\Channel presets" /f /s /q
rmdir ".\Data\Patches\Channel presets" /s /q
del ".\Data\Patches\Clipboard files" /f /s /q
rmdir ".\Data\Patches\Clipboard files" /s /q
del ".\Data\Patches\Collected" /f /s /q
rmdir ".\Data\Patches\Collected" /s /q
del ".\Data\Patches\Envelopes" /f /s /q
rmdir ".\Data\Patches\Envelopes" /s /q
del ".\Data\Patches\Impulses" /f /s /q
rmdir ".\Data\Patches\Impulses" /s /q
del ".\Data\Patches\Misc" /f /s /q
rmdir ".\Data\Patches\Misc" /s /q
del ".\Data\Patches\Mixer presets" /f /s /q
rmdir ".\Data\Patches\Mixer presets" /s /q
del ".\Data\Patches\Packs" /f /s /q
rmdir ".\Data\Patches\Packs" /s /q
del ".\Data\Patches\Projects bones" /f /s /q
rmdir ".\Data\Patches\Projects bones" /s /q
del ".\Data\Patches\Recorded" /f /s /q
rmdir ".\Data\Patches\Recorded" /s /q
del ".\Data\Patches\Rendered" /f /s /q
rmdir ".\Data\Patches\Rendered" /s /q
del ".\Data\Patches\Scores" /f /s /q
rmdir ".\Data\Patches\Scores" /s /q
del ".\Data\Patches\Sliced beats" /f /s /q
rmdir ".\Data\Patches\Sliced beats" /s /q
del ".\Data\Patches\Soundfonts" /f /s /q
rmdir ".\Data\Patches\Soundfonts" /s /q
del ".\Data\Patches\Speech" /f /s /q
rmdir ".\Data\Patches\Speech" /s /q
del ".\Data\Patches\User" /f /s /q
rmdir ".\Data\Patches\User" /s /q
del ".\Data\Projects" /f /s /q
rmdir ".\Data\Projects" /s /q
mkdir ".\Data\Projects\Templates"
echo.Removing unecessary plugins... 
xcopy ".\Plugins\Fruity\Generators\MIDI Out" ".\Plugins\Fruity\MIDI Out" /c /e /v /r /y /k /i /g
del ".\Plugins\Fruity\Generators" /f /s /q
rmdir ".\Plugins\Fruity\Generators" /s /q
mkdir ".\Plugins\Fruity\Generators"
xcopy ".\Plugins\Fruity\MIDI Out" ".\Plugins\Fruity\Generators\MIDI Out" /c /e /v /r /y /k /i /g
del ".\Plugins\Fruity\MIDI Out" /f /s /q
rmdir ".\Plugins\Fruity\MIDI Out" /s /q
del ".\Plugins\VST" /f /s /q
rmdir ".\Plugins\VST" /s /q
del ".\System\Plugin" /f /s /q
rmdir ".\System\Plugin" /s /q
del ".\AudioRestore.dll" /f /q
del ".\DelZip179.dll" /f /q
del ".\ds2wav.dll" /f /q
del ".\dsplib.dll" /f /q
del ".\iZAudioRestore.dll" /f /q
del ".\speaker.dll" /f /q
del ".\speaker_x64.dll" /f /q
del ".\ss2wav.dll" /f /q
del ".\ss2wav16.dll" /f /q
del ".\ZeroX_AS.dll" /f /q
del "..\Shared\ffmpeg" /f /s /q
rmdir "..\Shared\ffmpeg" /s /q
del "..\Shared\DelZip190.dll" /f /q
del "..\Shared\DirectWaveCtEngine.dll" /f /q
del "..\Shared\DirectWaveCtEngine_x64.dll" /f /q
del "..\Shared\dsp_ipp.dll" /f /q
del "..\Shared\Elastique.dll" /f /q
del "..\Shared\freetype.dll" /f /q
del "..\Shared\ILMinihostBridge32.exe" /f /q
del "..\Shared\ILMinihostBridge64.exe" /f /q
del "..\Shared\ILPluginScanner32.exe" /f /q
del "..\Shared\ILPluginScanner64.exe" /f /q
del "..\Shared\ILRemoteServer.dll" /f /q
del "..\Shared\ILRemoteServer_x64.dll" /f /q
del "..\Shared\LAMEenc.dll" /f /q
del "..\Shared\LAMEenc_x64.dll" /f /q
del "..\Shared\oggio.dll" /f /q
del "..\Shared\oggio_x64.dll" /f /q
del "..\Shared\QuickFontCache.dll" /f /q
del "..\Shared\Reverb.dll" /f /q
del "..\Shared\REX Shared Library.dll" /f /q
del "..\Shared\REX Shared Library_x64.dll" /f /q
del "..\Shared\SG.dll" /f /q
del "..\Shared\SG_x64.dll" /f /q
del "..\Shared\steam_api.dll" /f /q
del "..\Shared\steam_api64.dll" /f /q
del "..\Shared\svctl32.dll" /f /q
del "..\Shared\svctl64.dll" /f /q
del "..\Shared\sveng32.dll" /f /q
del "..\Shared\sveng64.dll" /f /q
echo.Removing unecessary tools and configurations... 
del ".\System\Tools" /f /s /q
rmdir ".\System\Tools" /s /q
del ".\System\Texts" /f /s /q
rmdir ".\System\Texts" /s /q
del ".\System\Legal" /f /s /q
rmdir ".\System\Legal" /s /q
del ".\System\Internet" /f /s /q
rmdir ".\System\Internet" /s /q
del ".\System\Installers" /f /s /q
rmdir ".\System\Installers" /s /q
del ".\System\Hardware specific" /f /s /q
rmdir ".\System\Hardware specific" /s /q
del ".\System\Config\Typing to piano" /f /s /q
rmdir ".\System\Config\Typing to piano" /s /q
del ".\System\Config\Screenshots" /f /s /q
rmdir ".\System\Config\Screenshots" /s /q
del ".\System\Config\Mapping" /f /s /q
rmdir ".\System\Config\Mapping" /s /q
del ".\System\Config\Browser" /f /s /q
rmdir ".\System\Config\Browser" /s /q
echo.Removing unecessary font license files... (Will anyone ever wanna read them?)
del "..\Shared\Artwork\Fonts\Cuprum" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Cuprum" /s /q
del "..\Shared\Artwork\Fonts\Francois One" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Francois One" /s /q
del "..\Shared\Artwork\Fonts\Italianno" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Italianno" /s /q
del "..\Shared\Artwork\Fonts\Josefin_Sans" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Josefin_Sans" /s /q
del "..\Shared\Artwork\Fonts\Khand" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Khand" /s /q
del "..\Shared\Artwork\Fonts\Open_Sans_Condensed" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Open_Sans_Condensed" /s /q
del "..\Shared\Artwork\Fonts\Oswald" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Oswald" /s /q
del "..\Shared\Artwork\Fonts\PT Sans" /f /s /q
rmdir "..\Shared\Artwork\Fonts\PT Sans" /s /q
del "..\Shared\Artwork\Fonts\Revalia" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Revalia" /s /q
del "..\Shared\Artwork\Fonts\Roboto_Condensed" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Roboto_Condensed" /s /q
del "..\Shared\Artwork\Fonts\Share" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Share" /s /q
del "..\Shared\Artwork\Fonts\Ubuntu_Condensed" /f /s /q
rmdir "..\Shared\Artwork\Fonts\Ubuntu_Condensed" /s /q
del "..\Shared\Artwork\Fonts\WebSymbols-Regular" /f /s /q
rmdir "..\Shared\Artwork\Fonts\WebSymbols-Regular" /s /q
echo.Removing unecessary fonts and skins... 
del ".\Artwork\Fonts" /f /s /q
rmdir ".\Artwork\Fonts" /s /q
del ".\Artwork\PlaylistBackground.png" /f /q
del ".\Artwork\PlugList.bmp" /f /q
del ".\Artwork\Skins\Silent startup" /f /s /q
rmdir ".\Artwork\Skins\Silent startup" /s /q
del ".\Artwork\Skins\Default\BigFruit.png" /f /q
del ".\Artwork\Skins\Default\ClipHanger.png" /f /q
del ".\Artwork\Skins\Default\config.txt" /f /q
del ".\Artwork\Skins\Default\PickerChan.png" /f /q
del ".\Artwork\Skins\Default\StartSnd.wav" /f /q
echo.Removing unecessary data files... 
del "%userprofile%\Documents\Image-Line" /f /s /q
rmdir "%userprofile%\Documents\Image-Line" /s /q
del "..\Shared\Temp" /f /s /q
rmdir "..\Shared\Temp" /s /q
echo.No more 32-bit Fl Studio! 
del ".\FLEngine.dll" /f /q
del ".\FL.exe" /f /q
del ".\FL (scaled).exe" /f /q
del ".\FL (compatible memory).exe" /f /q
mklink ".\FL.exe" ".\FL64.exe"
mklink ".\FL (scaled).exe" ".\FL64 (scaled).exe"
