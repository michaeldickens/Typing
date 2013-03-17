# 
# Makefile for Typing.
# 
# This version was created on 3/16/13.
# 

# Set default C compiler to gcc.
CC = gcc

# Compiler flags.
CFLAGS = 

# Flags used for debug mode.
DEBUG_FLAGS = -g -O0

# Flags used for profile mode.
PROFILE_FLAGS = -g -pg -O3

# Flags used for release mode.
RELEASE_FLAGS = -O3

# Libraries to link against.
LDLIBS = -lm -lpthread

programs = main

main: accessories.o cjalgorithm.o fitness.o keyboard.o tools.o values.o
accessories.o:
cjalgorithm.o:
fitness.o:
keyboard.o:
tools.o:
values.o:

all: $(programs)
all: CFLAGS += $(RELEASE_FLAGS)

debug: $(programs)
debug: CFLAGS += $(DEBUG_FLAGS)

profile: $(programs)
profile: CFLAGS += $(PROFILE_FLAGS)

build: accessories cjalgorithm fitness keyboard main tools values
    gcc -o optimizer -O0 accessories.c cjalgorithm.c fitness.c keyboard.c main.c tools.c values.c -lm -lpthread

clean: 
	rm -f $(programs) *.o