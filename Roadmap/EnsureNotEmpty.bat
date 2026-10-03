cd /d "%~dp0"
for /d /r %%d in (*) do (type nul >"%%~fd\_")