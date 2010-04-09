@echo off
cls      

if "%MSVC_VERSION%" == "" goto default

scons PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=%MSVC_VERSION%

:default
scons PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=9.0
goto :eof








REM echo HOLAAAAAAAAAAAAAAAAAAaa
REM 
REM scons env="PATH:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin;C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools,INCLUDE:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include;C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include,LIB:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Lib;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib"
REM 
REM echo HOLAAAAAAAAAAAAAAAAAAaa
REM 
REM 
REM 
REM REM scons
REM REM scons env="PATH:%PATH%,INCLUDE:%INCLUDE%,LIB:%LIB%"
REM 
REM 
REM REM PATH:
REM REM 	C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin;
REM REM	C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE;
REM REM	C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;
REM REM	C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools,
REM REM INCLUDE:
REM REM 	C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include;
REM REM 	C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Include;
REM REM 	C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include,
REM REM LIB:
REM REM 	C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\lib;
REM REM 	C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Lib;
REM REM 	C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib 
REM 
REM 
REM 
REM REM scons env="PATH:C:\Program Files\Microsoft Visual Studio 9.0\VC\bin;C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools,INCLUDE:C:\Program Files\Microsoft Visual Studio 9.0\VC\include;C:\Program Files\Microsoft SDKs\Windows\v6.1\Include,LIB:C:\Program Files\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib 