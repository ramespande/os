# Operating Systems Components Suite

A collection of core operating system components implemented from scratch in **C/C++** for Unix/Linux. This project demonstrates low-level systems programming concepts including process management, scheduling, executable loading, shell implementation, and multithreading.

## Features

- **Custom ELF Loader**
  - Loads and executes 32-bit ELF executables.
  - Implements demand paging for efficient memory utilization.
  - Parses ELF headers and program segments manually.

- **Unix Shell**
  - Command parsing and execution.
  - Support for foreground and background processes.
  - Built-in shell commands.
  - Process management using Unix system calls.

- **Process Scheduler**
  - Priority-based scheduling algorithm.
  - Daemon process support.
  - Process creation and lifecycle management.
  - Scheduling policy implementation.

- **Multithreading Library**
  - User-level thread management.
  - Context switching.
  - Thread synchronization primitives.
  - Concurrent execution support.

---

## Tech Stack

- **Language:** C, C++
- **Build System:** Makefile
- **Platform:** Unix/Linux
- **Concepts Used:**
  - ELF Format
  - Demand Paging
  - Virtual Memory
  - Process Scheduling
  - Context Switching
  - POSIX System Calls
  - Threads and Synchronization

---

## Learning Objectives

This project explores fundamental operating system concepts, including:

- Process creation and execution
- Virtual memory management
- ELF binary loading
- Demand paging
- CPU scheduling
- Shell implementation
- User-level multithreading
- Concurrency and synchronization
- Linux system programming

---

## Build

Compile the project using Make:

```bash
make
```

Clean build files:

```bash
make clean
```

---

## Running

Navigate to the required component and execute the corresponding binary.

Refer to the documentation within each component for usage details.

---

## Highlights

- Built multiple operating system components from scratch.
- Implemented demand paging for 32-bit ELF executables to reduce memory overhead.
- Developed a priority-based scheduler with daemon process handling.
- Designed a Unix shell supporting process creation and management.
- Implemented multithreading support for concurrent execution.
