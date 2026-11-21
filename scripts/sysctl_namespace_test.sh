#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -x bin/queue_count_probe ]] || { echo 'First run: make'; exit 2; }
[[ $(id -u) -eq 0 ]] || { echo 'Run: sudo ./scripts/sysctl_namespace_test.sh'; exit 2; }
printf 'Host System V limits (READ ONLY):\n'
sysctl kernel.msgmnb kernel.msgmni
printf '\nNew IPC namespace: bounds are changed ONLY within that namespace\n'
unshare --ipc --fork sh -eu -c '
  printf "Child IPC namespace: "; readlink /proc/self/ns/ipc
  sysctl -w kernel.msgmnb=2048 kernel.msgmni=4
  ./bin/sysv_limits 4096
  ./bin/queue_count_probe 6
'
printf '\nHost System V limits (unchanged):\n'
sysctl kernel.msgmnb kernel.msgmni
