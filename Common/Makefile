#
# Makefile for the Solarium Common library.
#

CC=gcc
CCFLAGS=-c -pthread -std=c99 -D_XOPEN_SOURCE=600 -O3
LINK=ar
SOURCES=Initialize.c  \
	Interval.c    \
	ProblemFile.c \
	str.c         \
	ThreadPool.c  \
	Timer.c       \
	Vector3.c
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

Interval.o:	Interval.c Interval.h

ProblemFile.o:	ProblemFile.c ProblemFile.h

str.o:		str.c str.h

ThreadPool.o:	ThreadPool.c

Timer.o:	Timer.c environ.h

Vector3.o:	Vector3.c

# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) *.s *.ll *~
