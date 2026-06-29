#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

int control_pipe[2];
int report_pipe[2];
pid_t scheduler_pid;
int NCPU, TSLICE;

void handle_sigint(int sig) {
    printf("\n[SimpleShell] Interrupted. Sending shutdown signal...\n");
    char msg[] = "SHUTDOWN\n";
    write(control_pipe[1], msg, strlen(msg));
    close(control_pipe[1]);
    
    printf("\n[SimpleShell] Waiting for scheduler to finish...\n\n");
    char buf[1024];
    int n;
    while ((n = read(report_pipe[0], buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        if (strstr(buf, "DONE")) break;
    }
    
    waitpid(scheduler_pid, NULL, 0);
    printf("\n[SimpleShell] Exiting.\n");
    exit(0);
}

char* read_user_input() {
    char* input = malloc(102400);
    if (!input) {
        fprintf(stderr, "allocation error\n");
        exit(1);
    }
    int index = 0;
    char c;
    while (1) {
        if (scanf("%c", &c) != 1) {
            break;
        }
        if (c == '\n') {
            input[index] = '\0';
            break;
        }
        input[index] = c;
        index++;
        if (index >= 102399) {
            input[index] = '\0';
            break;
        }
    }
    return input;
}

int launch(char *command) {
    if (strncmp(command, "submit ", 7) == 0) {
        char *path = command + 7;

        if (access(path, X_OK) != 0) {
            printf("Executable not found or not executable.\n");
            return 1;
        }
        
        printf("[SimpleShell] Submitting job %s\n", path);
        fflush(stdout);
        
        char msg[1024];
        snprintf(msg, sizeof(msg), "SUBMIT %s\n", path);
        write(control_pipe[1], msg, strlen(msg));
        
        return 1;
    }
    else if (strcmp(command, "exit") == 0) {
        printf("[SimpleShell] Sending shutdown signal...\n");
        char msg[] = "SHUTDOWN\n";
        write(control_pipe[1], msg, strlen(msg));
        close(control_pipe[1]);
        
        printf("\n[SimpleShell] Waiting for scheduler to finish...\n\n");
        char buf[1024];
        int n;
        while ((n = read(report_pipe[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("%s", buf);
            if (strstr(buf, "DONE")) break;
        }
        
        waitpid(scheduler_pid, NULL, 0);
        printf("\n[SimpleShell] Exiting.\n");
        return 0;
    }
    else {
        printf("Only submit command allowed\n");
        return 1;
    }
    return 0;
}

void start_scheduler(int NCPU, int TSLICE) {
    if (pipe(control_pipe) == -1 || pipe(report_pipe) == -1) {
        perror("pipe failed");
        exit(1);
    }

    scheduler_pid = fork();
    if (scheduler_pid == 0) {
        close(control_pipe[1]);
        close(report_pipe[0]);

        char ctl_fd[10], rpt_fd[10], ncpu_str[10], tslice_str[10];
        sprintf(ctl_fd, "%d", control_pipe[0]);
        sprintf(rpt_fd, "%d", report_pipe[1]);
        sprintf(ncpu_str, "%d", NCPU);
        sprintf(tslice_str, "%d", TSLICE);

        execl("./SimpleScheduler", "SimpleScheduler", ctl_fd, rpt_fd, ncpu_str, tslice_str, NULL);
        perror("exec failed");
        exit(1);
    } else if (scheduler_pid > 0) {
        close(control_pipe[0]);
        close(report_pipe[1]);
    } else {
        perror("fork failed");
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE_ms>\n", argv[0]);
        exit(1);
    }
    
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2]);
    printf("[SimpleShell] Started with %d CPUs and %d ms time slice.\n", NCPU, TSLICE);
    
    signal(SIGINT, handle_sigint);
    
    int status;
    start_scheduler(NCPU, TSLICE);
    
    do {
        printf("SimpleShell$ ");
        char* command = read_user_input();
        status = launch(command);
        free(command);
    } while (status);
    
    return 0;
}
