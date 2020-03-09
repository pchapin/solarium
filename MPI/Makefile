#
# Makefile for the Solar System Simulator project (MPI version)
#

CC=mpicc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -fopenmp -O2 -I../Common
LD=mpicc
LDFLAGS=-fopenmp
SOURCES=main.c Object.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=MPI

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
