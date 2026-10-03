cd /d "%~dp0"
chcp 65001
tree >Tree.txt
start "Roadmap" notepad.exe Tree.txt