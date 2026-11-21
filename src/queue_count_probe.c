#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Usage: %s QUEUE_COUNT(1..32)\n", argv[0]); return 2; }
    char *end = NULL;
    long requested = strtol(argv[1], &end, 10);
    if (!end || *end || requested < 1 || requested > 32) { fputs("Invalid count\n", stderr); return 2; }
    int ids[32];
    int count = 0;
    int failure_errno = 0;
    for (long i = 0; i < requested; ++i) {
        int q = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
        if (q == -1) { failure_errno = errno; break; }
        ids[count++] = q;
    }
    printf("QUEUE_COUNT requested=%ld created=%d stop_errno=%d (%s)\n",
           requested, count, failure_errno, failure_errno ? strerror(failure_errno) : "none");
    printf("RESULT kind=queue_count limit=%ld messages=%d bytes=0 errno_full=%d\n",
           requested, count, failure_errno);
    int cleanup_errors = 0;
    for (int i = 0; i < count; ++i) if (msgctl(ids[i], IPC_RMID, NULL) == -1) { perror("IPC_RMID"); ++cleanup_errors; }
    printf("QUEUE_COUNT CLEANUP=%s\n", cleanup_errors ? "FAILED" : "OK");
    return cleanup_errors ? 1 : 0;
}
