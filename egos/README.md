# egos

## Project Overview
This project implements a lightweight operating system with custom shell commands and a Multi-Level Feedback Queue (MLFQ) scheduler within the EGOS-2000 environment. 
The shell utilities (grep and wcl) use low-level system calls for file operations. The MLFQ scheduler manages process execution based on priority and runtime behavior, 
tracking lifecycle statistics like turnaround time, response time, and CPU time.

## Implementation
The grep utility searches for patterns in files and prints matching lines. The wcl utility counts lines across multiple files. Both use dir_lookup() and file_read() system calls to read 
entire files block-by-block. The MLFQ scheduler implements 5 priority levels (0-4), where processes start at level 0 and move down after exhausting their time slice ((i+1)×100ms). 
Every 10 seconds, all processes are boosted back to level 0. The scheduler tracks creation time, first scheduled time, CPU time, and timer interrupts for each process.

## Limitations
No persistent storage of statistics.
MLFQ applies only to user processes.
Scheduler does not support advanced policies beyond basic MLFQ rules.

## Contributions 
### Rijul:
Shell utilities (grep, wcl), file I/O logic, testing.
### Nakul:
MLFQ scheduler, lifecycle statistics, kernel integration.
### Github Link:
https://github.com/ramespande/egos

### Made by:
Rijul Agarwal 2024459

Nakul Zutshi 2024361
