
Solarium
========

This folder contains a simple solar system simulator. Essential it computes a solution for the
n-body problem. It is my intention for this program to one day be a useful application capable
of realistic simulations.

Most of the code is in C (perhaps with some C++). However, it is my intention to develop the
primary Solarium application in Ada. The C code in this repository was originally developed to
support my parallel programming class and thus demonstrates a variety of techniques and
technologies. The main application, in contrast, will choose a single approach (or perhaps be a
single executable that can be configured to support several related approaches).

The subfolders contain the following

+ Barnes-Hut: This version is serial but it uses the Barnes-Hut algorithm for solving the n-body
  probably in O(n log(n)) time. It can be used as the basis for a more efficient parallel
  solution.

+ Common: This folder contains a number of useful library components along with a test program
  for those components. The material here is used by several of the other programs in this
  project.

+ CUDA: Some trivial CUDA code just to demonstrate a few basic idea of CUDA programming.

+ doc: The official documentation set for this project.

+ MPI: An MPI version of the solar system simulator.

+ OpenMP: An OpenMP version of the solar system simulator.

+ Parallel: This version uses POSIX threads for parallelism.

+ Serial: This version is entirely serial. It can be used as a baseline against which timings of
  the parallel versions can be compared.

+ src: The main Solarium application in Ada.

Peter C. Chapin  
PChapin@vtc.vsc.edu
