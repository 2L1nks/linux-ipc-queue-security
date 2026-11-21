#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s create|visible|hidden|cleanup KEY\n", argv[0]); return 2;
    }
    char *end = NULL;
    unsigned long value = strtoul(argv[2], &end, 0);
    if (!end || *end || value == 0 || value > 0x7fffffffUL) { fputs("Invalid key\n", stderr); return 2; }
    key_t key = (key_t)value;
    if (strcmp(argv[1], "create") == 0) {
        int q = msgget(key, IPC_CREAT | IPC_EXCL | 0600);
        if (q == -1) { perror("create queue"); return 1; }
        printf("NAMESPACE CREATED key=0x%lx qid=%d\n", value, q); return 0;
    }
    if (strcmp(argv[1], "visible") == 0) {
        int q = msgget(key, 0400);
        if (q == -1) { perror("visible queue"); return 1; }
        printf("NAMESPACE VISIBLE key=0x%lx qid=%d\n", value, q); return 0;
    }
    if (strcmp(argv[1], "hidden") == 0) {
        errno = 0;
        int q = msgget(key, 0400);
        if (q == -1 && errno == ENOENT) {
            printf("NAMESPACE HIDDEN key=0x%lx errno=%d (%s) EXPECTED\n",
                   value, errno, strerror(errno)); return 0;
        }
        if (q == -1) fprintf(stderr, "NAMESPACE unexpected errno=%d (%s)\n", errno, strerror(errno));
        else fprintf(stderr, "NAMESPACE unexpected visible qid=%d\n", q);
        return 1;
    }
    if (strcmp(argv[1], "cleanup") == 0) {
        int q = msgget(key, 0);
        if (q == -1) { perror("lookup cleanup"); return 1; }
        if (msgctl(q, IPC_RMID, NULL) == -1) { perror("IPC_RMID"); return 1; }
        puts("NAMESPACE CLEANUP=OK"); return 0;
    }
    fputs("Unknown mode\n", stderr);
    return 2;
}
