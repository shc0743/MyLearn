@cd /d "%~dp0"
@set File=html_content.7z
@move "%File%" "%File%1" 1>nul 2>&1
@7za a -y -- "%file%" html resources html_pack_version.txt
@del "%File%1" 1>nul 2>&1
