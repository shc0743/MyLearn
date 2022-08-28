@echo off
Cd /d "%~dp0"
set svc=My Remote Terminal
timeout/t 10
..\..\bin\myremoteterminal64 --type=update --updateSource="..\..\bin\myremoteterminal64.exe" --targetService="%svc%"
echo %ERRORLEVEL%
timeout 5
