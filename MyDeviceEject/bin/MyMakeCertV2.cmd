@echo off
REM //TODO
echo Bugful
pause
exit /b

echo Notice: Please run in vsdevcmd
set /p YourName=YourName=
set /p Begin=Begin=(MM/dd/yyyy)
set /p End=End=(MM/dd/yyyy)

rem 将日期格式转换为openssl可接受的格式
for /f "tokens=1,2,3 delims=/" %%a in ("%Begin%") do set Begin=%%c%%a%%b
for /f "tokens=1,2,3 delims=/" %%a in ("%End%") do set End=%%c%%a%%b

rem 生成私钥
openssl genpkey -algorithm RSA -out %YourName%.key -pkeyopt rsa_keygen_bits:4096

rem 生成证书签名请求 (CSR)
openssl req -new -key %YourName%.key -out %YourName%.csr -subj "/CN=%YourName%"

rem 生成自签名证书
openssl x509 -req -in %YourName%.csr -signkey %YourName%.key -out %YourName%.crt -days 365 -startdate %Begin%000000Z -enddate %End%235959Z

rem 生成PFX文件
openssl pkcs12 -export -out %YourName%.pfx -inkey %YourName%.key -in %YourName%.crt -password pass:

set PFX_FILE=%YourName%.pfx
sign
