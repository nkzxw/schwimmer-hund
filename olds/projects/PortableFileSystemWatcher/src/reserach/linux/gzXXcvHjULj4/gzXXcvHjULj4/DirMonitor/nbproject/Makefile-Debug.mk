#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Debug/GNU-Linux-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/dir_monitor_test.o \
	${OBJECTDIR}/dir_monitor.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lboost_filesystem-gcc42-mt-1_36

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Linux-x86/dirmonitor

dist/Debug/GNU-Linux-x86/dirmonitor: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o dist/Debug/GNU-Linux-x86/dirmonitor ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/dir_monitor_test.o: dir_monitor_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/usr/local/include/boost-1_36 -o ${OBJECTDIR}/dir_monitor_test.o dir_monitor_test.cpp

${OBJECTDIR}/dir_monitor.o: dir_monitor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -g -I/usr/local/include/boost-1_36 -o ${OBJECTDIR}/dir_monitor.o dir_monitor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/dirmonitor

# Subprojects
.clean-subprojects:
