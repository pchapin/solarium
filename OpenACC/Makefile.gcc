#
# Makefile for the Solar System Simulator project (OpenACC version)
#

# Use the option -fopt-info-optimized for information about OpenACC optimizations that are done.
CC=gcc
CFLAGS=-c -Wall -fopenacc -foffload=nvptx-none -fcf-protection=none -D_XOPEN_SOURCE=600 -O3

# The 'sm_61' option specifies compute capability 6.1. Unfortunately, the gcc that comes with
# Ubuntu Linux 22.04 only supports compute capability up to 3.5. The CUDA toolkit version 12
# doesn't support any compute capability less than 5.0. Since there is no overlap between those
# ranges, the program can't be compiled on Ubuntu 22.04 without downgrading the CUDA toolkit or
# compiling a new version of GCC (with offloading support) from source.
LD=gcc
LDFLAGS=-fopenacc -foffload=nvptx-none -fcf-protection=none -foffload="-misa=sm_61"

SOURCES=main.c Initialize.c Object.c Timer.c Vector3.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=OpenACC

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -lm -o $@

# File Dependencies
###################

main.o:         main.c global.h

Initialize.o:	Initialize.c Initialize.h

Object.o:	Object.c global.h Initialize.h

Timer.o:	Timer.c Timer.h

Vector3.o:	Vector3.c Vector3.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
