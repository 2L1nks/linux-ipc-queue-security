#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
for program in sysv_demo posix_demo sysv_limits posix_limits; do
  echo "=== verify $program ==="
  if [[ "$program" == sysv_limits ]]; then output=$(timeout 8 ./bin/$program 2048)
  elif [[ "$program" == posix_limits ]]; then output=$(timeout 8 ./bin/$program 2)
  else output=$(timeout 8 ./bin/$program)
  fi
  printf '%s\n' "$output"
  grep -q 'CLEANUP=OK' <<< "$output" || { echo "FAIL $program cleanup"; exit 1; }
  if [[ "$program" == *limits ]]; then grep -q 'errno_full=11' <<< "$output" || { echo "FAIL $program full limit"; exit 1; }; fi
  echo "PASS $program"
done
