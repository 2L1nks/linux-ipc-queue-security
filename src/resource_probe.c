#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t interrupted = 0;
static void stop(int signum) { (void)signum; interrupted = 1; }
struct message { long type; char data[512]; };

static long parse_number(const char *text, long lower, long upper) {
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (!end || *end || value < lower || value > upper) return -1;
    return value;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s QUEUES(1..16) BYTES_PER_QUEUE(128..4096) HOLD_SECONDS(1..15)\n", argv[0]);
        return 2;
    }
    long num = parse_number(argv[1], 1, 16);
    long bytes = parse_number(argv[2], 128, 4096);
    long hold = parse_number(argv[3], 1, 15);
    if (num == -1 || bytes == -1 || hold == -1) { fputs("Unsafe/invalid arguments\n", stderr); return 2; }
    struct sigaction sa = {.sa_handler = stop};
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    int queues[16];
    int count = 0;
    unsigned long messages = 0, payload_bytes = 0;
    struct message message = {.type = 1};
    memset(message.data, 'R', sizeof(message.data));
    for (long i = 0; i < num && !interrupted; ++i) {
        int q = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
        if (q == -1) { perror("resource msgget"); break; }
        queues[count++] = q;
        struct msqid_ds ds;
        if (msgctl(q, IPC_STAT, &ds) == -1) { perror("resource IPC_STAT"); break; }
        if ((unsigned long)bytes < ds.msg_qbytes) {
            ds.msg_qbytes = (unsigned long)bytes;
            if (msgctl(q, IPC_SET, &ds) == -1) { perror("resource IPC_SET"); break; }
        }
        size_t payload = bytes < (long)sizeof(message.data) ? (size_t)bytes : sizeof(message.data);
        for (int attempts = 0; attempts < 64; ++attempts) {
            if (msgsnd(q, &message, payload, IPC_NOWAIT) == -1) {
                if (errno != EAGAIN) perror("resource msgsnd");
                break;
            }
            ++messages; payload_bytes += payload;
        }
    }
    printf("RESOURCE queues_created=%d requested_queues=%ld payload_bytes=%lu messages=%lu hold_seconds=%ld\n",
           count, num, payload_bytes, messages, hold);
    printf("RESULT kind=resource limit=%ld messages=%lu bytes=%lu queues=%d\n",
           bytes, messages, payload_bytes, count);
    fflush(stdout);
    for (long i = 0; i < hold && !interrupted; ++i) sleep(1);
    int cleanup_errors = 0;
    for (int i = 0; i < count; ++i) if (msgctl(queues[i], IPC_RMID, NULL) == -1) { perror("resource IPC_RMID"); ++cleanup_errors; }
    printf("RESOURCE CLEANUP=%s removed=%d\n", cleanup_errors ? "FAILED" : "OK", count - cleanup_errors);
    return cleanup_errors ? 1 : 0;
}
