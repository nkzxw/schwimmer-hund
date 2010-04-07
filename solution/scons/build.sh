#TODO: 


clear

#if "%GCC_VERSION%" == "" goto default

scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4

#:default
#scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=9.0
#goto :eof



