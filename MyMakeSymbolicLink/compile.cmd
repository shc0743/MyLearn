@echo off
where cl >nul
if %errorlevel% neq 0 (
    REM Call the Visual Studio Developer Command Prompt
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)
cl.exe SymLinkCreator.cpp /EHsc /D_UNICODE /DUNICODE /link /SUBSYSTEM:WINDOWS /MANIFEST:EMBED
@REM cl.exe /nologo /W4 /EHsc /D_UNICODE /DUNICODE /D_CRT_SECURE_NO_WARNINGS ^
@REM /FoSymLinkCreator.obj SymLinkCreator.cpp ^
@REM /link /OUT:SymLinkCreator.exe /SUBSYSTEM:WINDOWS /MANIFEST:embed ^
@REM comctl32.lib comdlg32.lib shell32.lib ole32.lib user32.lib
timeout 3