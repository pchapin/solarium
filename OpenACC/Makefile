#
# Makefile for the Solar System Simulator project (OpenACC version)
#

CC=pgcc
CFLAGS=-c -acc -D_XOPEN_SOURCE=600 -O2
LD=pgcc
LDFLAGS=-ta=tesla:cc60
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
