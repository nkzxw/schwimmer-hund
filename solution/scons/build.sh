#!/bin/sh
#
# Script to ...
#

clear

#export SYSTEM_NAME='uname'

uname
export SYSTEM_NAME="$(uname)"
echo SYSTEM_NAME: $SYSTEM_NAME
                                
if ( 'a' == 'a' )
then
	echo dentro del if
fi

                                
                                
#if [ $GCC_VERSION == "" ]; then
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 SYSTEM=$SYSTEM_NAME
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 SYSTEM=$SYSTEM_NAME install
#else
#	#TODO: el compilador está harcodearo
#	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION SYSTEM=$SYSTEM_NAME
#	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION SYSTEM=$SYSTEM_NAME install
#fi
