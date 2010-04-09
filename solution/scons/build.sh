#!/bin/bash
#
# Script to ...
#

clear

if [ $GCC_VERSION == "" ]; then
	#echo "GCC_VERSION == ''"
	scons -Q -c PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4
else
	#echo "GCC_VERSION != ''"
	#TODO: el compilador está harcodearo
	scons -Q -c PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION
fi

