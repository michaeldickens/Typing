# 
# Makefile for Typing.
# 
# This version was created on 3/16/13.
# 

# Set default C compiler to gcc.
CC = gcc

# Compiler flags.
CFLAGS = -std=c99 -Wall

# Flags used for debug mode.
DEBUG_FLAGS = -g -O0 -fno-inline

# Flags used for profile mode.
PROFILE_FLAGS = -g -pg -O2 -fno-inline

# Flags used for release mode.
RELEASE_FLAGS = -O2 -DNDEBUG

# Flags used with the default compilation mode.
DEFAULT_FLAGS = $(RELEASE_FLAGS)

# Libraries to link against.
LDLIBS = -lm -lpthread

PROGRAMS = optimizer unit

# Make the program under the default compilation mode.
all: $(PROGRAMS)
all: CFLAGS += $(DEFAULT_FLAGS)

# Make the program for debugging.
debug: $(PROGRAMS)
debug: CFLAGS += $(DEBUG_FLAGS)

# Make the program for profiling.
profile: $(PROGRAMS)
profile: CFLAGS += $(PROFILE_FLAGS)

# Make the program for releasing.
release: $(PROGRAMS)
release: CFLAGS += $(RELEASE_FLAGS)

# Dependencies
optimizer: accessories.o cjalgorithm.o fitness.o keyboard.o tools.o values.o
unit: accessories.o cjalgorithm.o fitness.o keyboard.o tools.o values.o

clean: 
	rm -f $(programs) *.o callgrind.out.*
