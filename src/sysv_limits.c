#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct msg { long type; char payload[512]; };

static long read_msgmax(void) {
    FILE *f = fopen("/proc/sys/kernel/msgmax", "r");
    long n = 0;
    if (f) { if (fscanf(f, "%ld", &n) != 1) n = 0; fclose(f); }
    return n;
}

int main(int argc, char **argv) {
    unsigned long requested = 4096;
    if (argc > 2) { fprintf(stderr, "Usage: %s [queue_bytes:128..4096]\n", argv[0]); return 2; }
    if (argc == 2) {
        char *end = NULL;
        requested = strtoul(argv[1], &end, 10);
        if (!end || *end != '\0' || requested < 128 || requested > 4096) {
            fputs("queue_bytes must be between 128 and 4096\n", stderr); return 2;
        }
    }
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) { perror("msgget"); return 1; }
    int result = 1;
    struct msqid_ds ds;
    if (msgctl(qid, IPC_STAT, &ds) == -1) { perror("IPC_STAT"); goto cleanup; }
    if (requested > ds.msg_qbytes) requested = ds.msg_qbytes;
    if (requested < 128) { fputs("Queue has unexpectedly low msg_qbytes\n", stderr); goto cleanup; }
    ds.msg_qbytes = requested;
    if (msgctl(qid, IPC_SET, &ds) == -1) { perror("IPC_SET"); goto cleanup; }
    struct msg message = {.type = 1};
    memset(message.payload, 'A', sizeof(message.payload));
    long maxmsg = read_msgmax();
    size_t payload = sizeof(message.payload);
    if (maxmsg > 0 && (unsigned long)maxmsg < payload) payload = (size_t)maxmsg;
    if (payload > requested) payload = requested;
    if (payload == 0) { fputs("msgmax is zero\n", stderr); goto cleanup; }
    int sent = 0;
    int full_errno = 0;
    while (sent < 100) {
        if (msgsnd(qid, &message, payload, IPC_NOWAIT) == -1) {
            full_errno = errno;
            break;
        }
        ++sent;
    }
    if (msgctl(qid, IPC_STAT, &ds) == -1) { perror("IPC_STAT"); goto cleanup; }
    printf("SYSV_LIMIT queue_id=%d msgmax=%ld queue_bytes_limit=%lu payload=%zu\n",
           qid, maxmsg, requested, payload);
    printf("SYSV_LIMIT sent=%d qnum=%lu used_bytes=%lu full_errno=%d (%s)\n",
           sent, (unsigned long)ds.msg_qnum, (unsigned long)ds.__msg_cbytes,
           full_errno, full_errno ? strerror(full_errno) : "limit not hit");
    printf("RESULT kind=sysv limit=%lu messages=%d bytes=%lu errno_full=%d\n",
           requested, sent, (unsigned long)ds.__msg_cbytes, full_errno);
    if (full_errno != EAGAIN) { fputs("Unexpected result: did not reach EAGAIN\n", stderr); goto cleanup; }
    if (maxmsg > 0 && maxmsg <= 65536) {
        size_t oversized = (size_t)maxmsg + 1;
        void *buffer = calloc(1, sizeof(long) + oversized);
        if (!buffer) { perror("calloc"); goto cleanup; }
        *(long *)buffer = 1;
        errno = 0;
        int rc = msgsnd(qid, buffer, oversized, IPC_NOWAIT);
        int saved_errno = errno;
        printf("SYSV_OVERSIZE size=%zu rc=%d errno=%d (%s)\n",
               oversized, rc, saved_errno, strerror(saved_errno));
        free(buffer);
        if (rc != -1 || saved_errno != EINVAL) goto cleanup;
    } else puts("SYSV_OVERSIZE skipped: msgmax out of safe test range");
    result = 0;
cleanup:
    if (msgctl(qid, IPC_RMID, NULL) == -1) { perror("IPC_RMID"); result = 1; }
    else puts("SYSV_LIMIT CLEANUP=OK");
    return result;
}
