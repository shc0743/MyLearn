@echo off
echo Notice: Please run in vsdevcmd
set /p YourName=YourName=
set /p Begin=Begin=(MM/dd/yyyy)
set /p End=End=(MM/dd/yyyy)
makecert -r -pe -n "CN=%YourName%" -b %Begin% -e %End% -sky signature -sv %YourName%.pvk %YourName%.cer -len 4096
pvk2pfx -pvk %YourName%.pvk -spc %YourName%.cer -pfx %YourName%.pfx -po ""

set PFX_FILE=%YourName%.pfx
sign
