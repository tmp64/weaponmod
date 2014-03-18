#(C)2004-2005 AMX Mod X Development Team
# Makefile written by David "BAILOPAN" Anderson


OPT_FLAGS = -O3 -funroll-loops -s -pipe -fomit-frame-pointer -fno-strict-aliasing -DNDEBUG

DEBUG_FLAGS = -g -ggdb3
CPP = gcc
#CPP = gcc-2.95
NAME = weaponmod

BIN_SUFFIX = amxx_i386.so

OBJECTS = sdk/amxxmodule.cpp wpnmod_config.cpp wpnmod_grenade.cpp wpnmod_hooker.cpp wpnmod_hooks.cpp wpnmod_main.cpp wpnmod_memory.cpp wpnmod_natives.cpp wpnmod_parse.cpp wpnmod_pvdata.cpp wpnmod_utils.cpp wpnmod_version.cpp wpnmod_vtable.cpp


LINK = 

INCLUDE = -I. -Idep/hlsdk/common -Idep/hlsdk/dlls -Idep/hlsdk/engine -Idep/hlsdk/pm_shared -Idep/metamod -Isdk -Iinclude

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS = $(DEBUG_FLAGS)
else
	BIN_DIR = Release

	ifeq "$(GCC_VERSION)" "4"
		OPT_FLAGS += -fvisibility=hidden -fvisibility-inlines-hidden
	endif
	CFLAGS = $(OPT_FLAGS)
endif

CFLAGS += -Wall -Wno-non-virtual-dtor -fno-exceptions -DHAVE_STDINT_H -fno-rtti -m32  -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -fno-threadsafe-statics

BINARY = $(NAME)_$(BIN_SUFFIX)
CFLAGS += -DPAWN_CELL_SIZE=32 -DJIT -DASM32
OPT_FLAGS += -march=i586

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/sdk
	rm -rf Release/*.o
	rm -rf Release/sdk/*.o
	rm -rf Release/$(NAME)_$(BIN_SUFFIX)
	rm -rf Debug/*.o
	rm -rf Debug/sdk/*.o
	rm -rf Debug/$(NAME)_$(BIN_SUFFIX)
	./wpnmod_appversion.sh
	$(MAKE) weaponmod

weaponmod: $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -shared -ldl -lm -o$(BIN_DIR)/$(BINARY)
	
debug:	
	$(MAKE) all DEBUG=true

default: all
