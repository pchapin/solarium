#
# Makefile for the Solar System Simulator project (BarnesHut version with optional OpenMP))
#

CC=gcc
CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -O2 -fopenmp -I../Common
noopt: CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -I../Common
gcov:  CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -fprofile-arcs -ftest-coverage -I../Common
gprof: CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -pg -I../Common
LD=gcc
LDFLAGS=-fopenmp
LDLIBS=
gcov:  LDLIBS=-lgcov
gprof: LDLIBS=-pg
SOURCES=main.c Object.c Octree.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=BarnesHut

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -L../Common -lCommon -lm -o $@

noopt:	$(EXECUTABLE)

gcov:	$(EXECUTABLE)

gprof:	$(EXECUTABLE)

# File Dependencies
###################

main.o:		main.c ../Common/global.h ../Common/Initialize.h

Object.o:	Object.c ../Common/global.h Octree.h

Octree.o:	Octree.c Octree.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
