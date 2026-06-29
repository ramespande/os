# multithread

## Project Overview
This project implements SimpleMultithreader, a header-file-only abstraction that helps programmers use Pthreads more easily. The library provides two functions named parallel_for, one for a single for-loop and one for two-dimensional for-loops. These functions accept C++11 lambda expressions and run the loop body in parallel using the number of threads specified by the programmer.

## Implementation
Each call to parallel_for creates the exact number of Pthreads requested, including work done by the main thread. Loop iterations are divided among threads, and all threads terminate once the loop finishes. The implementation follows the assignment requirement of not using any C++ threading libraries. The program also prints the total execution time for every call to parallel_for. The two provided sample programs (vector.cpp and matrix.cpp) run directly with this implementation without modification.

## Limitations
Pthreads are created for every call; no task or thread pools are used, as required.
Only the features mentioned in the assignment (C++11 lambdas and Pthreads) are used.

## Contributions 
### Rijul:
Implemented the one-dimensional parallel_for, handled thread creation logic, and performed initial debugging.
### Nakul:
Implemented the two-dimensional parallel_for, added error checks, and tested the examples.
### Github Link:
[https://github.com/rijulagarwal/multithread](url)

### Made by:
Rijul Agarwal 2024459

Nakul Zutshi 2024361
