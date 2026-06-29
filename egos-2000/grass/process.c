/*
 * (C) 2025, Cornell University
 * All rights reserved.
 *
 * Description: helper functions for process management
 */

#include "process.h"

#define MLFQ_NLEVELS          5
#define MLFQ_RESET_PERIOD     10000000         /* 10 seconds */
#define MLFQ_LEVEL_RUNTIME(x) (x + 1) * 100000 /* e.g., 100ms for level 0 */
extern struct process proc_set[MAX_NPROCESS + 1];

static void proc_set_status(int pid, enum proc_status status) {
    for (uint i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == pid) proc_set[i].status = status;
}

void proc_set_ready(int pid) { proc_set_status(pid, PROC_READY); }
void proc_set_running(int pid) { proc_set_status(pid, PROC_RUNNING); }
void proc_set_runnable(int pid) { proc_set_status(pid, PROC_RUNNABLE); }
void proc_set_pending(int pid) { proc_set_status(pid, PROC_PENDING_SYSCALL); }

int proc_alloc() {
    static uint curr_pid = 0;
    for (uint i = 1; i <= MAX_NPROCESS; i++)
        if (proc_set[i].status == PROC_UNUSED) {
            proc_set[i].pid    = ++curr_pid;
            proc_set[i].status = PROC_LOADING;
            
            /* Student's code goes here (Preemptive Scheduler | System Call). */
            /* Initialization of lifecycle statistics, MLFQ or process sleep. */
            ulonglong current = mtime_get();
            proc_set[i].creation_time = current;
            proc_set[i].first_scheduled_time = 0;
            proc_set[i].termination_time = 0;
            proc_set[i].total_cpu_time = 0;
            proc_set[i].last_scheduled_time = 0;
            proc_set[i].timer_interrupts = 0;
            proc_set[i].first_run = 1; 
            proc_set[i].queue_level = 0;
            proc_set[i].level_runtime = 0;
            
            /* Student's code ends here. */
            return curr_pid;
        }
    FATAL("proc_alloc: reach the limit of %d processes", MAX_NPROCESS);
}

void proc_free(int pid) {
    /* Student's code goes here (Preemptive Scheduler). */
    /* Print the lifecycle statistics of the terminated process or processes. */
    
    if (pid != GPID_ALL) {
        for (uint i = 0; i < MAX_NPROCESS; i++) {
            if (proc_set[i].pid == pid && proc_set[i].status != PROC_UNUSED) {
                proc_set[i].termination_time = mtime_get();
                
                ulonglong turnaround_time = proc_set[i].termination_time - 
                                           proc_set[i].creation_time;
                ulonglong response_time = 0;
                if (proc_set[i].first_scheduled_time > 0 && 
                    proc_set[i].first_scheduled_time >= proc_set[i].creation_time) {
                    response_time = proc_set[i].first_scheduled_time - 
                                   proc_set[i].creation_time;
                }
                
                INFO("[STATS] Process PID=%d terminated", pid);
                INFO("  Turnaround Time: %d microseconds", (uint)turnaround_time);
                INFO("  Response Time: %d microseconds", (uint)response_time);
                INFO("  CPU Time: %d microseconds", (uint)proc_set[i].total_cpu_time);
                INFO("  Timer Interrupts: %d", proc_set[i].timer_interrupts);
                
                earth->mmu_free(pid);
                proc_set[i].status = PROC_UNUSED;
                break;
            }
        }
    } else {
        /* Free all user processes. */
        for (uint i = 0; i < MAX_NPROCESS; i++)
            if (proc_set[i].pid >= GPID_USER_START &&
                proc_set[i].status != PROC_UNUSED) {
                
                proc_set[i].termination_time = mtime_get();
                
                ulonglong turnaround_time = proc_set[i].termination_time - 
                                           proc_set[i].creation_time;
                
                ulonglong response_time = 0;
                if (proc_set[i].first_scheduled_time > 0 && 
                    proc_set[i].first_scheduled_time >= proc_set[i].creation_time) {
                    response_time = proc_set[i].first_scheduled_time - 
                                   proc_set[i].creation_time;
                }
                
                INFO("[STATS] Process PID=%d terminated", proc_set[i].pid);
                INFO("  Turnaround Time: %d microseconds", (uint)turnaround_time);
                INFO("  Response Time: %d microseconds", (uint)response_time);
                INFO("  CPU Time: %d microseconds", (uint)proc_set[i].total_cpu_time);
                INFO("  Timer Interrupts: %d", proc_set[i].timer_interrupts);
                
                earth->mmu_free(proc_set[i].pid);
                proc_set[i].status = PROC_UNUSED;
            }
    }
    
    /* Student's code ends here. */
}

void mlfq_update_level(struct process* p, ulonglong runtime) {
    /* Student's code goes here (Preemptive Scheduler). */

    /* Update the MLFQ-related fields in struct process* p after this
     * process has run on the CPU for another runtime microseconds. */
    p->level_runtime += runtime;
    ulonglong time_allotment = MLFQ_LEVEL_RUNTIME(p->queue_level);
    
    if (p->level_runtime >= time_allotment) {
        if (p->queue_level < MLFQ_NLEVELS - 1) {
            p->queue_level++;
            p->level_runtime = 0; 
        } else {
            p->level_runtime = 0;
        }
    }
    /* Student's code ends here. */
}

void mlfq_reset_level() {
    /* Student's code goes here (Preemptive Scheduler). */
    if (!earth->tty_input_empty()) {
        /* Reset the level of GPID_SHELL if there is pending keyboard input. */
        for (uint i = 0; i < MAX_NPROCESS; i++) {
            if (proc_set[i].pid == GPID_SHELL && 
                proc_set[i].status != PROC_UNUSED) {
                proc_set[i].queue_level = 0;
                proc_set[i].level_runtime = 0;
                break;
            }
        }
    }
    
    static ulonglong MLFQ_last_reset_time = 0;
    /* Reset the level of all processes every MLFQ_RESET_PERIOD microseconds. */
    
    ulonglong current_time = mtime_get();
    
    if (MLFQ_last_reset_time == 0) {
        MLFQ_last_reset_time = current_time;
    }
    
    if (current_time - MLFQ_last_reset_time >= MLFQ_RESET_PERIOD) {
        for (uint i = 0; i < MAX_NPROCESS; i++) {
            if (proc_set[i].status != PROC_UNUSED) {
                proc_set[i].queue_level = 0;
                proc_set[i].level_runtime = 0;
            }
        }
        MLFQ_last_reset_time = current_time;
    }
    

    /* Student's code ends here. */
}

void proc_sleep(int pid, uint usec) {
    /* Student's code goes here (System Call & Protection). */

    /* Update the sleep-related fields in the struct process for process pid. */

    /* Student's code ends here. */
}

void proc_coresinfo() {
    /* Student's code goes here (Multicore & Locks). */

    /* Print out the pid of the process running on each CPU core. */

    /* Student's code ends here. */
}
