#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static long sysctl_value(const char *path, long fallback) {
    FILE *f = fopen(path, "r");
    long val = fallback;
    if (f) { if (fscanf(f, "%ld", &val) != 1) val = fallback; fclose(f); }
    return val;
}

int main(int argc, char **argv) {
    long wanted = 4;
    if (argc > 2) { fprintf(stderr, "Usage: %s [max_messages:1..4]\n", argv[0]); return 2; }
    if (argc == 2) {
        char *end = NULL;
        wanted = strtol(argv[1], &end, 10);
        if (!end || *end || wanted < 1 || wanted > 4) { fputs("max_messages must be 1..4\n", stderr); return 2; }
    }
    long sysmax = sysctl_value("/proc/sys/fs/mqueue/msg_max", 4);
    long sysmsgsize = sysctl_value("/proc/sys/fs/mqueue/msgsize_max", 256);
    if (sysmax < 1 || sysmsgsize < 1) { fputs("POSIX mqueue limits invalid\n", stderr); return 1; }
    if (wanted > sysmax) wanted = sysmax;
    long size = sysmsgsize < 256 ? sysmsgsize : 256;
    char name[64];
    snprintf(name, sizeof(name), "/ipc_course_limit_%ld", (long)getpid());
    struct mq_attr attr = {.mq_flags = 0, .mq_maxmsg = wanted, .mq_msgsize = size, .mq_curmsgs = 0};
    mqd_t q = mq_open(name, O_CREAT | O_EXCL | O_RDWR | O_NONBLOCK, 0600, &attr);
    if (q == (mqd_t)-1) { perror("mq_open (check RLIMIT_MSGQUEUE / namespace POSIX limits)"); return 1; }
    int result = 1;
    char *buffer = calloc((size_t)size + 1, 1);
    if (!buffer) { perror("calloc"); goto cleanup; }
    memset(buffer, 'B', (size_t)size + 1);
    int sent = 0, full_errno = 0;
    while (sent < 10) {
        if (mq_send(q, buffer, (size_t)size, 1) == -1) { full_errno = errno; break; }
        ++sent;
    }
    if (mq_getattr(q, &attr) == -1) { perror("mq_getattr"); goto cleanup_buffer; }
    printf("POSIX_LIMIT name=%s maxmsg=%ld msgsize=%ld sent=%d curmsgs=%ld full_errno=%d (%s)\n",
           name, attr.mq_maxmsg, attr.mq_msgsize, sent, attr.mq_curmsgs,
           full_errno, full_errno ? strerror(full_errno) : "limit not hit");
    printf("RESULT kind=posix limit=%ld messages=%d bytes=%ld errno_full=%d\n",
           attr.mq_maxmsg, sent, sent * attr.mq_msgsize, full_errno);
    if (full_errno != EAGAIN || sent != wanted) { fputs("Unexpected POSIX limit result\n", stderr); goto cleanup_buffer; }
    errno = 0;
    int rc = mq_send(q, buffer, (size_t)size + 1, 1);
    int saved_errno = errno;
    printf("POSIX_OVERSIZE size=%ld rc=%d errno=%d (%s)\n",
           size + 1, rc, saved_errno, strerror(saved_errno));
    if (rc != -1 || saved_errno != EMSGSIZE) goto cleanup_buffer;
    result = 0;
cleanup_buffer:
    free(buffer);
cleanup:
    if (mq_close(q) == -1) { perror("mq_close"); result = 1; }
    if (mq_unlink(name) == -1) { perror("mq_unlink"); result = 1; }
    else puts("POSIX_LIMIT CLEANUP=OK");
    return result;
}
