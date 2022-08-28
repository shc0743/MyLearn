@set /p v=<html_pack_version.txt
@set /a v=%v%+1
@echo %v% >html_pack_version.txt
