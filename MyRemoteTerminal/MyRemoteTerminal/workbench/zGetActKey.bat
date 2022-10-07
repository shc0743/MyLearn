@cd /d "%~dp0"
@if "%svc%"=="" set "svc=My Remote Terminal"
@start "" "..\..\bin\MyRemoteTerminal64.exe" --type=service-activator --name="%svc%" --interactive
@REM @timeout 3