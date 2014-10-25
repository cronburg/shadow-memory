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
