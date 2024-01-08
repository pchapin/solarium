
# Solarium

This folder contains a simple solar system simulator that computes a solution for the n-body
problem where the inter-object interaction is the gravitational force. It is my intention for
this program to one day be a useful application capable of realistic simulations. It is also my
intention for this program to illustrate a variety of parallel and HPC programming technologies.
A major goal of this work is thus educational. In particular, I use this example in my Parallel
Programming class at Vermont State University.

Most of the code is in C (but some other languages are also represented).

The folders contain the following

+ Ada: The main Solarium application in Ada.

+ Barnes-Hut: This version is serial, but it uses the Barnes-Hut algorithm for solving the
  n-body problem in O(n log(n)) time. It can be used as the basis for a more efficient parallel
  solution.

+ Common: This folder contains a number of useful library components along with a test program
  for those components. The material here is used by several of the other programs in this
  project.

+ CUDA: Some trivial CUDA code to demonstrate a few basic ideas of CUDA programming.

+ doc: The official documentation for this project.

+ Julia: The main Solarium application in Julia.

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

+ Scala: The main Solarium application in Scala.

+ Serial: This version is entirely serial. It can be used as a baseline against which timings of
  all the parallel versions can be compared.

## Editing

Visual Studio Code is the recommended editor for this project. The `.vscode` folder contains
settings that should make it easy to get started. The actual building is typically done in
the terminal by running `make` in the appropriate folder manually.

Two IDEs are also supported: Code::Blocks and Visual Studio. See below for details.

## Building

Most folders contain a `Makefile` that can be used to build the programs in that folder. The
`Makefile` in the `Common` folder can be used to build the common library components and their
test program. All the make files should be compatible across Linux, macOS, and Windows (using
Cygwin). They are not necessarily compatible with Microsoft's `nmake` tool.

Each folder contains a `README.md` file that describes the programs in that folder and how to
use them.

## Building with Code::Blocks

The Code::Blocks IDE can be used to build the programs in this project, but be aware that
Code::Blocks is mostly only tested on the Linux platform; it may or may not work properly on
other platforms.

There is a workspace file in the root folder that can be used to open the entire project in
Code::Blocks. Each folder contains a Code::Blocks project file that can be used to open just
that folder in Code::Blocks.

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
