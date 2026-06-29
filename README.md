# Operating Systems Components Suite

A collection of operating systems projects developed as part of the **Operating Systems Laboratory** course. This repository demonstrates the implementation of fundamental operating system components from scratch using **C**, **C++**, POSIX APIs, and Unix/Linux system programming concepts.

The projects cover executable loading, process scheduling, shell implementation, multithreading, and kernel scheduling, providing practical experience with low-level systems programming and operating system internals.

---

## Projects

### 1. ELF Loader

Implemented a custom loader capable of loading and executing 32-bit ELF executables.

**Features**
- Manual parsing of ELF and Program Headers
- Memory mapping using `mmap`
- Dynamic loading of executable segments
- Execution transfer to the program entry point
- Shared library (`lib_simpleloader.so`) and launcher implementation

**Concepts**
- ELF Binary Format
- Virtual Memory
- Memory Mapping
- Executable Loading

---

### 2. Unix Shell & CPU Scheduler

Developed a Unix-like shell integrated with a CPU scheduler through inter-process communication.

**Features**
- Command parsing and execution
- Scheduler implemented as a separate process
- Communication using Unix pipes
- Configurable CPU count and time slice
- Round-Robin scheduling
- Job queue management
- Process lifecycle management

**Concepts**
- Inter-Process Communication (IPC)
- Process Scheduling
- Fork and Exec
- Pipes
- Context Switching

---

### 3. SimpleMultithreader

Implemented a lightweight multithreading abstraction using POSIX Pthreads.

**Features**
- One-dimensional `parallel_for`
- Two-dimensional `parallel_for`
- Support for C++11 lambda expressions
- Static workload distribution
- Execution time measurement

**Concepts**
- POSIX Threads
- Parallel Programming
- Thread Synchronization
- Concurrent Execution

---

### 4. EGOS-2000 Kernel Extensions

Extended the EGOS-2000 educational operating system with additional shell utilities and an MLFQ scheduler.

**Features**
- Custom shell utilities (`grep`, `wcl`)
- Multi-Level Feedback Queue (MLFQ) scheduler
- Five priority levels
- Dynamic priority adjustment
- Periodic priority boosting
- Process lifecycle statistics including turnaround time, response time, and CPU execution time

**Concepts**
- Kernel Scheduling
- File System Operations
- System Calls
- Scheduling Policies
- Performance Evaluation

---

## Technologies

- C
- C++
- POSIX Threads (Pthreads)
- Unix/Linux System Calls
- Makefiles
- ELF Binary Format
- Memory Mapping (`mmap`)
- Inter-Process Communication (Pipes)

---

## Building

Each project contains its own Makefile.

Build a project by navigating to its directory and running:

```bash
make
```

To clean generated files:

```bash
make clean
```

---

## Operating Systems Concepts Covered

- ELF executable loading
- Virtual memory management
- Memory mapping
- Process creation and execution
- Unix shell implementation
- Inter-process communication
- CPU scheduling
- Round-Robin Scheduling
- Multi-Level Feedback Queue (MLFQ)
- User-level multithreading
- Context switching
- Thread synchronization
- Linux system programming
- Kernel scheduling
- File system operations

---

## Contributors

**Nakul Zutshi (2024361)**
- ELF Loader implementation
- CPU Scheduler logic
- Two-dimensional multithreading abstraction
- MLFQ scheduler and kernel integration

**Rijul Agarwal (2024459)**
- ELF Loader execution pipeline
- Shell implementation and IPC
- One-dimensional multithreading abstraction
- Shell utilities and file-system operations
