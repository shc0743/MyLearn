@echo off
Cd /d "%~dp0"
set svc=My Remote Terminal
call pack-update_version
call pack
call zOneKeyDevelopUpdate

