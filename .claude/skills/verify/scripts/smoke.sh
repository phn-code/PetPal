#!/bin/sh
# Usage: smoke.sh [seconds] [--screenshot]
# Builds the game, launches the real window from build/verify/run (a sandbox
# with an assets symlink, so the repo's own pet_stats.txt is never touched),
# keeps it up for the given seconds (default 4), then stops the process it
# started. Evidence goes to build/verify/: smoke.log (stdout and stderr) and,
# with --screenshot, screen.png of the whole screen.
set -u
root=$(cd "$(dirname "$0")/../../../.." && pwd)
secs=${1:-4}
cd "$root" && make -s all || exit 1
out="$root/build/verify"
mkdir -p "$out/run"
ln -sfn "$root/assets" "$out/run/assets"
rm -f "$out/run/pet_stats.txt" "$out/screen.png"
cd "$out/run"
"$root/build/Petpal" >"$out/smoke.log" 2>&1 &
pid=$!
sleep "$secs"
if ! kill -0 "$pid" 2>/dev/null; then
  wait "$pid"
  echo "smoke FAILED: Petpal exited early with code $?; see $out/smoke.log"
  exit 1
fi
[ "${2:-}" = --screenshot ] && screencapture -x "$out/screen.png"
kill "$pid"
wait "$pid" 2>/dev/null
if grep -qiE "not found|error|failed" "$out/smoke.log"; then
  echo "smoke FAILED: load errors in $out/smoke.log"
  exit 1
fi
echo "smoke ok: Petpal ran ${secs}s with no load errors (pid $pid, stopped)"
