#!/bin/sh
#
# Script to ... #TODO
#

clear

#uname
#export OS_NAME="$(uname)"
##echo OS_NAME: $OS_NAME

cd ~/dev/schwimmer-hund/
#cd ../..
svn update
#cd solution/scons
cd ~/dev/schwimmer-hund/solutions/scons

chmod a+x config.sh
./config.sh


#TODO: ver si se puede sacar la version de GCC por ac�... o por el scons
                                
if [ $GCC_VERSION = "" ]
then
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=4.4 install
else
	#TODO: el compilador est� harcodearo
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION
	scons -Q PROJECT_NAME=schwimmer-hund COMPILER_NAME=gcc COMPILER_VERSION=$GCC_VERSION install
fi
