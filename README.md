
# Solarium

This folder contains a simple solar system simulator that computes a solution for the n-body
problem where the inter-object interaction is the gravitational force. It is our intention for
this program to one day be a useful application capable of realistic simulations. It is also our
intention for this program to illustrate a variety of parallel and HPC programming technologies.
A major goal of this work is thus educational.

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

The use of Visual Studio for some subprojects is also supported. See below for details. There is
also some support for using Eclipse/CDT in some projects, but it is not well maintained.

## Building

Most folders contain a `Makefile` that can be used to build the programs in that folder. The
`Makefile` in the `Common` folder can be used to build the common library components and their
test program. All the make files should be compatible across Linux, macOS, and Windows (using
WSL or Cygwin (not extensively tested)).

Each folder contains a `README.md` file that describes the programs in that folder and how to
use them.

## Building with Visual Studio

Before Visual Studio can be used, it is necessary to install pthread4w. This is a Windows
implementation of the POSIX threads API. It can be downloaded from
[SourceForge](https://sourceforge.net/projects/pthreads4w/). Unzip the archive in any convenient
location and use `nmake` to build and install the library. The default installation location is
in a folder `PTHREADS-BUILT` as a sibling folder to where the source code is located.

Add an environment variable named `PTHREADS_HOME` that points to the `PTHREADS-BUILT` folder.
The Visual Studio project files use this environment variable to locate the pthread4w library.

You can now open the Visual Studio solution file in the root folder and build the entire
project. Each folder contains a Visual Studio project file that can be used to open just that
folder in Visual Studio.

Peter Chapin  
spicacality@kelseymountain.org  
