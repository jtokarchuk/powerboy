BINARY       := powerboy
OBJ          := obj
BUILD        := 
SOURCES      := src
CFLAGS       := -Wall 
DEBUG_CFLAGS := -Wall
LIBS         := 
INCLUDES     := -Iinclude
CLEAN_CMD    :=

CFILES   := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
HFILES   := $(wildcard include/*.h)
OFILES   := $(patsubst %.c,$(OBJ)/%.o,$(CFILES))

# set the cleanup commands
ifeq ($(OS),Windows_NT)
	CLEAN_CMD += @del obj\* /Q && @del build\release\*.exe /Q  && @del build\debug\*.exe /Q 
else
	CLEAN_CMD += @rm -rf obj\* && @rm -rf build\release\*.out && @rm -rf build\debug\*.out
endif

# set the build directory
ifeq ($(MAKECMDGOALS), all)
	BUILD += $(CURDIR)/build/release
	ifeq ($(OS),Windows_NT)
		CFLAGS +=-Wl,-subsystem,windows
	endif
endif
ifeq ($(MAKECMDGOALS), debug)
	BUILD += $(CURDIR)/build/debug
endif

all: $(BUILD)/$(BINARY)

$(BUILD)/$(BINARY): $(OFILES)
	@cd $(OBJ) && \
	gcc -o $@ $(CFLAGS) $(LIBS) $(notdir $^) && \
	@cd ..

$(OBJ)/%.o: %.c $(HFILES) $(OBJ)
	gcc -c -o $(OBJ)/$(notdir $@) $(CFLAGS) $(INCLUDES) $<

debug: $(BUILD)/$(BINARY)

clean:
	$(CLEAN_CMD)