#include "app.h"

int main(int argc, char **argv) {

    if (argc < 2) {
        INFO("Usage: wcl FILE...");
        return -1;
    }

    for (int a = 1; a < argc; a++) {

        char *filename = argv[a];
        int ino = dir_lookup(workdir_ino, filename);
        if (ino < 0) {
            INFO("wcl: file %s not found", filename);
            continue;   
        }
        char buf[BLOCK_SIZE];
        file_read(ino, 0, buf);
        int count = 0;
        for (char *p = buf; *p; p++) {
            if (*p == '\n') count++;
        }
        printf("%d %s\n", count, filename);
    }

    return 0;
}
