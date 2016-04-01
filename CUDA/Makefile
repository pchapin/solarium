#
# Makefile for the Solar System Simulator project (CUDA version)
#

NVCC=nvcc
NVCFLAGS=-c -m64 -arch=sm_13 -O -I../Common -I../../Spica/C
LD=nvcc
LDFLAGS=-m64 -arch=sm_13
SOURCES=main.cu Object.cu Initialize.cu
OBJECTS=main.o Object.o Initialize.o
EXECUTABLE=CUDA

%.o:	%.cpp
	$(NVCC) $(NVCFLAGS) $< -o $@

%.o:	%.cu
	$(NVCC) $(NVCFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../../Spica/C -lSpicaC -lm -o $@

# File Dependencies
###################

main.o:         main.cu global.h Initialize.h

Object.o:	Object.cu CUDAVector3.h global.h Initialize.h

Initialize.o:	Initialize.cu global.h Initialize.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
