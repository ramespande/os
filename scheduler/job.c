#include <signal.h>      // include before dummy_main.h
#include <stdio.h>
#include "dummy_main.h"

int main(int argc, char **argv) {
    for (int i = 0; i < 5; i++) {
        printf("Job running iteration %d\n", i);
        fflush(stdout);
    }
    return 0;
}
