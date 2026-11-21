#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

static int parse_key(const char *s, key_t *out) {
    char *end = NULL;
    unsigned long k = strtoul(s, &end, 0);
    if (!end || *end || k == 0 || k > 0x7fffffffUL) return -1;
    *out = (key_t)k;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s create|check|cleanup KEY (e.g. 0x50012345)\n", argv[0]); return 2;
    }
    key_t key;
    if (parse_key(argv[2], &key) != 0) { fputs("Invalid key\n", stderr); return 2; }
    if (strcmp(argv[1], "create") == 0) {
        int qid = msgget(key, IPC_CREAT | IPC_EXCL | 0600);
        if (qid == -1) { perror("msgget create"); return 1; }
        printf("PERMISSION CREATED key=0x%lx qid=%d uid=%ld mode=0600\n",
               (unsigned long)key, qid, (long)getuid());
        return 0;
    }
    if (strcmp(argv[1], "check") == 0) {
        errno = 0;
        int qid = msgget(key, 0400); /* request read permission */
        if (qid == -1 && errno == EACCES) {
            printf("PERMISSION DENIED uid=%ld errno=%d (%s) EXPECTED\n",
                   (long)getuid(), errno, strerror(errno)); return 0;
        }
        if (qid == -1) fprintf(stderr, "PERMISSION UNEXPECTED errno=%d (%s)\n", errno, strerror(errno));
        else fprintf(stderr, "PERMISSION UNEXPECTED qid=%d (access was allowed)\n", qid);
        return 1;
    }
    if (strcmp(argv[1], "cleanup") == 0) {
        int qid = msgget(key, 0);
        if (qid == -1) { perror("msgget cleanup"); return 1; }
        if (msgctl(qid, IPC_RMID, NULL) == -1) { perror("IPC_RMID"); return 1; }
        puts("PERMISSION CLEANUP=OK");
        return 0;
    }
    fprintf(stderr, "Unknown mode: %s\n", argv[1]);
    return 2;
}
