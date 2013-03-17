# 
# Makefile for Typing.
# 
# This version was created on 3/16/13.
# 

# Set default C compiler to gcc.
CC = gcc

# Compiler flags.
CFLAGS = -std=c99

# Flags used for debug mode.
DEBUG_FLAGS = -g -O0 -Wall

# Flags used for profile mode.
PROFILE_FLAGS = -g -pg -O3

# Flags used for release mode.
RELEASE_FLAGS = -O3

# Default compilation mode.
DEFAULT_FLAGS = -g -O3 -Wall

# Libraries to link against.
LDLIBS = -lm -lpthread

programs = optimizer

optimizer: accessories.o cjalgorithm.o fitness.o keyboard.o tools.o values.o


# Make the program under the default compilation mode.
all: $(programs)
all: CFLAGS += $(DEFAULT_FLAGS)

# Make the program for debugging.
debug: $(programs)
debug: CFLAGS += $(DEBUG_FLAGS)

# Make the program for profiling.
profile: $(programs)
profile: CFLAGS += $(PROFILE_FLAGS)

# Make the program for releasing.
release: $(programs)
release: CFLAGS += $(RELEASE_FLAGS)

clean: 
	rm -f $(programs) *.o