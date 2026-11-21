#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct message { long mtype; char text[128]; };

int main(void) {
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (qid == -1) { perror("msgget"); return EXIT_FAILURE; }
    printf("SYSV queue_id=%d parent_pid=%ld\n", qid, (long)getpid());
    fflush(stdout);

    struct message sent = {.mtype = 1};
    snprintf(sent.text, sizeof(sent.text), "Hello via System V IPC");
    size_t length = strlen(sent.text) + 1;
    if (msgsnd(qid, &sent, length, IPC_NOWAIT) == -1) {
        perror("msgsnd"); msgctl(qid, IPC_RMID, NULL); return EXIT_FAILURE;
    }
    printf("SYSV SENT bytes=%zu\n", length);
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); msgctl(qid, IPC_RMID, NULL); return EXIT_FAILURE; }
    if (pid == 0) {
        struct message received = {0};
        ssize_t received_len = msgrcv(qid, &received, sizeof(received.text), 1, IPC_NOWAIT);
        if (received_len == -1) { perror("child msgrcv"); _exit(EXIT_FAILURE); }
        printf("SYSV RECEIVED child_pid=%ld type=%ld bytes=%zd text=%s\n",
               (long)getpid(), received.mtype, received_len, received.text);
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }
    int status = 0;
    int child_status = 0;
    if (waitpid(pid, &child_status, 0) == -1) { perror("waitpid"); status = 1; }
    else if (!WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) status = 1;
    if (msgctl(qid, IPC_RMID, NULL) == -1) { perror("IPC_RMID"); status = 1; }
    else puts("SYSV CLEANUP=OK");
    return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
