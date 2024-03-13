#
# Makefile for the Solar System Simulator project (BarnesHut version)
#

CC=gcc
CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -O3 -fopenmp -I../Common
debug:	CFLAGS=-c -g -Wall -std=c99 -D_XOPEN_SOURCE=600 -O0 -I../Common
gcov:	CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -fprofile-arcs -ftest-coverage -I../Common
gprof:	CFLAGS=-c -Wall -std=c99 -D_XOPEN_SOURCE=600 -pg -I../Common
LD=gcc
LDFLAGS=-fopenmp
debug:	LDFLAGS=
gcov:	LDFLAGS=
gprof:	LDFLAGS=-pg
LDLIBS=
debug:	LDLIBS=
gcov:	LDLIBS=-lgcov
gprof:	LDLIBS=
SOURCES=main.c Object.c Octree.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=BarnesHut

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../Common -lCommon -lm $(LDLIBS) -o $@

debug:	$(EXECUTABLE)

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
	rm -f *.o *.bc *.s *.ll gmon.out *.gcno *~ $(EXECUTABLE)
