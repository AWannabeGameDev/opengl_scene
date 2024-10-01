# Just change these variables and everything just magically works :D

# TODO: Handle subproject dependencies properly

projectFolder = ..
subProjDeps = gl_util

srcDirs = src
incDirs = ..\deps\include ..\gl_util\include
libDirs = ..\deps\lib ..\gl_util\lib "C:\Program^Files^(x86)\Windows^Kits\10\Lib\10.0.26100.0\um\x64"
intermDir = interms

libFilesDebug = gl_utild.lib GLFW\libglfw3.a Gdi32.lib
libFilesRelease = gl_util.lib GLFW\libglfw3.a Gdi32.lib

outDebug = bin\maind.exe
outRelease = bin\main.exe

preDefinesDebug = _DEBUG
preDefinesRelease =  

#-----------------------------------------------------------------------

CC = ccache g++

empty = 

srcs = $(foreach dir, $(srcDirs), $(wildcard $(dir)/*.cpp))

depFlags = -MP -MD
incFlags = $(foreach dir, $(incDirs), -I$(dir))
linkFlags = $(foreach dir, $(libDirs), $(subst ^, $(empty), -L$(dir)))

#------------------------------------------------------------------------

preDefDebugFlags = $(foreach def, $(preDefinesDebug), -D$(def))
preDefReleaseFlags = $(foreach def, $(preDefinesRelease), -D$(def))

libFilesDebugFlag = $(foreach file, $(libFilesDebug), -l:$(file))
libFilesReleaseFlag = $(foreach file, $(libFilesRelease), -l:$(file))

debugOptim = -O0
releaseOptim = -O2

debugCompFlags = -g $(incFlags) $(preDefDebugFlags) $(debugOptim) $(depFlags) -std=c++20
releaseCompFlags = $(incFlags) $(preDefReleaseFlags) $(releaseOptim) $(depFlags) -std=c++20

debugIntermDir = $(intermDir)\debug
releaseIntermDir = $(intermDir)\release

debugObjs = $(patsubst %.cpp, $(debugIntermDir)/%.o, $(srcs))
releaseObjs = $(patsubst %.cpp, $(releaseIntermDir)/%.o, $(srcs))

debugDeps = $(patsubst %.cpp, $(debugIntermDir)/%.d, $(srcs))
releaseDeps = $(patsubst %.cpp, $(releaseIntermDir)/%.d, $(srcs))

#--------------------------------------------------------------------

debug: makeTarget = debug
debug: libFilesFlag = $(libFilesDebugFlag)
debug: compFlags = $(debugCompFlags)

release: makeTarget = release
release: libFilesFlag = $(libFilesReleaseFlag)
release: compFlags = $(releaseCompFlags)

#---------------------------------------------------------------------

.PHONY: debug release test gendirs clean $(subProjDeps)

debug: gendirs $(outDebug)
release: gendirs $(outRelease)

gendirs: 
	@for %%x in ($(srcDirs)) do if not exist $(debugIntermDir)\%%x mkdir $(debugIntermDir)\%%x
	@for %%x in ($(srcDirs)) do if not exist $(releaseIntermDir)\%%x mkdir $(releaseIntermDir)\%%x

define buildExecutable
	@echo Building $@
	@$(CC) $(linkFlags) -o $@ $(filter-out $(subProjDeps), $^) $(libFilesFlag)
endef 

$(outDebug): $(subProjDeps) $(debugObjs) ; $(buildExecutable)
$(outRelease): $(subProjDeps) $(releaseObjs) ; $(buildExecutable)

$(subProjDeps):
	@make $(makeTarget) -s -C $(projectFolder)\$@ 

$(debugIntermDir)/%.o $(releaseIntermDir)/%.o: %.cpp
	@echo Compiling $<
	@$(CC) $(compFlags) -c -o $@ $<

clean: 
	@if exist $(intermDir) rmdir /s /q $(intermDir)
	@if exist $(outDebug) del /q $(outDebug)
	@if exist $(outRelease) del /q $(outRelease)

-include $(debugDeps)
-include $(releaseDeps)