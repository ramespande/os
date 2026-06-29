# scheduler

## Project Overview
This project implements a simplified CPU scheduler and shell system in C. The shell accepts user commands and communicates with a separate scheduler process via inter-process communication. The scheduler manages multiple CPU threads, executing submitted tasks based on a fixed time slice.

## Implementation
The shell and scheduler communicate bidirectionally using pipes. The shell forks the scheduler, passes file descriptors and configuration parameters (NCPU, TSLICE), and sends job requests. Each submitted program runs via dummy_main to simulate execution control. The scheduler maintains job queues, handles task switching using time slices, and reports job status back to the shell.

## Limitations
No persistent storage; all state is lost after shutdown.
Limited command set (only SUBMIT).
No priority-based or dynamic scheduling policies.

## Contributions 
### Rijul:
Shell-scheduler IPC, process creation, command parsing.
### Nakul:
Scheduler logic, queue management, time-slice handling.
### Github Link:
https://github.com/rijulagarwal/scheduler

### Made by:
Rijul Agarwal 2024459

Nakul Zutshi 2024361
