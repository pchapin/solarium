#
# Makefile for the Solar System Simulator project (Parallel-pools version)
#

CC=gcc
CFLAGS=-c -std=c99 -pthread -D_XOPEN_SOURCE=600 -O2 -I../Common -I../../Spica/C
LD=gcc
LDFLAGS=-pthread
SOURCES=main.c Object.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Parallel-pools

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../Common -L../../Spica/C -lCommon -lSpicaC -lm -o $@

# File Dependencies
###################

main.o:		main.c Pool.h ../Common/global.h ../Common/Initialize.h

Object.o:	Object.c Pool.h ../Common/global.h ../Common/Initialize.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
