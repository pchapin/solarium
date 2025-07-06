
# Solarium

This folder contains a simple solar system simulator that computes a solution for the n-body
problem where the inter-object interaction is the gravitational force. It is our intention for
this program to one day be a useful application capable of realistic simulations. It is also our
intention for this program to illustrate a variety of parallel and HPC programming technologies.
Thus, a major goal of this work is educational.

The official operating system for this project is Linux, but much of the code should also work
on macOS and on Windows using WSL (and maybe Cygwin, but that has not been extensively tested).
Some of the subprojects use highly specialized technologies that may not be available on all
platforms. For example, the CUDA version requires an NVIDIA GPU and the OpenCL version requires
an OpenCL-compatible GPU. See the `README.md` files in the various folders for more information
about the requirements for each subproject.

Most of the code is in C, but some other languages are also represented.

The folders contain the following

+ Ada: An Ada version of the solar system simulator.

+ Barnes-Hut: This uses the Barnes-Hut algorithm for solving the n-body problem in O(n log(n))
  time. It parallelizes the computation of the next dynamics using OpenMP, but it builds the
  octree data structure serially.

+ Common: This folder contains a number of useful library components along with a test program
  for those components. The material here is used by several of the other programs in this
  project.

+ CUDA: A CUDA version of the solar system simulator.

+ doc: The official documentation for this project.

+ Julia: A Julia version of the solar system simulator.

+ MPI: An MPI version of the solar system simulator.

+ OpenACC: An OpenACC version of the solar system simulator.

+ OpenCL: An OpenCL version of the solar system simulator.

+ OpenMP: An OpenMP version of the solar system simulator.

+ Parallel-barriers: This version uses POSIX threads for parallelism. It uses barriers to
  "steer" a single team of threads without having to create and destroy threads over and over as
  is done in the baseline parallel version.

+ Parallel-pools: This version uses POSIX threads for parallelism. It uses a thread pool to
  reduce thread creation overhead.

+ Parallel-pthread: This version uses POSIX threads for parallelism. It is the baseline parallel
  version. It suffers from repeatedly creating and joining with threads and thus incurs
  significant thread management overhead.

+ Rust: A Rust version of the solar system simulator.

+ Scala: A Scala version of the solar system simulator.

+ Serial: This version is entirely serial. It can be used as a baseline against which timings of
  all the parallel versions can be compared.

## Editing

Visual Studio Code is the recommended editor for this project. The `.vscode` folder contains
settings that should make it easy to get started. The actual building is typically done in
the terminal by running `make` in the appropriate folder manually.

## Building

Most folders contain a `Makefile` that can be used to build the programs in that folder. The
`Makefile` in the `Common` folder can be used to build the common library components and their
test program. All the make files should be compatible across Linux, macOS, and Windows (using
WSL or Cygwin (not extensively tested)).

Each folder contains a `README.md` file that describes the programs in that folder and how to
use them.

Peter Chapin  
spicacality@kelseymountain.org  
