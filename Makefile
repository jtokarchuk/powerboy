#OBJS specifies which files to compile as part of the project
OBJS = $(wildcard src/*.c) \
	   $(wildcard src/cpu/*.c)


#CC specifies which compiler we're using
CC = gcc

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows
DEBUG_COMPILER_FLAGS = -w


#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = poweremu
DEBUG_OBJ_NAME = poweremu-debug

RELEASE_PATH = .\build\release
DEBUG_PATH = .\build\debug

ifdef OS
	RM = del /Q
	DEL_FILES = *.exe
else
	ifeq ($(shell uname), Linux)
		RM = rm -f
		DEL_FILES = *.out
	endif
endif

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(RELEASE_PATH)\$(OBJ_NAME)

debug : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(DEBUG_COMPILER_FLAGS) $(LINKER_FLAGS) -o $(DEBUG_PATH)\$(DEBUG_OBJ_NAME)

.PHONY : clean
clean :
	$(RM) $(DEBUG_PATH)\$(DEL_FILES)
	$(RM) $(RELEASE_PATH)\$(DEL_FILES)
