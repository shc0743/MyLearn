@echo off
Cd /d "%~dp0"
set svc=MyRemoteTerminalService_dce5be09
sc stop "%svc%"
call pack-update_version
call pack
timeout/t 10
sc start "%svc%"
