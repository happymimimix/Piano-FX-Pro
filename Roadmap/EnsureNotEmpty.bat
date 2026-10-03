cd /d "%~dp0"
chcp 65001
for /d /r "%~dp0" %%d in (*) do (type nul >"%%~fd\_")