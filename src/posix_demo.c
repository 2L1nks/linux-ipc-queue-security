#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char name[64];
    snprintf(name, sizeof(name), "/ipc_course_demo_%ld", (long)getpid());
    struct mq_attr attr = {.mq_flags = 0, .mq_maxmsg = 4, .mq_msgsize = 128, .mq_curmsgs = 0};
    mqd_t q = mq_open(name, O_CREAT | O_EXCL | O_RDWR, 0600, &attr);
    if (q == (mqd_t)-1) { perror("mq_open"); return EXIT_FAILURE; }
    printf("POSIX queue=%s parent_pid=%ld\n", name, (long)getpid());
    fflush(stdout);
    const char *data = "Hello via POSIX mqueue";
    if (mq_send(q, data, strlen(data) + 1, 3) == -1) {
        perror("mq_send"); mq_close(q); mq_unlink(name); return EXIT_FAILURE;
    }
    printf("POSIX SENT priority=3 bytes=%zu\n", strlen(data) + 1);
    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); mq_close(q); mq_unlink(name); return EXIT_FAILURE; }
    if (pid == 0) {
        char buffer[128];
        unsigned priority = 0;
        ssize_t len = mq_receive(q, buffer, sizeof(buffer), &priority);
        if (len == -1) { perror("child mq_receive"); _exit(EXIT_FAILURE); }
        if ((size_t)len >= sizeof(buffer)) buffer[sizeof(buffer)-1] = '\0';
        else buffer[len] = '\0';
        printf("POSIX RECEIVED child_pid=%ld priority=%u bytes=%zd text=%s\n",
               (long)getpid(), priority, len, buffer);
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    }
    int status = 0;
    int child_status = 0;
    if (waitpid(pid, &child_status, 0) == -1) { perror("waitpid"); status = 1; }
    else if (!WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) status = 1;
    if (mq_close(q) == -1) { perror("mq_close"); status = 1; }
    if (mq_unlink(name) == -1) { perror("mq_unlink"); status = 1; }
    else puts("POSIX CLEANUP=OK");
    return status ? EXIT_FAILURE : EXIT_SUCCESS;
}
