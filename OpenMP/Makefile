#
# Makefile for the Solar System Simulator project (OpenMP version)
#

CC=gcc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -fopenmp -O3 -I../Common
LD=gcc
LDFLAGS=-fopenmp
SOURCES=main.c Object.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=OpenMP

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../Common -lCommon -lm -o $@

# File Dependencies
###################

main.o:         main.c ../Common/global.h ../Common/Initialize.h

Object.o:	Object.c ../Common/global.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
