
Solarium
========

This folder contains a simple solar system simulator. It computes a solution for the n-body
problem where the inter-object interaction is the gravitational force. It is my intention for
this program to one day be a useful application capable of realistic simulations. It is also my
intention for this program to illustrate a variety of parallel and HPC programming technologies.
A major goal of this work is thus educational. In particular, I use this example in my Parallel
Programming class.

Most of the code is in C (perhaps with some Ada and some C++).

The subfolders contain the following

+ Ada: The main Solarium application in Ada.

+ Barnes-Hut: This version is serial but it uses the Barnes-Hut algorithm for solving the n-body
  probably in O(n log(n)) time. It can be used as the basis for a more efficient parallel
  solution.

+ Common: This folder contains a number of useful library components along with a test program
  for those components. The material here is used by several of the other programs in this
  project.

+ CUDA: Some trivial CUDA code just to demonstrate a few basic idea of CUDA programming.

+ doc: The official documentation for this project.

+ MPI: An MPI version of the solar system simulator.

+ OpenMP: An OpenMP version of the solar system simulator.

+ Parallel-pthread: This version uses POSIX threads for parallelism. There are several variants
  of this program: Parallel-pools uses a thread pool to reduce thread creation overhead.
  Parallel-barriers uses barries (instead of a thread pool) to "steer" a single team of threads
  without having to create and destroy threads over and over as is done in the baseline parallel
  version.

+ Serial: This version is entirely serial. It can be used as a baseline against which timings of
  the parallel versions can be compared.

Peter C. Chapin  
chapinp@acm.org  

