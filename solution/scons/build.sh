#!/bin/sh
#
# Script to ...
#

clear

#export OS_NAME='uname'

uname
export OS_NAME="$(uname)"
echo OS_NAME: $OS_NAME
                                
if [ $GCC_VERSION = "" ]
then
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 OS_NAME=$OS_NAME
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 OS_NAME=$OS_NAME install
else
	#TODO: el compilador está harcodearo
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION OS_NAME=$OS_NAME
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION OS_NAME=$OS_NAME install
fi
