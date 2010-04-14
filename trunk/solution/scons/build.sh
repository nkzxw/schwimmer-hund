#!/bin/sh
#
# Script to ... #TODO
#

clear

#uname
#export OS_NAME="$(uname)"
##echo OS_NAME: $OS_NAME

cd ../..
svn update
cd solution/scons


#TODO: ver si se puede sacar la version de GCC por acá... o por el scons
                                
if [ $GCC_VERSION = "" ]
then
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 install
else
	#TODO: el compilador está harcodearo
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION install
fi
