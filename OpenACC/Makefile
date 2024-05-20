#
# Makefile for the Solar System Simulator project (OpenACC version)
#

# The cc70 option selects compute capabiity 7.0. Consider trying -fast to enable aggressive optimization.
CC=nvc
CFLAGS=-c -Wall -acc -gpu=cc50 -D_XOPEN_SOURCE=600 -O3

LD=nvc
LDFLAGS=-acc -gpu=cc50

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
