cd /d "%~dp0"
chcp 65001
tree "%~dp0." >Tree_OEM.txt
powershell Get-Content Tree_OEM.txt -Encoding oem ^| Set-Content Tree.txt -Encoding utf8
start "Roadmap" notepad.exe Tree.txt