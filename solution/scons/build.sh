#!/bin/sh
#
# Script to ...
#

clear

export SYSTEM_NAME='uname'

echo "SYSTEM_NAME: " + $SYSTEM_NAME

if [ $GCC_VERSION == "" ]; then
	#echo "GCC_VERSION == ''"
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 SYSTEM=$SYSTEM_NAME
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 SYSTEM=$SYSTEM_NAME install
else
	#echo "GCC_VERSION != ''"
	#TODO: el compilador est� harcodearo
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION SYSTEM=$SYSTEM_NAME
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION SYSTEM=$SYSTEM_NAME install
fi

