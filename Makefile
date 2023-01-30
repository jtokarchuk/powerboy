#OBJS specifies which files to compile as part of the project
OBJS = main.cpp


#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I.\lib\SDL2\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L.\lib\SDL2\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows
DEBUG_COMPILER_FLAGS = -w


#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = poweremu
DEBUG_OBJ_NAME = poweremu-debug

#This is the target that compiles our executable
all : .\src\$(OBJS)
	$(CC) .\src\$(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o .\build\release\$(OBJ_NAME)

debug : .\src\$(OBJS)
	$(CC) .\src\$(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(DEBUG_COMPILER_FLAGS) $(LINKER_FLAGS) -o .\build\debug\$(DEBUG_OBJ_NAME)