@echo off
where cl >nul
if %errorlevel% neq 0 (
    REM Call the Visual Studio Developer Command Prompt
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"  -startdir=none -arch=x64 -host_arch=x64
    if %errorlevel% neq 0 (
        call D:\software\VisualStudio\ms\versions\2022\Community\Common7\Tools\VsDevCmd.bat  -startdir=none -arch=x64 -host_arch=x64
    )
)
cl.exe SymLinkCreator.cpp /EHsc /MT /D_UNICODE /DUNICODE /std:c++20 /link /SUBSYSTEM:WINDOWS /MANIFEST:EMBED /MACHINE:X64
cmd /c C:\Users\shc07\OneDrive\self_cert\sign_auto.cmd %~dp0\SymLinkCreator.exe
timeout 3