#include "app.h"

int main(int argc, char** argv) {

    if (argc != 3) {
        INFO("Usage: grep PATTERN FILE");
        return -1;
    }

    char* pattern  = argv[1];
    char* filename = argv[2];
    int ino = dir_lookup(workdir_ino, filename);
    if (ino < 0) {
        INFO("grep: file %s not found", filename);
        return -1;
    }
    char buf[BLOCK_SIZE];
    file_read(ino, 0, buf);
    char* line = buf;
    while (*line) {
        char* end = strchr(line, '\n');
        if (!end) end = line + strlen(line);
        char saved = *end;
        *end = '\0';
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\r')
            line[len - 1] = '\0';
        if (strstr(line, pattern) != NULL) {
            printf("%s\n", line);
        }

        *end = saved;
        if (*end == '\0') break;
        line = end + 1;
    }

    return 0;
}
