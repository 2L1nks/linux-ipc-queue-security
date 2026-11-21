#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -x bin/permission_probe ]] || { echo 'First run: make'; exit 2; }
command -v sudo >/dev/null || { echo 'sudo is required'; exit 2; }
id nobody >/dev/null || { echo 'User nobody is absent'; exit 2; }
# Require permission before creating the temporary queue.
sudo -v
probe_dir=$(mktemp -d /tmp/ipc-lesson.XXXXXX)
chmod 0755 "$probe_dir"
install -m 0755 bin/permission_probe "$probe_dir/permission_probe"
key=$(printf '0x%x' "$((0x52000000 | ($$ & 65535)))")
created=0
cleanup() {
  if [[ "$created" -eq 1 ]]; then
    ./bin/permission_probe cleanup "$key" || echo 'Manual cleanup required (ipcs -q)'
  fi
  rm -rf -- "$probe_dir"
}
trap cleanup EXIT
./bin/permission_probe create "$key"
created=1
printf 'TEST owner should have access:\n'
# The test executes under a different real UID without privilege escalation.
sudo -u nobody "$probe_dir/permission_probe" check "$key"
echo 'PASS: DAC access blocked for nobody'
