@echo off
cls      

if "%MSVC_VERSION%" == "" goto default

@REM call scons -c

cd ..\..
svn update
cd solutions\scons

@REM TODO: esta fijo "vc" en este caso y no debería estarlo. AGREGAR UN COMPILER_NAME
call scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=%MSVC_VERSION% 
call scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=%MSVC_VERSION% install

goto :eof

:default
call scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=9.0
call scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=9.0 install

goto :eof








REM echo HOLAAAAAAAAAAAAAAAAAAaa
REM 
REM call scons env="PATH:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin;C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools,INCLUDE:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\include;C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include,LIB:C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files (x86)\Microsoft SDKs\Windows\v6.0A\Lib;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib"
REM 
REM echo HOLAAAAAAAAAAAAAAAAAAaa
REM 
REM 
REM 
REM REM call scons
REM REM call scons env="PATH:%PATH%,INCLUDE:%INCLUDE%,LIB:%LIB%"
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
REM REM call scons env="PATH:C:\Program Files\Microsoft Visual Studio 9.0\VC\bin;C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools,INCLUDE:C:\Program Files\Microsoft Visual Studio 9.0\VC\include;C:\Program Files\Microsoft SDKs\Windows\v6.1\Include,LIB:C:\Program Files\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib 



echo HOLA-11