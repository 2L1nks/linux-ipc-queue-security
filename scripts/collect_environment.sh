#!/usr/bin/env bash
set -euo pipefail
printf '=== Collected UTC ===\n'; date -u '+%Y-%m-%dT%H:%M:%SZ'
printf '\n=== OS ===\n'; cat /etc/os-release
printf '\n=== Kernel / architecture ===\n'; uname -srvmo
printf '\n=== User and namespaces ===\n'; id; readlink /proc/self/ns/ipc || true
printf '\n=== System V tunables ===\n'
for name in kernel.msgmax kernel.msgmnb kernel.msgmni; do sysctl "$name"; done
printf '\n=== POSIX mqueue tunables ===\n'
for name in fs.mqueue.queues_max fs.mqueue.msg_max fs.mqueue.msgsize_max fs.mqueue.msg_default fs.mqueue.msgsize_default; do sysctl "$name" 2>/dev/null || true; done
printf '\n=== RLIMIT_MSGQUEUE / limits ===\n'; prlimit --pid "$$" --msgqueue || true
printf '\n=== IPC limits and current state ===\n'; ipcs -l; ipcs -u
printf '\n=== mqueue filesystem ===\n'; findmnt /dev/mqueue || true
printf '\n=== Installed tools ===\n'
for cmd in gcc make strace unshare ipcs python3 git gh; do command -v "$cmd" || true; done
