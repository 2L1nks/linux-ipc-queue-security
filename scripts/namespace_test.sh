#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -x bin/namespace_probe ]] || { echo 'First run: make'; exit 2; }
command -v unshare >/dev/null || { echo 'unshare is missing'; exit 2; }
[[ $(id -u) -eq 0 ]] || { echo 'Run: sudo ./scripts/namespace_test.sh'; exit 2; }
key=$(printf '0x%x' "$((0x53000000 | ($$ & 65535)))")
cleanup() { ./bin/namespace_probe cleanup "$key" || echo 'Manual cleanup required (ipcs -q)'; }
./bin/namespace_probe create "$key"
trap cleanup EXIT
printf 'HOST IPC namespace: '; readlink /proc/self/ns/ipc
./bin/namespace_probe visible "$key"
echo '=== Child IPC namespace (requires unshare permission) ==='
unshare --ipc --fork sh -c 'printf "CHILD IPC namespace: "; readlink /proc/self/ns/ipc; exec ./bin/namespace_probe hidden "$1"' sh "$key"
echo 'PASS: queue is not visible from a new IPC namespace'
