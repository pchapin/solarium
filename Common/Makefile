#
# Makefile for the Solarium Common library.
#

CC=gcc
CCFLAGS=-c -pthread -std=c99 -D_XOPEN_SOURCE=600 -O2 -I../../Spica/C
LINK=ar
SOURCES=Initialize.c  \
	ProblemFile.c
OBJECTS=$(SOURCES:.c=.o)
LIBRARY=libCommon.a

%.o:	%.c
	$(CC) $(CCFLAGS) $< -o $@

$(LIBRARY):	$(OBJECTS)
	$(LINK) r $@ $(OBJECTS)

# File Dependencies
###################

# Module dependencies

Initialize.o:	Initialize.c global.h Initialize.h

ProblemFile.o:	ProblemFile.c ProblemFile.h

# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) *.s *.ll *~
