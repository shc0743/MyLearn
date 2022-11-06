@echo off

set inpath=.\bin\Release\net6.0-windows8.0
set outpath=..\bin\


echo [Deploy] Preparing for deploy
set result=0



echo [Deploy] Deploying...

xcopy /s /y "%inpath%\*" "%outpath%\"
set result=%ERRORLEVEL%
if not "%RESULT%"=="0" goto failed


goto succeed



:succeed
echo [Deploy] Deploied successfully
exit /b 0

:failed
echo [Deploy] Failed to deploy, error %result%
exit /b %result%

