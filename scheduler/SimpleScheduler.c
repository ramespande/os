#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define READY 0
#define RUNNING 1
#define FINISHED 2

struct Job {
    int pid;
    char name[256];
    int run_slices;
    int state;
    int arrival_slice;
    int completion_slice;
    struct Job *next;
};

int is_alive(int pid) {
    int status;
    pid_t ret = waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

    if (ret == 0) {
        return 1;
    }

    if (ret == pid) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            return 0;
        }
        return 1;
    }

    if (kill(pid, 0) == 0) {
        return 1;
    }
    return 0;
}

void enqueue_job(struct Job **queue, struct Job *job) {
    job->next = NULL;
    if (*queue == NULL) {
        *queue = job;
    } else {
        struct Job *curr = *queue;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = job;
    }
}

void move_finished_to_summary(struct Job **queue, struct Job **finished_list, int slice_number) {
    struct Job *prev = NULL, *curr = *queue;

    while (curr) {
        struct Job *next = curr->next;

        if (curr->state == FINISHED) {
            if (prev) {
                prev->next = next;
            } else {
                *queue = next;
            }

            curr->next = NULL;
            if (*finished_list == NULL) {
                *finished_list = curr;
            } else {
                struct Job *tail = *finished_list;
                while (tail->next) {
                    tail = tail->next;
                }
                tail->next = curr;
            }

            curr = next;
        } else {
            prev = curr;
            curr = next;
        }
    }
}

int queue_empty(struct Job *queue) {
    return queue == NULL;
}

void parse_and_enqueue(char *buf, struct Job **queue, int slice_number, int *shutdown) {
    char *line = strtok(buf, "\n");

    while (line != NULL) {
        if (strcmp(line, "SHUTDOWN") == 0) {
            *shutdown = 1;
        } else if (strncmp(line, "SUBMIT ", 7) == 0) {
            char *path = line + 7;

            pid_t pid = fork();
            if (pid == 0) {
                execl(path, path, NULL);
                perror("exec failed");
                exit(1);
            } else if (pid > 0) {
                struct Job *job = malloc(sizeof(struct Job));
                job->pid = pid;
                strncpy(job->name, path, sizeof(job->name) - 1);
                job->name[sizeof(job->name) - 1] = '\0';
                job->state = READY;
                job->arrival_slice = slice_number + 1;
                job->run_slices = 0;
                job->completion_slice = -1;
                job->next = NULL;

                enqueue_job(queue, job);
            } else {
                perror("fork failed");
            }
        }
        line = strtok(NULL, "\n");
    }
}

void print_summary(int report_fd, struct Job *finished_jobs) {
    dprintf(report_fd, "\n========================================\n");
    dprintf(report_fd, "Job Summary (in multiples of TSLICE):\n");
    dprintf(report_fd, "========================================\n");
    dprintf(report_fd, "Name\t\tPID\tCompletion\tWaiting\n");
    dprintf(report_fd, "----------------------------------------\n");

    struct Job *summary_curr = finished_jobs;
    while (summary_curr) {
        int completion = summary_curr->run_slices;
        int waiting = (summary_curr->completion_slice - summary_curr->arrival_slice) - summary_curr->run_slices;
        
        if (waiting < 0) {
            waiting = 0;
        }

        dprintf(report_fd, "%s\t\t%d\t\t%d\t\t%d\n",
               summary_curr->name, summary_curr->pid, completion, waiting);

        struct Job *next = summary_curr->next;
        free(summary_curr);
        summary_curr = next;
    }

    dprintf(report_fd, "DONE\n");
}

struct Job *global_finished_jobs = NULL;
int global_report_fd = -1;

void handle_sigint(int sig) {
    if (global_report_fd != -1 && global_finished_jobs != NULL) {
        print_summary(global_report_fd, global_finished_jobs);
    }
    exit(0);
}

void scheduler(int control_fd, int report_fd, int NCPU, int TSLICE_ms) {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, handle_sigint);

    global_report_fd = report_fd;

    int flags = fcntl(control_fd, F_GETFL, 0);
    fcntl(control_fd, F_SETFL, flags | O_NONBLOCK);

    struct Job *ready_queue = NULL;
    struct Job *finished_jobs = NULL;
    int shutdown = 0;
    int slice_number = 0;

    while (1) {
        char buf[4096];
        int n = read(control_fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            parse_and_enqueue(buf, &ready_queue, slice_number, &shutdown);
            usleep(5000);
        } else if (n == 0) {
            shutdown = 1;
        }

        if (shutdown && queue_empty(ready_queue)) {
            break;
        }

        struct Job *to_run[NCPU];
        int count = 0;
        struct Job *curr = ready_queue;

        while (curr && count < NCPU) {
            if (curr->state == READY && curr->arrival_slice <= slice_number) {
                to_run[count++] = curr;
            }
            curr = curr->next;
        }

        if (count == 0) {
            nanosleep(&(struct timespec){0, 10000000L}, NULL);
            slice_number++;
            continue;
        }

        for (int i = 0; i < count; i++) {
            if (is_alive(to_run[i]->pid)) {
                to_run[i]->state = RUNNING;
                to_run[i]->run_slices++;
                kill(to_run[i]->pid, SIGCONT);
            } else {
                to_run[i]->state = FINISHED;
                to_run[i]->completion_slice = slice_number;
                if (to_run[i]->run_slices == 0) {
                    to_run[i]->run_slices = 1;
                }
            }
        }

        nanosleep(&(struct timespec){0, (long)TSLICE_ms * 1000000L}, NULL);

        for (int i = 0; i < count; i++) {
            if (to_run[i]->state == RUNNING) {
                if (is_alive(to_run[i]->pid)) {
                    kill(to_run[i]->pid, SIGSTOP);
                    to_run[i]->state = READY;
                } else {
                    to_run[i]->state = FINISHED;
                    to_run[i]->completion_slice = slice_number + 1;
                }
            }
        }

        move_finished_to_summary(&ready_queue, &finished_jobs, slice_number);
        global_finished_jobs = finished_jobs;

        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) {
        }

        slice_number++;
    }

    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);

    print_summary(report_fd, finished_jobs);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s ctl_fd rpt_fd NCPU TSLICE_ms\n", argv[0]);
        return 1;
    }

    int control_fd = atoi(argv[1]);
    int report_fd = atoi(argv[2]);
    int NCPU = atoi(argv[3]);
    int TSLICE_ms = atoi(argv[4]);

    scheduler(control_fd, report_fd, NCPU, TSLICE_ms);
    return 0;
}
