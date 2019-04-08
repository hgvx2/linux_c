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
	${OBJECTDIR}/_ext/4e21f46f/TcpEpoll.o \
	${OBJECTDIR}/_ext/516c26c5/FileOpt.o \
	${OBJECTDIR}/_ext/516c26c5/IniFile.o \
	${OBJECTDIR}/_ext/516c26c5/Log.o \
	${OBJECTDIR}/_ext/65a12900/Lock.o \
	${OBJECTDIR}/_ext/35e1160c/DataDepot.o \
	${OBJECTDIR}/_ext/35be97d4/TcpSrv.o \
	${OBJECTDIR}/_ext/c94f842c/TcpSrvBase.o \
	${OBJECTDIR}/_ext/349903c6/TcpSrvTask.o \
	${OBJECTDIR}/_ext/349903c6/TcpSrvTaskManager.o \
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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/testtools_new

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/testtools_new: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/testtools_new ${OBJECTFILES} ${LDLIBSOPTIONS}

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

${OBJECTDIR}/_ext/4e21f46f/TcpEpoll.o: ../common/epoll/TcpEpoll.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/4e21f46f
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4e21f46f/TcpEpoll.o ../common/epoll/TcpEpoll.cpp

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

${OBJECTDIR}/_ext/35be97d4/TcpSrv.o: ../common/srv/tcp/TcpSrv.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/35be97d4
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/35be97d4/TcpSrv.o ../common/srv/tcp/TcpSrv.cpp

${OBJECTDIR}/_ext/c94f842c/TcpSrvBase.o: ../common/srv/tcp/base/TcpSrvBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/c94f842c
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c94f842c/TcpSrvBase.o ../common/srv/tcp/base/TcpSrvBase.cpp

${OBJECTDIR}/_ext/349903c6/TcpSrvTask.o: ../common/srv/tcp/tcp_srv_task/TcpSrvTask.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/349903c6
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/349903c6/TcpSrvTask.o ../common/srv/tcp/tcp_srv_task/TcpSrvTask.cpp

${OBJECTDIR}/_ext/349903c6/TcpSrvTaskManager.o: ../common/srv/tcp/tcp_srv_task/TcpSrvTaskManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/349903c6
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/349903c6/TcpSrvTaskManager.o ../common/srv/tcp/tcp_srv_task/TcpSrvTaskManager.cpp

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
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/testtools_new

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
