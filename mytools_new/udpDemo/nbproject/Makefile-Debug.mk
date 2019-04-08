#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS= 

# Macros
CND_PLATFORM=GNU_4.8.2-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/714056d5/BinaryBuf.o \
	${OBJECTDIR}/_ext/714056d5/BinaryMemPool.o \
	${OBJECTDIR}/_ext/4e21f46f/Epoll.o \
	${OBJECTDIR}/_ext/4e21f46f/UdpEpoll.o \
	${OBJECTDIR}/_ext/516c26c5/FileOpt.o \
	${OBJECTDIR}/_ext/516c26c5/IniFile.o \
	${OBJECTDIR}/_ext/516c26c5/Log.o \
	${OBJECTDIR}/_ext/65a12900/Lock.o \
	${OBJECTDIR}/_ext/35e1160c/DataDepot.o \
	${OBJECTDIR}/_ext/35be9bb4/UdpSrv.o \
	${OBJECTDIR}/_ext/6616ec4c/UdpSrvBase.o \
	${OBJECTDIR}/_ext/c9ca406/UdpSrvTask.o \
	${OBJECTDIR}/_ext/c9ca406/UdpSrvTaskManager.o \
	${OBJECTDIR}/_ext/389e0bbc/Task.o \
	${OBJECTDIR}/_ext/389e0bbc/TaskManager.o \
	${OBJECTDIR}/_ext/389e0bbc/TaskQueue.o \
	${OBJECTDIR}/_ext/389e0bbc/ThreadPool.o \
	${OBJECTDIR}/TestSrv.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/udpdemo

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/udpdemo: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/udpdemo ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/714056d5/BinaryBuf.o: ../common/buffer/BinaryBuf.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/714056d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/714056d5/BinaryBuf.o ../common/buffer/BinaryBuf.cpp

${OBJECTDIR}/_ext/714056d5/BinaryMemPool.o: ../common/buffer/BinaryMemPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/714056d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/714056d5/BinaryMemPool.o ../common/buffer/BinaryMemPool.cpp

${OBJECTDIR}/_ext/4e21f46f/Epoll.o: ../common/epoll/Epoll.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/4e21f46f
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4e21f46f/Epoll.o ../common/epoll/Epoll.cpp

${OBJECTDIR}/_ext/4e21f46f/UdpEpoll.o: ../common/epoll/UdpEpoll.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/4e21f46f
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4e21f46f/UdpEpoll.o ../common/epoll/UdpEpoll.cpp

${OBJECTDIR}/_ext/516c26c5/FileOpt.o: ../common/file_opt/FileOpt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/516c26c5
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/516c26c5/FileOpt.o ../common/file_opt/FileOpt.cpp

${OBJECTDIR}/_ext/516c26c5/IniFile.o: ../common/file_opt/IniFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/516c26c5
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/516c26c5/IniFile.o ../common/file_opt/IniFile.cpp

${OBJECTDIR}/_ext/516c26c5/Log.o: ../common/file_opt/Log.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/516c26c5
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/516c26c5/Log.o ../common/file_opt/Log.cpp

${OBJECTDIR}/_ext/65a12900/Lock.o: ../common/lock/Lock.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/65a12900
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/65a12900/Lock.o ../common/lock/Lock.cpp

${OBJECTDIR}/_ext/35e1160c/DataDepot.o: ../common/srv/data_depot/DataDepot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/35e1160c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/35e1160c/DataDepot.o ../common/srv/data_depot/DataDepot.cpp

${OBJECTDIR}/_ext/35be9bb4/UdpSrv.o: ../common/srv/udp/UdpSrv.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/35be9bb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/35be9bb4/UdpSrv.o ../common/srv/udp/UdpSrv.cpp

${OBJECTDIR}/_ext/6616ec4c/UdpSrvBase.o: ../common/srv/udp/base/UdpSrvBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6616ec4c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6616ec4c/UdpSrvBase.o ../common/srv/udp/base/UdpSrvBase.cpp

${OBJECTDIR}/_ext/c9ca406/UdpSrvTask.o: ../common/srv/udp/udp_srv_task/UdpSrvTask.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/c9ca406
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c9ca406/UdpSrvTask.o ../common/srv/udp/udp_srv_task/UdpSrvTask.cpp

${OBJECTDIR}/_ext/c9ca406/UdpSrvTaskManager.o: ../common/srv/udp/udp_srv_task/UdpSrvTaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/c9ca406
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c9ca406/UdpSrvTaskManager.o ../common/srv/udp/udp_srv_task/UdpSrvTaskManager.cpp

${OBJECTDIR}/_ext/389e0bbc/Task.o: ../common/thread_pool/Task.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/389e0bbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/389e0bbc/Task.o ../common/thread_pool/Task.cpp

${OBJECTDIR}/_ext/389e0bbc/TaskManager.o: ../common/thread_pool/TaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/389e0bbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/389e0bbc/TaskManager.o ../common/thread_pool/TaskManager.cpp

${OBJECTDIR}/_ext/389e0bbc/TaskQueue.o: ../common/thread_pool/TaskQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/389e0bbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/389e0bbc/TaskQueue.o ../common/thread_pool/TaskQueue.cpp

${OBJECTDIR}/_ext/389e0bbc/ThreadPool.o: ../common/thread_pool/ThreadPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/389e0bbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/389e0bbc/ThreadPool.o ../common/thread_pool/ThreadPool.cpp

${OBJECTDIR}/TestSrv.o: TestSrv.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TestSrv.o TestSrv.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/udpdemo

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
