shadow-memory
=============

A robust and scalable shadow memory library based on Valgrind's memcheck / the paper
*How to shadow every byte of memory used by a program.* by Nicholas Nethercote and
Julian Seward (2007).

The goal is to have a user-friendly API for working with shadow memory representing
a user-defined concept. Concepts of particular interest include but are not limited to:

* Software-based memory error detection:
  * Bad memory accesses (e.g. Segmentation Fault inducing semantic bugs)
  * State-based memory permission assertions (sanity checking program models)
  * Data integrity (trusted vs untrusted data sources)
* Profiling
  * Memory access patterns
  * Data-flow analysis

For now, we present a shadow map library capable of tracking one bit of information
per bit of memory being shadowed. In our examples, this bit of information corresponds
to whether or not the memory in question is meta-data (object meta-data used by
a memory allocator, garbage collector, or runtime system in general).

Basic Usage
-----------

The simplest use case is to use libshadow as a statically linked library in a C
project using the standard C libraries. To do this, run:

    mkdir shadow-memory && cd shadow-memory
    git clone https://github.com/cronburg/shadow-memory.git .
    make shadow.so

Then `#include "shadow.c"` in any C source file, and link libshadow by passing
the flags `-L/path/to/shadow-memory -lshadow` to GCC.

Advanced Usage
--------------

For projects relying on non-standard versions of system and library calls
(e.g. Valgrind), one can define how libshadow calls the following functions:

    free(), malloc(), calloc(), memcpy(), out_of_memory()

The recommended way to override these functions is to
clone this repository into a subdirectory of your project, and edit
files "shadow.c" and "shadow.h" to match your needs.

Valgrind Use Case
-----------------

If you are writing a Valgrind tool, the above "Advanced Usage" involves
removing "shadow.h" all-together, and replacing each system call in
"shadow.c" to use Valgrind's shimmed version. For example `free(addr)`
becomes `VG_(free)(addr)`, and in general `syscall(...)` becomes
`VG_(syscall)(...)`.

A Valgrind tool relying on this library will be made available in the
near future. This use case is currently the primary motivation behind
development of this library, however we hope to keep the interface as
general as possible.

