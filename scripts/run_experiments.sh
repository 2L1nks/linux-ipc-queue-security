#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -x bin/resource_probe ]] || { echo 'First run: make'; exit 2; }
run="results/runs/$(date -u +%Y%m%dT%H%M%SZ)"
mkdir -p "$run"
echo "OUTPUT_DIR=$run"
./scripts/collect_environment.sh > "$run/environment.txt"
./bin/sysv_demo > "$run/sysv_demo.log"
./bin/posix_demo > "$run/posix_demo.log"
./bin/sysv_limits 4096 > "$run/sysv_limits_4096.log"
./bin/sysv_limits 2048 > "$run/sysv_limits_2048.log"
./bin/posix_limits 4 > "$run/posix_limits_4.log"
./bin/posix_limits 2 > "$run/posix_limits_2.log"
if command -v strace >/dev/null; then
  if ! strace -f -e trace=msgget,msgsnd,msgrcv,msgctl -o "$run/sysv_strace.log" ./bin/sysv_demo > /dev/null; then
    echo 'strace System V unavailable: ptrace may be restricted' > "$run/sysv_strace_warning.txt"
  fi
  if ! strace -f -e trace=mq_open,mq_timedsend,mq_timedreceive,mq_unlink,mq_getsetattr -o "$run/posix_strace.log" ./bin/posix_demo > /dev/null; then
    echo 'strace POSIX unavailable: ptrace may be restricted' > "$run/posix_strace_warning.txt"
  fi
fi
snapshot() {
  echo "=== SNAPSHOT $1 ==="
  date -u '+%Y-%m-%dT%H:%M:%SZ'
  ipcs -u
  echo '--- /proc/meminfo (KiB) ---'
  grep -E '^(MemAvailable|Slab|SReclaimable|SUnreclaim|Committed_AS):' /proc/meminfo
  echo '--- /proc/sysvipc/msg ---'
  cat /proc/sysvipc/msg
}
snapshot BEFORE > "$run/resource_state.log"
./bin/resource_probe 8 2048 5 > "$run/resource_probe.log" &
probe_pid=$!
trap 'kill -TERM "$probe_pid" 2>/dev/null || true; wait "$probe_pid" 2>/dev/null || true' EXIT
sleep 1
snapshot DURING >> "$run/resource_state.log"
wait "$probe_pid"
trap - EXIT
snapshot AFTER >> "$run/resource_state.log"
python3 scripts/plot_results.py "$run" || echo 'Graph unavailable (see logs)'
printf '\n=== Results ===\n'
grep -h '^RESULT ' "$run"/sysv_limits_*.log "$run"/posix_limits_*.log "$run"/resource_probe.log
printf '\nInspect local evidence: %s\n' "$run"
printf 'Files in results/runs are ignored by Git until reviewed and copied to results/sanitized/.\n'
