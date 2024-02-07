#
# Makefile for the Solar System Simulator project (Parallel version using barriers)
#

CC=gcc
CFLAGS=-c -pthread -std=c99 -D_XOPEN_SOURCE=600 -O2 -I../Common
LD=gcc
LDFLAGS=-pthread
SOURCES=main.c Object.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Parallel-barriers

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../Common -lCommon -lm -o $@

# File Dependencies
###################

main.o:		main.c ../Common/global.h ../Common/Initialize.h

Object.o:	Object.c ../Common/Initialize.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
