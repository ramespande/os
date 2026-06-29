# OS Assignment 1

## Project Overview
This project implements a simplified ELF loader in C. The loader reads an ELF executable (e.g., fib.c compiled to fib), loads its segments into memory, and transfers control to the program’s entry point. The assignment also required splitting the project into components, creating a shared library (lib_simpleloader.so), a launcher (launch), and test programs. A Makefile system was set up to automate compilation and organisation.

## Implementation
The loader was written to handle ELF header parsing, program segment mapping using mmap, and execution transfer to the loaded program. The launcher was implemented to take user input and link against the loader’s shared library for execution. Separate Makefiles in each subdirectory automate the compilation process, while the top-level Makefile coordinates everything. The fib.c program was compiled and used to test the functionality of the loader.

## Contribution
### Nakul: 
Worked on ELF header parsing, segment mapping, loader initialisation, and debugging.
### Rijul: 
Implemented loader logic for PT_LOAD segments, entry point handling, and Makefiles.

### Github Link: 
[https://github.com/rijulagarwal/SimpleLoader_OS_Assignment1
](url)

### Made by:
Nakul Zutshi 2024361

Rijul Agarwal 2024459
