@echo OFF

REM ::::::::::::::::::::::::::::::::
REM ::                            ::
REM ::    Unnamed Application     ::
REM ::                by shc0743  ::
REM ::                            ::
REM ::                            ::
REM ::  (Just a console to test)  ::
REM ::  ( the Captcha Embedded )  ::
REM ::                            ::
REM ::                       :-)  ::
REM ::                            ::
REM ::::::::::::::::::::::::::::::::


chcp 65001 >nul 2>&1


rem ::AppMain
:main
echo [Unnamed Application]
start /wait rundll32.exe MyCaptchaEmbedded64.dll,CheckCaptcha_RunDLL
if "%ERRORLEVEL%"=="1" call :NOT_ACTIVATED
if "%ERRORLEVEL%"=="0" call :ACTIVATED

set /p userquestion=Type what you want to know about: 
echo Querying... [Your query-id is %random%]
ping 127.0.0.1 -n 5 >nul 2>&1
echo Query successfully!
echo Result:
echo   "%userquestion%" is "%userquestion%"
pause










:quit
exit /b
















:NOT_ACTIVATED
echo [Activation] You didn't activate the application!!
choice /c YN /M "[Activation] Do you want to activate?"
if "%ERRORLEVEL%"=="2" exit -1
start /wait rundll32.exe MyCaptchaEmbedded64.dll,RunCaptcha_RunDLL /plan random --center-foreground-window
if "%ERRORLEVEL%"=="0" exit /b
echo [Activation] You failed the CAPTCHA!!!
pause & exit -2
exit /b




:ACTIVATED
echo [Company Genuine Verification] This copy is genuine
exit /b













