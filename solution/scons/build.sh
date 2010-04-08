#!/bin/sh
#
# Script to ...
#

#TODO: 



#if [ $# -eq 0 ]
#then
#echo "$0 : You must give/supply one integers"
#exit 1
#fi


#VAR=${VAR:=default_value}
#echo $VAR


if [ $GCC_VERSION == ""  ]
then
	echo "GCC_VERSION == ''"
	exit 1
fi


if [ $GCC_VERSION != ""  ]
then
	echo "GCC_VERSION != ''"
	exit 1
fi



clear

#if "%GCC_VERSION%" == "" goto default

scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4

#:default
#scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=vc COMPILER_VERSION=9.0
#goto :eof



