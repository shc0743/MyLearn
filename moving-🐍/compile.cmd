@echo off
chcp 65001

where cl >nul
if %errorlevel% neq 0 (
    REM Call the Visual Studio Developer Command Prompt
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"  -startdir=none -arch=x64 -host_arch=x64
)

cd /d %~dp0
@REM rc 🐍.rc
@REM cl /source-charset:utf-8 deepseek_cpp_20250502_ec3cd0.cpp /link /MANIFEST:EMBED
cl /source-charset:utf-8 /std:c++20 🐍.cpp /link /MANIFEST:EMBED
